#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Window.hpp"
#include "RaZ/World.hpp"

namespace Raz {

class Application {
public:
  explicit Application(std::size_t worldCount = 1) { m_worlds.reserve(worldCount); }

  const std::vector<World>& getWorlds() const { return m_worlds; }
  std::vector<World>& getWorlds() { return m_worlds; }
  float getDeltaTime() const { return m_deltaTime; }

  World& addWorld(World&& world);
  bool run();
  void quit();

private:
  std::vector<World> m_worlds {};

  std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime {};
  float m_deltaTime {};
  bool m_running = true;
};

} // namespace Raz

#endif // RAZ_APPLICATION_HPP
