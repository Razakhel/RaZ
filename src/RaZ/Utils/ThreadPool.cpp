#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Threading.hpp"
#include "RaZ/Utils/ThreadPool.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

ThreadPool::ThreadPool() : ThreadPool(Threading::getSystemThreadCount()) {}

ThreadPool::ThreadPool(unsigned int threadCount) {
  ZoneScopedN("ThreadPool::ThreadPool");

  Logger::debug("[ThreadPool] Initializing (with " + std::to_string(threadCount) + " thread(s))...");

  m_threads.reserve(threadCount);

  for (unsigned int i = 0; i < threadCount; ++i) {
    m_threads.emplace_back([this] () {
      std::function<void()> action;

      while (true) {
        {
          std::unique_lock<std::mutex> lock(m_actionsMutex);
          m_condVar.wait(lock, [this] () { return (!m_actions.empty() || m_shouldStop); });

          if (m_shouldStop)
            return;

          action = std::move(m_actions.front());
          m_actions.pop();
        }

        action();
      }
    });
  }

  Logger::debug("[ThreadPool] Initialized");
}

void ThreadPool::addAction(std::function<void()> action) {
  {
    std::lock_guard<std::mutex> lock(m_actionsMutex);
    m_actions.push(std::move(action));
  }

  m_condVar.notify_one();
}

ThreadPool::~ThreadPool() {
  ZoneScopedN("ThreadPool::~ThreadPool");

  Logger::debug("[ThreadPool] Destroying...");

  {
    std::lock_guard<std::mutex> lock(m_actionsMutex);
    m_shouldStop = true;
  }

  m_condVar.notify_all();

  for (std::thread& thread : m_threads)
    thread.join();

  Logger::debug("[ThreadPool] Destroyed");
}

} // namespace Raz
