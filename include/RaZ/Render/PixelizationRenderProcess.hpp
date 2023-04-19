#pragma once

#ifndef RAZ_PIXELIZATIONRENDERPROCESS_HPP
#define RAZ_PIXELIZATIONRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class PixelizationRenderProcess final : public MonoPassRenderProcess {
public:
  explicit PixelizationRenderProcess(RenderGraph& renderGraph);

  void resizeBuffers(unsigned int width, unsigned int height) override;
  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  /// Sets the pixelization strength.
  /// \param strength Strength value between 0 (no pixelization) and 1 (the whole screen is taken by one single pixel).
  void setStrength(float strength) const;
};

} // namespace Raz

#endif // RAZ_PIXELIZATIONRENDERPROCESS_HPP
