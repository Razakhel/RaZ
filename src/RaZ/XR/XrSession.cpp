#include "RaZ/Data/Color.hpp"
#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/XR/XrContext.hpp"
#include "RaZ/XR/XrSession.hpp"

#if defined(XR_OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Unknwn.h>
#endif

#include "GL/glew.h"
#if defined(XR_OS_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#elif defined(XR_OS_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace Raz {

namespace {

constexpr std::string_view swapchainCopySource = R"(
  in vec2 fragTexcoords;

  uniform sampler2D uniFinalColorBuffer;
  uniform sampler2D uniFinalDepthBuffer;

  layout(location = 0) out vec4 fragColor;

  void main() {
    fragColor     = texture(uniFinalColorBuffer, fragTexcoords).rgba;
    // Gamma uncorrection, as the swapchain seems to apply it itself
    fragColor.rgb = pow(fragColor.rgb, vec3(2.2));
    gl_FragDepth  = texture(uniFinalDepthBuffer, fragTexcoords).r;
  }
)";

const char* getResultStr(XrInstance instance, XrResult result) {
  static std::array<char, XR_MAX_RESULT_STRING_SIZE> errorStr {};
  xrResultToString(instance, result, errorStr.data());
  return errorStr.data();
}

std::string getErrorStr(const std::string& errorMsg, XrResult result, XrInstance instance) {
  return "[XrSession] " + errorMsg + ": " + getResultStr(instance, result) + " (" + std::to_string(result) + ')';
}

void checkLog(XrResult result, const std::string& errorMsg, XrInstance instance) {
  if (XR_SUCCEEDED(result))
    return;

  Logger::error(getErrorStr(errorMsg, result, instance));
}

void checkThrow(XrResult result, const std::string& errorMsg, XrInstance instance) {
  if (XR_SUCCEEDED(result))
    return;

  throw std::runtime_error(getErrorStr(errorMsg, result, instance));
}

#if defined(XR_USE_PLATFORM_WIN32)
using GraphicsBinding = XrGraphicsBindingOpenGLWin32KHR;
#elif defined(XR_USE_PLATFORM_XLIB)
using GraphicsBinding = XrGraphicsBindingOpenGLXlibKHR;
#endif

GraphicsBinding getGraphicsBinding() {
  GLFWwindow* window = glfwGetCurrentContext();
  GraphicsBinding graphicsBinding {};

#if defined(XR_USE_PLATFORM_WIN32)
  graphicsBinding.type  = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;
  graphicsBinding.hDC   = GetDC(glfwGetWin32Window(window));
  graphicsBinding.hGLRC = glfwGetWGLContext(window);
#elif defined(XR_USE_PLATFORM_XLIB)
  Display* x11Display = glfwGetX11Display();

  // TODO: some fields can't be directly filled with what GLFW exposes; see https://github.com/glfw/glfw/issues/2129

  // TODO: unless there's a way to easily recover the FBConfig from a GLXContext (related GLFW PR: https://github.com/glfw/glfw/pull/1925), it has to be done
  //  manually; see https://github.com/KhronosGroup/OpenXR-SDK-Source/blob/dfe3ad67f11ab71a64b2c75d6b7a97034b9766fd/src/common/gfxwrapper_opengl.c#L1016-L1077
  GLXFBConfig fbConfig {};
  graphicsBinding.type        = XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR;
  graphicsBinding.xDisplay    = x11Display;
  graphicsBinding.visualid    = static_cast<uint32_t>(glXGetVisualFromFBConfig(x11Display, fbConfig)->visualid);
  graphicsBinding.glxFBConfig = fbConfig;
  graphicsBinding.glxDrawable = glXGetCurrentDrawable();
  graphicsBinding.glxContext  = glfwGetGLXContext(window);
#endif

  return graphicsBinding;
}

int64_t selectColorSwapchainFormat(const std::vector<int64_t>& formats) {
  constexpr std::array<int64_t, 4> supportedColorSwapchainFormats = {
    GL_RGB10_A2,
    GL_RGBA16F,
    // The following values should only be used as a fallback, as they are linear color formats without enough bits for color depth, thus leading to banding
    GL_RGBA8,
    GL_RGBA8_SNORM
  };

  const auto formatIter = std::find_first_of(formats.cbegin(), formats.cend(),
                                             supportedColorSwapchainFormats.cbegin(), supportedColorSwapchainFormats.cend());

  if (formatIter == formats.cend())
    return 0;

  return *formatIter;
}

int64_t selectDepthSwapchainFormat(const std::vector<int64_t>& formats) {
  constexpr std::array<int64_t, 4> supportedDepthSwapchainFormats = {
    GL_DEPTH_COMPONENT32F,
    GL_DEPTH_COMPONENT32,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_COMPONENT16
  };

  const auto formatIter = std::find_first_of(formats.cbegin(), formats.cend(),
                                             supportedDepthSwapchainFormats.cbegin(), supportedDepthSwapchainFormats.cend());

  if (formatIter == formats.cend())
    return 0;

  return *formatIter;
}

} // namespace

struct XrSession::RenderLayerInfo {
  XrTime predictedDisplayTime {};
  std::vector<XrCompositionLayerBaseHeader*> layers;
  XrCompositionLayerProjection layerProjection{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
  std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
};

enum class XrSession::SwapchainType : uint8_t {
  COLOR,
  DEPTH
};

XrSession::XrSession(const XrContext& context) : m_instance{ context.m_instance },
                                                 m_swapchainCopyPass(FragmentShader::loadFromSource(swapchainCopySource), "Swapchain copy pass") {
  ZoneScopedN("XrSession::XrSession");

  Logger::debug("[XrSession] Creating session...");

  if (m_instance == XR_NULL_HANDLE)
    throw std::runtime_error("[XrSession] The XR instance must be valid");

  if (!Renderer::isInitialized())
    throw std::runtime_error("[XrSession] The renderer must be initialized");

  PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR {};
  checkLog(xrGetInstanceProcAddr(m_instance,
                                 "xrGetOpenGLGraphicsRequirementsKHR",
                                 reinterpret_cast<PFN_xrVoidFunction*>(&xrGetOpenGLGraphicsRequirementsKHR)),
           "Failed to get OpenGL graphics requirements get function",
           m_instance);
  XrGraphicsRequirementsOpenGLKHR graphicsRequirements {};
  graphicsRequirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR;
  checkLog(xrGetOpenGLGraphicsRequirementsKHR(m_instance, context.m_systemId, &graphicsRequirements),
           "Failed to get graphics requirements for OpenGL", m_instance);

  const XrVersion graphicsApiVersion = XR_MAKE_VERSION(Renderer::getMajorVersion(), Renderer::getMinorVersion(), 0);
  if (graphicsRequirements.minApiVersionSupported > graphicsApiVersion) {
    const uint16_t requiredMajorVersion = XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported);
    const uint16_t requiredMinorVersion = XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported);
    throw std::runtime_error("[XrSession] The current OpenGL version "
                             + std::to_string(Renderer::getMajorVersion()) + '.' + std::to_string(Renderer::getMinorVersion())
                             + " does not meet the minimum required version "
                             + std::to_string(requiredMajorVersion) + '.' + std::to_string(requiredMinorVersion)
                             + " for OpenXR");
  }

  const GraphicsBinding graphicsBinding = getGraphicsBinding();
  XrSessionCreateInfo sessionCreateInfo {};
  sessionCreateInfo.type        = XR_TYPE_SESSION_CREATE_INFO;
  sessionCreateInfo.next        = &graphicsBinding;
  sessionCreateInfo.createFlags = 0;
  sessionCreateInfo.systemId    = context.m_systemId;
  checkThrow(xrCreateSession(m_instance, &sessionCreateInfo, &m_handle), "Failed to create session", m_instance);

  createReferenceSpace();

  RenderShaderProgram& swapchainCopyProgram = m_swapchainCopyPass.getProgram();
  swapchainCopyProgram.setAttribute(0, "uniFinalColorBuffer");
  swapchainCopyProgram.setAttribute(1, "uniFinalDepthBuffer");
  swapchainCopyProgram.sendAttributes();

  constexpr DrawBuffer drawBuffer = DrawBuffer::COLOR_ATTACHMENT0;
  Renderer::bindFramebuffer(m_swapchainCopyPass.getFramebuffer().getIndex(), FramebufferType::DRAW_FRAMEBUFFER);
  Renderer::setDrawBuffers(1, &drawBuffer);
  Renderer::bindFramebuffer(0);

  Logger::debug("[XrSession] Created session");
}

void XrSession::begin(unsigned int viewConfigType) {
  Logger::debug("[XrSession] Beginning session...");

  XrSessionBeginInfo sessionBeginInfo {};
  sessionBeginInfo.type                         = XR_TYPE_SESSION_BEGIN_INFO;
  sessionBeginInfo.primaryViewConfigurationType = static_cast<XrViewConfigurationType>(viewConfigType);
  checkLog(xrBeginSession(m_handle, &sessionBeginInfo), "Failed to begin session", m_instance);

  Logger::debug("[XrSession] Began session");
}

void XrSession::end() {
  Logger::debug("[XrSession] Ending session...");
  checkLog(xrEndSession(m_handle), "Failed to end session", m_instance);
  Logger::debug("[XrSession] Ended session");
}

void XrSession::renderFrame(const std::vector<XrViewConfigurationView>& viewConfigViews,
                            unsigned int viewConfigType,
                            unsigned int environmentBlendMode,
                            const ViewRenderFunc& viewRenderFunc) {
  ZoneScopedN("XrSession::renderFrame");

  if (!m_isRunning)
    return;

  XrFrameWaitInfo frameWaitInfo {};
  frameWaitInfo.type = XR_TYPE_FRAME_WAIT_INFO;
  XrFrameState frameState {};
  frameState.type = XR_TYPE_FRAME_STATE;
  checkLog(xrWaitFrame(m_handle, &frameWaitInfo, &frameState), "Failed to wait for the XR frame", m_instance);

  XrFrameBeginInfo frameBeginInfo {};
  frameBeginInfo.type = XR_TYPE_FRAME_BEGIN_INFO;
  checkLog(xrBeginFrame(m_handle, &frameBeginInfo), "Failed to begin the XR frame", m_instance);

  RenderLayerInfo renderLayerInfo;
  // TODO: either the application should use this display time, or the application's global & delta times should be used here somehow
  //  See:
  //  - https://registry.khronos.org/OpenXR/specs/1.0/man/html/xrWaitFrame.html#_description
  //  - https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrTime.html
  //  - https://registry.khronos.org/OpenXR/specs/1.0/man/html/XR_KHR_convert_timespec_time.html
  renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;

  const bool isSessionActive = (m_state == XR_SESSION_STATE_SYNCHRONIZED || m_state == XR_SESSION_STATE_VISIBLE || m_state == XR_SESSION_STATE_FOCUSED);
  if (isSessionActive && frameState.shouldRender && renderLayer(renderLayerInfo, viewConfigViews, viewConfigType, viewRenderFunc))
    renderLayerInfo.layers.emplace_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&renderLayerInfo.layerProjection));

  {
    ZoneNamedN(endFrameZone, "xrEndFrame", true);
    TracyGpuZone("xrEndFrame")

    XrFrameEndInfo frameEndInfo {};
    frameEndInfo.type                 = XR_TYPE_FRAME_END_INFO;
    frameEndInfo.displayTime          = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = static_cast<XrEnvironmentBlendMode>(environmentBlendMode);
    frameEndInfo.layerCount           = static_cast<uint32_t>(renderLayerInfo.layers.size());
    frameEndInfo.layers               = renderLayerInfo.layers.data();
    checkLog(xrEndFrame(m_handle, &frameEndInfo), "Failed to end the XR frame", m_instance);
  }
}

XrSession::~XrSession() {
  if (m_handle == XR_NULL_HANDLE)
    return;

  Logger::debug("[XrSession] Destroying session...");

  destroySwapchains();
  destroyReferenceSpace();
  checkLog(xrDestroySession(m_handle), "Failed to destroy session", m_instance);

  Logger::debug("[XrSession] Destroyed session");
}

void XrSession::createSwapchains(const std::vector<XrViewConfigurationView>& viewConfigViews) {
  ZoneScopedN("XrSession::createSwapchains");

  Logger::debug("[XrSession] Creating swapchains...");

  uint32_t formatCount {};
  checkLog(xrEnumerateSwapchainFormats(m_handle, 0, &formatCount, nullptr), "Failed to get swapchain format count", m_instance);
  std::vector<int64_t> formats(formatCount);
  checkLog(xrEnumerateSwapchainFormats(m_handle, formatCount, &formatCount, formats.data()), "Failed to enumerate swapchain formats", m_instance);

  if (selectDepthSwapchainFormat(formats) == 0)
    Logger::error("[XrSession] Failed to find a supported depth swapchain format");

  m_colorSwapchains.resize(viewConfigViews.size());
  m_depthSwapchains.resize(viewConfigViews.size());
  m_swapchainImages.reserve(viewConfigViews.size());

  for (std::size_t viewIndex = 0; viewIndex < viewConfigViews.size(); ++viewIndex) {
    XrSwapchain& colorSwapchain = m_colorSwapchains[viewIndex];
    XrSwapchain& depthSwapchain = m_depthSwapchains[viewIndex];

    XrSwapchainCreateInfo swapchainCreateInfo {};
    swapchainCreateInfo.type        = XR_TYPE_SWAPCHAIN_CREATE_INFO;
    swapchainCreateInfo.createFlags = 0;
    swapchainCreateInfo.usageFlags  = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT; // Technically ignored with OpenGL
    swapchainCreateInfo.format      = selectColorSwapchainFormat(formats);
    swapchainCreateInfo.sampleCount = viewConfigViews[viewIndex].recommendedSwapchainSampleCount;
    swapchainCreateInfo.width       = viewConfigViews[viewIndex].recommendedImageRectWidth;
    swapchainCreateInfo.height      = viewConfigViews[viewIndex].recommendedImageRectHeight;
    swapchainCreateInfo.faceCount   = 1;
    swapchainCreateInfo.arraySize   = 1;
    swapchainCreateInfo.mipCount    = 1;
    checkLog(xrCreateSwapchain(m_handle, &swapchainCreateInfo, &colorSwapchain), "Failed to create color swapchain", m_instance);

    swapchainCreateInfo.createFlags = 0;
    swapchainCreateInfo.usageFlags  = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // Technically ignored with OpenGL
    swapchainCreateInfo.format      = selectDepthSwapchainFormat(formats);
    swapchainCreateInfo.sampleCount = viewConfigViews[viewIndex].recommendedSwapchainSampleCount;
    swapchainCreateInfo.width       = viewConfigViews[viewIndex].recommendedImageRectWidth;
    swapchainCreateInfo.height      = viewConfigViews[viewIndex].recommendedImageRectHeight;
    swapchainCreateInfo.faceCount   = 1;
    swapchainCreateInfo.arraySize   = 1;
    swapchainCreateInfo.mipCount    = 1;
    checkLog(xrCreateSwapchain(m_handle, &swapchainCreateInfo, &depthSwapchain), "Failed to create depth swapchain", m_instance);

    createSwapchainImages(colorSwapchain, SwapchainType::COLOR);
    createSwapchainImages(depthSwapchain, SwapchainType::DEPTH);
  }

  Logger::debug("[XrSession] Created swapchains");
}

void XrSession::destroySwapchains() {
  ZoneScopedN("XrSession::destroySwapchains");

  Logger::debug("[XrSession] Destroying swapchains...");

  for (std::size_t swapchainIndex = 0; swapchainIndex < m_colorSwapchains.size(); ++swapchainIndex) {
    checkLog(xrDestroySwapchain(m_colorSwapchains[swapchainIndex]), "Failed to destroy color swapchain", m_instance);
    checkLog(xrDestroySwapchain(m_depthSwapchains[swapchainIndex]), "Failed to destroy depth swapchain", m_instance);
  }

  m_swapchainImages.clear();

  Logger::debug("[XrSession] Destroyed swapchains");
}

void XrSession::createSwapchainImages(XrSwapchain swapchain, SwapchainType swapchainType) {
  ZoneScopedN("XrSession::createSwapchainImages");

  const std::string typeStr = (swapchainType == SwapchainType::DEPTH ? "depth" : "color");

  Logger::debug("[XrSession] Creating " + typeStr + " swapchain images...");

  uint32_t swapchainImageCount {};
  checkLog(xrEnumerateSwapchainImages(swapchain, 0, &swapchainImageCount, nullptr),
           "Failed to get " + typeStr + " swapchain image count",
           m_instance);

  std::vector<XrSwapchainImageOpenGLKHR>& images = m_swapchainImages[swapchain];
  images.resize(swapchainImageCount, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR });
  checkLog(xrEnumerateSwapchainImages(swapchain, swapchainImageCount, &swapchainImageCount,
                                      reinterpret_cast<XrSwapchainImageBaseHeader*>(images.data())),
           "Failed to enumerate " + typeStr + " swapchain images",
           m_instance);

  Logger::debug("[XrSession] Created " + typeStr + " swapchain images");
}

void XrSession::createReferenceSpace() {
  ZoneScopedN("XrSession::createReferenceSpace");

  Logger::debug("[XrSession] Creating reference space...");

  XrReferenceSpaceCreateInfo referenceSpaceCreateInfo {};
  referenceSpaceCreateInfo.type                 = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
  referenceSpaceCreateInfo.referenceSpaceType   = XR_REFERENCE_SPACE_TYPE_LOCAL;
  referenceSpaceCreateInfo.poseInReferenceSpace = { XrQuaternionf{ 0.f, 0.f, 0.f, 1.f }, XrVector3f{ 0.f, 0.f, 0.f }};
  checkLog(xrCreateReferenceSpace(m_handle, &referenceSpaceCreateInfo, &m_localSpace), "Failed to create reference space", m_instance);

  Logger::debug("[XrSession] Created reference space");
}

void XrSession::destroyReferenceSpace() {
  Logger::debug("[XrSession] Destroying reference space...");
  checkLog(xrDestroySpace(m_localSpace), "Failed to destroy space", m_instance);
  Logger::debug("[XrSession] Destroyed reference space");
}

bool XrSession::renderLayer(RenderLayerInfo& layerInfo,
                            const std::vector<XrViewConfigurationView>& viewConfigViews,
                            unsigned int viewConfigType,
                            const ViewRenderFunc& viewRenderFunc) {
  ZoneScopedN("XrSession::renderLayer");

  std::vector<XrView> views(m_swapchainImages.size(), { XR_TYPE_VIEW });

  XrViewLocateInfo viewLocateInfo {};
  viewLocateInfo.type                  = XR_TYPE_VIEW_LOCATE_INFO;
  viewLocateInfo.viewConfigurationType = static_cast<XrViewConfigurationType>(viewConfigType);
  viewLocateInfo.displayTime           = layerInfo.predictedDisplayTime;
  viewLocateInfo.space                 = m_localSpace;
  XrViewState viewState {};
  viewState.type = XR_TYPE_VIEW_STATE;
  uint32_t viewCount {};
  if (xrLocateViews(m_handle, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data()) != XR_SUCCESS) {
    Logger::error("[XrSession] Failed to locate views");
    return false;
  }

  // TODO: view state flags must be checked; see: https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrViewStateFlagBits.html#_description

  layerInfo.layerProjectionViews.resize(viewCount, { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW });

  for (uint32_t viewIndex = 0; viewIndex < viewCount; ++viewIndex) {
    const char* eyeStr = (viewCount == 1 ? "Single view"
                       : (viewIndex == 0 ? "Left eye"
                                         : "Right eye"));

    ZoneTransientN(cpuEyeZone, eyeStr, true);

    const XrSwapchain colorSwapchain = m_colorSwapchains[viewIndex];
    const XrSwapchain depthSwapchain = m_depthSwapchains[viewIndex];

    XrSwapchainImageAcquireInfo acquireInfo {};
    acquireInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
    uint32_t colorImageIndex {};
    uint32_t depthImageIndex {};
    checkLog(xrAcquireSwapchainImage(colorSwapchain, &acquireInfo, &colorImageIndex),
             "Failed to acquire image from the color swapchain",
             m_instance);
    checkLog(xrAcquireSwapchainImage(depthSwapchain, &acquireInfo, &depthImageIndex),
             "Failed to acquire image from the depth swapchain",
             m_instance);

    XrSwapchainImageWaitInfo waitInfo {};
    waitInfo.type    = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
    waitInfo.timeout = XR_INFINITE_DURATION;
    checkLog(xrWaitSwapchainImage(colorSwapchain, &waitInfo), "Failed to wait for image from the color swapchain", m_instance);
    checkLog(xrWaitSwapchainImage(depthSwapchain, &waitInfo), "Failed to wait for image from the depth swapchain", m_instance);

    const uint32_t width  = viewConfigViews[viewIndex].recommendedImageRectWidth;
    const uint32_t height = viewConfigViews[viewIndex].recommendedImageRectHeight;

    const XrView& currentView = views[viewIndex];

    XrCompositionLayerProjectionView& layerProjectionView = layerInfo.layerProjectionViews[viewIndex];
    layerProjectionView.type                      = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
    layerProjectionView.pose                      = currentView.pose;
    layerProjectionView.fov                       = currentView.fov;
    layerProjectionView.subImage.swapchain        = colorSwapchain;
    layerProjectionView.subImage.imageRect.offset = { 0, 0 };
    layerProjectionView.subImage.imageRect.extent = { static_cast<int32_t>(width), static_cast<int32_t>(height) };
    layerProjectionView.subImage.imageArrayIndex  = 0;  // Useful for multiview rendering

    TracyGpuZoneTransient(gpuEyeZone, eyeStr, true)

#if defined(RAZ_CONFIG_DEBUG)
    if (Renderer::checkVersion(4, 3))
      Renderer::pushDebugGroup(eyeStr);
#endif

    const auto& [colorBuffer, depthBuffer] = viewRenderFunc(Vec3f(currentView.pose.position.x, currentView.pose.position.y, currentView.pose.position.z),
                                                            Quaternionf(currentView.pose.orientation.w, currentView.pose.orientation.x,
                                                                        currentView.pose.orientation.y, currentView.pose.orientation.z),
                                                            ViewFov{ Radiansf(currentView.fov.angleRight), Radiansf(currentView.fov.angleLeft),
                                                                     Radiansf(currentView.fov.angleUp), Radiansf(currentView.fov.angleDown) });

    const uint32_t colorSwapchainImage = m_swapchainImages.find(colorSwapchain)->second[colorImageIndex].image;
    const uint32_t depthSwapchainImage = m_swapchainImages.find(depthSwapchain)->second[depthImageIndex].image;
    copyToSwapchains(colorBuffer, depthBuffer, colorSwapchainImage, depthSwapchainImage);

#if defined(RAZ_CONFIG_DEBUG)
    if (Renderer::checkVersion(4, 3))
      Renderer::popDebugGroup();
#endif

    XrSwapchainImageReleaseInfo releaseInfo {};
    releaseInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
    checkLog(xrReleaseSwapchainImage(colorSwapchain, &releaseInfo), "Failed to release image back to the color swapchain", m_instance);
    checkLog(xrReleaseSwapchainImage(depthSwapchain, &releaseInfo), "Failed to release image back to the depth swapchain", m_instance);
  }

  layerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT
                                       | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT; // Should be deprecated and ignored
  layerInfo.layerProjection.space      = m_localSpace;
  layerInfo.layerProjection.viewCount  = static_cast<uint32_t>(layerInfo.layerProjectionViews.size());
  layerInfo.layerProjection.views      = layerInfo.layerProjectionViews.data();

  return true;
}

void XrSession::copyToSwapchains(const Texture2D& colorBuffer, const Texture2D& depthBuffer, uint32_t colorSwapchainImage, uint32_t depthSwapchainImage) {
  ZoneScopedN("XrSession::copyToSwapchains");
  TracyGpuZone("XrSession::copyToSwapchains")

  m_swapchainCopyPass.getProgram().use();
  Renderer::activateTexture(0);
  colorBuffer.bind();
  Renderer::activateTexture(1);
  depthBuffer.bind();

  Renderer::bindFramebuffer(m_swapchainCopyPass.getFramebuffer().getIndex(), FramebufferType::DRAW_FRAMEBUFFER);
  Renderer::setFramebufferTexture2D(FramebufferAttachment::COLOR0, colorSwapchainImage, 0);
  Renderer::setFramebufferTexture2D(FramebufferAttachment::DEPTH, depthSwapchainImage, 0);

  Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);

  m_swapchainCopyPass.execute();
}

} // namespace Raz
