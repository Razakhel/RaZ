#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include "RaZ/Data/Bitset.hpp"

#include <chrono>
#include <memory>

namespace Raz {

class World;
using WorldPtr = std::unique_ptr<World>;

class Application {
public:
  explicit Application(std::size_t worldCount = 1);

  const std::vector<WorldPtr>& getWorlds() const { return m_worlds; }
  std::vector<WorldPtr>& getWorlds() { return m_worlds; }
  float getDeltaTime() const { return m_deltaTime; }
  float getGlobalTime() const { return m_globalTime; }

  /// Adds a World into the Application.
  /// \tparam Args Types of the arguments to be forwarded to the World.
  /// \param args Arguments to be forwarded to the World.
  /// \return Reference to the newly added World.
  template <typename... Args> World& addWorld(Args&&... args);
  /// Runs the application.
  void run();
  /// Runs the application and call the given callable between each cycle.
  /// \tparam FuncT Type of the callback to call between each cycle.
  /// \param callback Callback to call between each cycle.
  template <typename FuncT> void run(FuncT&& callback);
  /// Runs one cycle of the application.
  /// \return True if the application is still running, false otherwise.
  bool runOnce();
  /// Tells the application to stop running.
  void quit() { m_isRunning = false; }

private:
  std::vector<WorldPtr> m_worlds {};
  Bitset m_activeWorlds {};

  std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime = std::chrono::system_clock::now();
  float m_deltaTime {}; ///< Time elapsed since the application's last execution, in seconds.
  float m_globalTime = 0.f; ///< Time elapsed since the application started, in seconds.
  bool m_isRunning = true;
};

} // namespace Raz

#include "RaZ/Application.inl"

#endif // RAZ_APPLICATION_HPP
