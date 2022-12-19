#pragma once

#ifndef RAZ_RENDERTIMER_HPP
#define RAZ_RENDERTIMER_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

namespace Raz {

class RenderTimer {
public:
  RenderTimer() noexcept;
  RenderTimer(const RenderTimer&) = delete;
  RenderTimer(RenderTimer&&) noexcept = default;

  /// Starts the time measure.
  /// \note This action is not available with OpenGL ES and will do nothing.
  void start() const noexcept;
  /// Stops the time measure.
  /// \note This action is not available with OpenGL ES and will do nothing.
  void stop() const noexcept;
  /// Recovers the elapsed time (in milliseconds) of the latest measure.
  /// \warning The measure must be stopped for the actual time to be available.
  /// \see stop()
  /// \note This action is not available with OpenGL ES and will always return 0.
  /// \return Elapsed time in milliseconds.
  float recoverTime() const noexcept;

  RenderTimer& operator=(const RenderTimer&) = delete;
  RenderTimer& operator=(RenderTimer&&) noexcept = default;

  ~RenderTimer();

private:
#if !defined(USE_OPENGL_ES)
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERTIMER_HPP
