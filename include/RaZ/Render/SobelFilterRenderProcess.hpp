#pragma once

#ifndef RAZ_SOBELFILTERRENDERPROCESS_HPP
#define RAZ_SOBELFILTERRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

/// [Sobel filter](https://en.wikipedia.org/wiki/Sobel_operator) render process.
class SobelFilterRenderProcess final : public MonoPassRenderProcess {
public:
  explicit SobelFilterRenderProcess(RenderGraph& renderGraph);

  void resizeBuffers(unsigned int width, unsigned int height) override;
  void setInputBuffer(Texture2DPtr colorBuffer);
  /// Sets the output buffer which will contain the gradient's value.
  /// \param gradientBuffer Gradient's values buffer.
  void setOutputGradientBuffer(Texture2DPtr gradientBuffer);
  /// Sets the output buffer which will contain the gradient's direction.
  ///
  ///           /--0.75--\
  ///         /            \
  ///       /                \
  ///     0.5                0/1
  ///       \                /
  ///         \            /
  ///           \--0.25--/
  ///
  /// \note The direction's values are just like those of [atan2](https://en.wikipedia.org/wiki/Atan2) (see image below), but remapped between [0; 1].
  /// \imageSize{https://upload.cppreference.com/mwiki/images/9/91/math-atan2.png, height: 20%; width: 20%;}
  /// \image html https://upload.cppreference.com/mwiki/images/9/91/math-atan2.png
  /// \param gradDirBuffer Gradient's direction buffer.
  void setOutputGradientDirectionBuffer(Texture2DPtr gradDirBuffer);
};

} // namespace Raz

#endif // RAZ_SOBELFILTERRENDERPROCESS_HPP
