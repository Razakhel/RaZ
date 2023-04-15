#pragma once

#ifndef RAZ_FILMGRAINRENDERPROCESS_HPP
#define RAZ_FILMGRAINRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class FilmGrainRenderProcess final : public MonoPassRenderProcess {
public:
  explicit FilmGrainRenderProcess(RenderGraph& renderGraph);

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  void setStrength(float strength) const;
};

} // namespace Raz

#endif // RAZ_FILMGRAINRENDERPROCESS_HPP
