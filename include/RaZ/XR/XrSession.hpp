#pragma once

#ifndef RAZ_XRSESSION_HPP
#define RAZ_XRSESSION_HPP

using XrSession = struct XrSession_T*;
struct XrInstance_T;
struct XrSpace_T;
struct XrSwapchain_T;
struct XrSwapchainImageOpenGLKHR;
struct XrViewConfigurationView;
enum XrEnvironmentBlendMode : int;
enum XrViewConfigurationType : int;

namespace Raz {

class XrContext;

class XrSession {
  friend class XrSystem;

public:
  explicit XrSession(const XrContext& context);

  void begin(XrViewConfigurationType viewConfigType);
  void end();
  void renderFrame(const std::vector<XrViewConfigurationView>& viewConfigViews,
                   XrViewConfigurationType viewConfig,
                   XrEnvironmentBlendMode environmentBlendMode);

  ~XrSession();

private:
  using XrInstance = XrInstance_T*;
  using XrSpace = XrSpace_T*;
  using XrSwapchain = XrSwapchain_T*;
  struct ImageViewCreateInfo;
  struct RenderLayerInfo;
  enum class SwapchainType : uint8_t;

  struct SwapchainInfo {
    XrSwapchain swapchain {};
    int64_t swapchainFormat {};
    std::vector<void*> imageViews {};
  };

  void createSwapchains(const std::vector<XrViewConfigurationView>& viewConfigViews);
  void destroySwapchains();
  void createSwapchainImages(SwapchainInfo& swapchainInfo, SwapchainType swapchainType);
  unsigned int createSwapchainImageView(const ImageViewCreateInfo& imageViewCreateInfo);
  void destroySwapchainImageView(void* imageView);
  void createReferenceSpace();
  void destroyReferenceSpace();
  bool renderLayer(RenderLayerInfo& layerInfo,
                   const std::vector<XrViewConfigurationView>& viewConfigViews,
                   XrViewConfigurationType viewConfig);

  ::XrSession m_handle {};
  XrInstance m_instance {};
  int m_state {};
  bool m_isRunning = false;

  std::vector<SwapchainInfo> m_colorSwapchainInfos;
  std::vector<SwapchainInfo> m_depthSwapchainInfos;
  std::unordered_map<XrSwapchain, std::pair<SwapchainType, std::vector<XrSwapchainImageOpenGLKHR>>> m_swapchainImages;
  std::unordered_map<unsigned int, ImageViewCreateInfo> m_imageViews;

  XrSpace m_localSpace {};
};

} // namespace Raz

#endif // RAZ_XRSESSION_HPP
