#pragma once

#ifndef RAZ_XRSESSION_HPP
#define RAZ_XRSESSION_HPP

#include "RaZ/Math/Angle.hpp"
#include "RaZ/Render/RenderPass.hpp"

using XrSession = struct XrSession_T*;
struct XrInstance_T;
struct XrSpace_T;
struct XrSwapchain_T;
struct XrSwapchainImageOpenGLKHR;
struct XrViewConfigurationView;

namespace Raz {

template <typename T>
class Quaternion;
using Quaternionf = Quaternion<float>;

template <typename T, std::size_t Size>
class Vector;
using Vec3f = Vector<float, 3>;

class Texture2D;
class XrContext;

struct ViewFov {
  Radiansf angleRight;
  Radiansf angleLeft;
  Radiansf angleUp;
  Radiansf angleDown;
};
using ViewRenderFunc = std::function<std::pair<const Texture2D&, const Texture2D&>(const Vec3f&, const Quaternionf&, const ViewFov&)>;

class XrSession {
  friend class XrSystem;

public:
  explicit XrSession(const XrContext& context);

  void begin(unsigned int viewConfigType);
  void end();
  void renderFrame(const std::vector<XrViewConfigurationView>& viewConfigViews,
                   unsigned int viewConfigType,
                   unsigned int environmentBlendMode,
                   const ViewRenderFunc& viewRenderFunc);

  ~XrSession();

private:
  using XrInstance = XrInstance_T*;
  using XrSpace = XrSpace_T*;
  using XrSwapchain = XrSwapchain_T*;
  struct RenderLayerInfo;
  enum class SwapchainType : uint8_t;

  void createSwapchains(const std::vector<XrViewConfigurationView>& viewConfigViews);
  void destroySwapchains();
  void createSwapchainImages(XrSwapchain swapchain, SwapchainType swapchainType);
  void createReferenceSpace();
  void destroyReferenceSpace();
  bool renderLayer(RenderLayerInfo& layerInfo,
                   const std::vector<XrViewConfigurationView>& viewConfigViews,
                   unsigned int viewConfigType,
                   const ViewRenderFunc& viewRenderFunc);
  void copyToSwapchains(const Texture2D& colorBuffer, const Texture2D& depthBuffer, uint32_t colorSwapchainImage, uint32_t depthSwapchainImage);

  ::XrSession m_handle {};
  XrInstance m_instance {};
  int m_state {};
  bool m_isRunning = false;

  std::vector<XrSwapchain> m_colorSwapchains;
  std::vector<XrSwapchain> m_depthSwapchains;
  std::unordered_map<XrSwapchain, std::vector<XrSwapchainImageOpenGLKHR>> m_swapchainImages;

  XrSpace m_localSpace {};

  RenderPass m_swapchainCopyPass {};
};

} // namespace Raz

#endif // RAZ_XRSESSION_HPP
