#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Threading.hpp"
#include "RaZ/Utils/ThreadPool.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

ThreadPool::ThreadPool(const std::string& threadNamePrefix) : ThreadPool(Threading::getSystemThreadCount(), threadNamePrefix) {}

ThreadPool::ThreadPool(unsigned int threadCount, const std::string& threadNamePrefix) {
  ZoneScopedN("ThreadPool::ThreadPool");

  Logger::debug("[ThreadPool] Initializing (with {} thread(s))...", threadCount);

  m_threads.reserve(threadCount);

  for (unsigned int threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
    m_threads.emplace_back([this, threadName = threadNamePrefix + ' ' + std::to_string(threadIndex + 1)] () {
      Threading::setCurrentThreadName(threadName);

      std::function<void()> task;

      while (true) {
        {
          std::unique_lock<std::mutex> lock(m_tasksMutex);
          m_condVar.wait(lock, [this] () { return (!m_tasks.empty() || m_shouldStop); });

          if (m_shouldStop)
            return;

          task = std::move(m_tasks.front());
          m_tasks.pop();
        }

        task();
      }
    });
  }

  Logger::debug("[ThreadPool] Initialized");
}

void ThreadPool::addTask(std::function<void()> task) {
  {
    const std::lock_guard<std::mutex> lock(m_tasksMutex);
    m_tasks.push(std::move(task));
  }

  m_condVar.notify_one();
}

ThreadPool::~ThreadPool() {
  ZoneScopedN("ThreadPool::~ThreadPool");

  Logger::debug("[ThreadPool] Destroying...");

  {
    const std::lock_guard<std::mutex> lock(m_tasksMutex);
    m_shouldStop = true;
  }

  m_condVar.notify_all();

  for (std::thread& thread : m_threads)
    thread.join();

  Logger::debug("[ThreadPool] Destroyed");
}

} // namespace Raz
