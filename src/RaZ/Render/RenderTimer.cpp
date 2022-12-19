#if !defined(USE_OPENGL_ES)
#include "RaZ/Render/Renderer.hpp"
#endif
#include "RaZ/Render/RenderTimer.hpp"

namespace Raz {

RenderTimer::RenderTimer() noexcept {
#if !defined(USE_OPENGL_ES)
  Renderer::generateQuery(m_index);
#endif
}

void RenderTimer::start() const noexcept {
#if !defined(USE_OPENGL_ES)
  Renderer::beginQuery(QueryType::TIME_ELAPSED, m_index);
#endif
}

void RenderTimer::stop() const noexcept {
#if !defined(USE_OPENGL_ES)
  Renderer::endQuery(QueryType::TIME_ELAPSED);
#endif
}

float RenderTimer::recoverTime() const noexcept {
#if !defined(USE_OPENGL_ES)
  int64_t time {};
  Renderer::recoverQueryResult(m_index, time);

  return (static_cast<float>(time) / 1'000'000.f);
#else
  return 0;
#endif
}

RenderTimer::~RenderTimer() {
#if !defined(USE_OPENGL_ES)
  Renderer::deleteQuery(m_index);
#endif
}

} // namespace Raz
