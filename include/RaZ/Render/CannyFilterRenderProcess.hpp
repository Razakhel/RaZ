#pragma once

#ifndef RAZ_CANNYFILTERRENDERPROCESS_HPP
#define RAZ_CANNYFILTERRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

/// [Canny filter/edge detector](https://en.wikipedia.org/wiki/Canny_edge_detector) render process.
/// Detects the edges within an image given its pixels' gradient information.
class CannyFilterRenderProcess final : public MonoPassRenderProcess {
public:
  explicit CannyFilterRenderProcess(RenderGraph& renderGraph);

  void resizeBuffers(unsigned int width, unsigned int height) override;
  /// Sets the given gradient buffer as input.
  /// \param gradientBuffer Buffer containing the gradient values. Obtained from another filter such as Sobel.
  /// \see SobelFilterRenderProcess
  void setInputGradientBuffer(Texture2DPtr gradientBuffer);
  /// Sets the given gradient direction buffer as input.
  /// \param gradDirBuffer Buffer containing the gradient direction values. Obtained from another filter such as Sobel.
  /// \see SobelFilterRenderProcess
  void setInputGradientDirectionBuffer(Texture2DPtr gradDirBuffer);
  void setOutputBuffer(Texture2DPtr binaryBuffer);
  void setLowerBound(float lowerBound) const;
  void setUpperBound(float upperBound) const;
};

} // namespace Raz

#endif // RAZ_CANNYFILTERRENDERPROCESS_HPP
