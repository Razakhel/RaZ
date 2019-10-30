#include "RaZ/Application.hpp"

namespace Raz {

World& Application::addWorld(World&& world) {
  m_worlds.emplace_back(std::move(world));
  m_activeWorlds.setBit(m_worlds.size() - 1);

  return m_worlds.back();
}

bool Application::run() {
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
