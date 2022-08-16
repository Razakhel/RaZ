#pragma once

#ifndef RAZ_SSRRENDERPROCESS_HPP
#define RAZ_SSRRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class SsrRenderProcess final : public MonoPassRenderProcess {
public:
  explicit SsrRenderProcess(RenderGraph& renderGraph);

  void setInputDepthBuffer(Texture2DPtr depthBuffer);
  void setInputColorBuffer(Texture2DPtr colorBuffer);
  void setInputNormalBuffer(Texture2DPtr normalBuffer);
  void setInputSpecularBuffer(Texture2DPtr specularBuffer);
  void setOutputBuffer(Texture2DPtr outputBuffer);
};

} // namespace Raz

#endif // RAZ_SSRRENDERPROCESS_HPP
