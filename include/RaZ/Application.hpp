#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include <chrono>

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

  /// Adds a World into the Application.
  /// \param world World to be added.
  /// \return Reference to the newly added World.
  World& addWorld(World world);
  /// Runs the application.
  /// \return True if the application is still running, false otherwise.
  bool run();
  /// Tells the application to stop running.
  void quit() { m_isRunning = false; }

private:
  std::vector<World> m_worlds {};
  Bitset m_activeWorlds {};

  std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime {};
  float m_deltaTime {};
  bool m_isRunning = true;
};

} // namespace Raz

#endif // RAZ_APPLICATION_HPP
