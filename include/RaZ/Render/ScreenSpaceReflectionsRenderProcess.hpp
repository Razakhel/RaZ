#pragma once

#ifndef RAZ_SCREENSPACEREFLECTIONSRENDERPROCESS_HPP
#define RAZ_SCREENSPACEREFLECTIONSRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class ScreenSpaceReflectionsRenderProcess final : public MonoPassRenderProcess {
public:
  explicit ScreenSpaceReflectionsRenderProcess(RenderGraph& renderGraph);

  void setInputDepthBuffer(Texture2DPtr depthBuffer);
  void setInputColorBuffer(Texture2DPtr colorBuffer);
  void setInputBlurredColorBuffer(Texture2DPtr blurredColorBuffer);
  void setInputNormalBuffer(Texture2DPtr normalBuffer);
  void setInputSpecularBuffer(Texture2DPtr specularBuffer);
  void setOutputBuffer(Texture2DPtr outputBuffer);
};

} // namespace Raz

#endif // RAZ_SCREENSPACEREFLECTIONSRENDERPROCESS_HPP
