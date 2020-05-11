#include "RaZ/Application.hpp"

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Raz {

World& Application::addWorld(World world) {
  m_worlds.emplace_back(std::move(world));
  m_activeWorlds.setBit(m_worlds.size() - 1);

  return m_worlds.back();
}

void Application::run() {
#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  emscripten_set_main_loop_arg([] (void* instance) {
    static_cast<decltype(this)>(instance)->runOnce();
  }, this, 0, 1);
#else
  while (runOnce());
#endif
}

bool Application::runOnce() {
  const auto currentTime = std::chrono::system_clock::now();
  m_deltaTime            = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_lastFrameTime).count();
  m_lastFrameTime        = currentTime;

  for (std::size_t worldIndex = 0; worldIndex < m_worlds.size(); ++worldIndex) {
    if (!m_worlds[worldIndex].update(m_deltaTime))
      m_activeWorlds.setBit(worldIndex, false);
  }

  return m_isRunning && !m_activeWorlds.isEmpty();
}

} // namespace Raz
