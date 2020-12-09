#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Window.hpp"
#include "RaZ/World.hpp"

#include <chrono>

namespace Raz {

class Application {
public:
  explicit Application(std::size_t worldCount = 1) { m_worlds.reserve(worldCount); }

  const std::vector<World>& getWorlds() const { return m_worlds; }
  std::vector<World>& getWorlds() { return m_worlds; }
  float getDeltaTime() const { return m_deltaTime; }

  /// Adds a World into the Application.
  /// \tparam Args Types of the arguments to be forwarded to the World.
  /// \param args Arguments to be forwarded to the World.
  /// \return Reference to the newly added World.
  template <typename... Args> World& addWorld(Args&&... args);
  /// Runs the application.
  void run();
  /// Runs the application and call the given callable between each cycle.
  /// \param callback Callback to call between each cycle.
  template <typename F>
  void run(F&& callback);
  /// Runs one cycle of the application.
  /// \return True if the application is still running, false otherwise.
  bool runOnce();
  /// Tells the application to stop running.
  void quit() { m_isRunning = false; }

private:
  std::vector<World> m_worlds {};
  Bitset m_activeWorlds {};

  std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime = std::chrono::system_clock::now();
  float m_deltaTime {};
  bool m_isRunning = true;
};

} // namespace Raz

#include "RaZ/Application.inl"

#endif // RAZ_APPLICATION_HPP
