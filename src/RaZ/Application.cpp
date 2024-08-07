#include "RaZ/Application.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

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
  ZoneScopedN("Application::runOnce");

  const auto currentTime = std::chrono::system_clock::now();
  m_timeInfo.deltaTime   = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
  m_timeInfo.globalTime += m_timeInfo.deltaTime;
  m_lastFrameTime        = currentTime;

  m_timeInfo.substepCount = 0;
  m_remainingTime        += m_timeInfo.deltaTime;

  while (m_remainingTime >= m_timeInfo.substepTime) {
    ++m_timeInfo.substepCount;
    m_remainingTime -= m_timeInfo.substepTime;
  }

  for (std::size_t worldIndex = 0; worldIndex < m_worlds.size(); ++worldIndex) {
    if (!m_activeWorlds[worldIndex])
      continue;

    if (!m_worlds[worldIndex]->update(m_timeInfo))
      m_activeWorlds.setBit(worldIndex, false);
  }

  // Adding a frame mark registers the past frame
  // TODO: the application setup (everything up until Application::run() is called, hence including the main function) is merged with the very first frame
  //  A "fix" would be to add another FrameMark at the top of the run function, but the currently templated callback overload being in a header,
  //  including Tracy to be used in there wouldn't be a good idea
  FrameMark;

  return (m_isRunning && !m_activeWorlds.isEmpty());
}

} // namespace Raz
