#pragma once

#ifndef RAZ_SSAORENDERPROCESS_HPP
#define RAZ_SSAORENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class SsaoRenderProcess final : public MonoPassRenderProcess {
public:
  explicit SsaoRenderProcess(RenderGraph& renderGraph);

  void setInputDepthBuffer(Texture2DPtr depthBuffer);
  void setInputNormalBuffer(Texture2DPtr normalBuffer);
  void setOutputBuffer(Texture2DPtr outputBuffer);
};

} // namespace Raz

#endif // RAZ_SSAORENDERPROCESS_HPP
