#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Utils/Logger.hpp"

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Raz {

Application::Application(std::size_t worldCount) {
  m_worlds.reserve(worldCount);
}

void Application::run() {
  Logger::debug("[Application] Running...");

#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  emscripten_set_main_loop_arg([] (void* instance) {
    static_cast<decltype(this)>(instance)->runOnce();
  }, this, 0, 1);
#else
  while (runOnce());
#endif

  Logger::debug("[Application] Exiting...");
}

bool Application::runOnce() {
  const auto currentTime = std::chrono::system_clock::now();
  m_deltaTime            = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
  m_lastFrameTime        = currentTime;
  m_globalTime          += m_deltaTime;

  int substepCount = 0;
  m_remainingTime += m_deltaTime;

  while (m_remainingTime >= m_fixedTimeStep) {
    ++substepCount;
    m_remainingTime -= m_fixedTimeStep;
  }

  const FrameTimeInfo timeInfo{ m_deltaTime, m_globalTime, substepCount, m_fixedTimeStep };

  for (std::size_t worldIndex = 0; worldIndex < m_worlds.size(); ++worldIndex) {
    if (!m_activeWorlds[worldIndex])
      continue;

    if (!m_worlds[worldIndex]->update(timeInfo))
      m_activeWorlds.setBit(worldIndex, false);
  }

  return m_isRunning && !m_activeWorlds.isEmpty();
}

} // namespace Raz
