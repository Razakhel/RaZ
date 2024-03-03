#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include "RaZ/World.hpp"
#include "RaZ/Data/Bitset.hpp"

#include <cassert>
#include <chrono>
#include <memory>

namespace Raz {

struct FrameTimeInfo {
  float deltaTime {};   ///< Time elapsed since the application's last execution, in seconds.
  float globalTime {};  ///< Time elapsed since the application started, in seconds.
  int substepCount {};  ///< Amount of fixed time steps to process.
  float substepTime {}; ///< Time to be used by each fixed time step, in seconds.
};

class Application {
public:
  explicit Application(std::size_t worldCount = 1);

  const std::vector<WorldPtr>& getWorlds() const { return m_worlds; }
  std::vector<WorldPtr>& getWorlds() { return m_worlds; }
  const FrameTimeInfo& getTimeInfo() const { return m_timeInfo; }

  void setFixedTimeStep(float fixedTimeStep) {
    assert("Error: Fixed time step must be positive." && fixedTimeStep > 0.f);
    m_timeInfo.substepTime = fixedTimeStep;
  }

  /// Adds a world into the application.
  /// \tparam Args Types of the arguments to be forwarded to the world.
  /// \param args Arguments to be forwarded to the world.
  /// \return Reference to the newly added world.
  template <typename... Args> World& addWorld(Args&&... args);
  /// Runs the application.
  void run();
  /// Runs the application and call the given callable on each cycle.
  /// \tparam FuncT Type of the callback to call on each cycle.
  /// \param callback Callback to call on each cycle.
  template <typename FuncT> void run(FuncT&& callback);
  /// Runs one cycle of the application.
  /// \return True if the application is still running, false otherwise.
  bool runOnce();
  /// Tells the application to stop running.
  void quit() { m_isRunning = false; }

private:
  std::vector<WorldPtr> m_worlds {};
  Bitset m_activeWorlds {};

  FrameTimeInfo m_timeInfo{ 0.f, 0.f, 0, 0.016666f }; ///< Time-related attributes for each cycle.
  std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime = std::chrono::system_clock::now();
  float m_remainingTime {}; ///< Extra time remaining after executing the systems' fixed step update.

  bool m_isRunning = true;
};

} // namespace Raz

#include "RaZ/Application.inl"

#endif // RAZ_APPLICATION_HPP
