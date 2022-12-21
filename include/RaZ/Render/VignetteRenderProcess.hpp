#pragma once

#ifndef RAZ_VIGNETTERENDERPROCESS_HPP
#define RAZ_VIGNETTERENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class Color;

class VignetteRenderProcess final : public MonoPassRenderProcess {
public:
  explicit VignetteRenderProcess(RenderGraph& renderGraph);

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  void setStrength(float strength) const;
  void setOpacity(float opacity) const;
  void setColor(const Color& color) const;
};

} // namespace Raz

#endif // RAZ_VIGNETTERENDERPROCESS_HPP
