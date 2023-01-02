#pragma once

#ifndef RAZ_CHROMATICABERRATIONRENDERPROCESS_HPP
#define RAZ_CHROMATICABERRATIONRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

template <typename T, std::size_t Size>
class Vector;
using Vec2f = Vector<float, 2>;

class ChromaticAberrationRenderProcess final : public MonoPassRenderProcess {
public:
  explicit ChromaticAberrationRenderProcess(RenderGraph& renderGraph);

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  void setStrength(float strength) const;
  void setDirection(const Vec2f& direction) const;
  void setMaskTexture(Texture2DPtr mask) const;
};

} // namespace Raz

#endif // RAZ_CHROMATICABERRATIONRENDERPROCESS_HPP
