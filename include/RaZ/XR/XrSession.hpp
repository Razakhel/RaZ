#pragma once

#ifndef RAZ_XRSESSION_HPP
#define RAZ_XRSESSION_HPP

#include "RaZ/Math/Angle.hpp"

using XrSession = struct XrSession_T*;
struct XrInstance_T;
struct XrSpace_T;
struct XrSwapchain_T;
struct XrSwapchainImageOpenGLKHR;
struct XrViewConfigurationView;

namespace Raz {

using Quaternionf = Quaternion<float>;
using Vec3f       = Vector<float, 3>;

class Texture2D;
class XrContext;

struct ViewFov {
  Radiansf angleRight;
  Radiansf angleLeft;
  Radiansf angleUp;
  Radiansf angleDown;
};
using ViewRenderFunc = std::function<std::pair<const Texture2D&, const Texture2D&>(Vec3f, Quaternionf, ViewFov)>;

class XrSession {
  friend class XrSystem;

public:
  explicit XrSession(const XrContext& context);

  void begin(unsigned int viewConfigType);
  void end();
  bool renderFrame(const std::vector<XrViewConfigurationView>& viewConfigViews,
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

  void initialize(uint64_t systemId);
  void createReferenceSpace();
  void destroyReferenceSpace();
  void createSwapchains(const std::vector<XrViewConfigurationView>& viewConfigViews);
  void destroySwapchains();
  void createSwapchainImages(XrSwapchain swapchain, SwapchainType swapchainType);
  bool renderLayer(RenderLayerInfo& layerInfo,
                   const std::vector<XrViewConfigurationView>& viewConfigViews,
                   unsigned int viewConfigType,
                   const ViewRenderFunc& viewRenderFunc);
  void copyToSwapchains(const Texture2D& colorBuffer, const Texture2D& depthBuffer, uint32_t colorSwapchainImage, uint32_t depthSwapchainImage);

  ::XrSession m_handle {};
  XrInstance m_instance {};
  int m_state {};
  bool m_isRunning = false;

  XrSpace m_localSpace {};

  std::vector<XrSwapchain> m_colorSwapchains;
  std::vector<XrSwapchain> m_depthSwapchains;
  std::unordered_map<XrSwapchain, std::vector<XrSwapchainImageOpenGLKHR>> m_swapchainImages;
};

} // namespace Raz

#endif // RAZ_XRSESSION_HPP
