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

  struct SwapchainInfo {
    XrSwapchain swapchain {};
    std::vector<uint32_t> images;
  };

  void createSwapchains(const std::vector<XrViewConfigurationView>& viewConfigViews);
  void destroySwapchains();
  void createSwapchainImages(SwapchainInfo& swapchainInfo, SwapchainType swapchainType);
  void createReferenceSpace();
  void destroyReferenceSpace();
  bool renderLayer(RenderLayerInfo& layerInfo,
                   const std::vector<XrViewConfigurationView>& viewConfigViews,
                   unsigned int viewConfigType,
                   const ViewRenderFunc& viewRenderFunc);

  ::XrSession m_handle {};
  XrInstance m_instance {};
  int m_state {};
  bool m_isRunning = false;

  std::vector<SwapchainInfo> m_colorSwapchainInfos;
  std::vector<SwapchainInfo> m_depthSwapchainInfos;
  std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageOpenGLKHR>>> m_swapchainImages;

  XrSpace m_localSpace {};
};

} // namespace Raz

#endif // RAZ_XRSESSION_HPP
