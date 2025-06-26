#pragma once

#ifndef RAZ_THREADPOOL_HPP
#define RAZ_THREADPOOL_HPP

// <thread> must be included first, since it creates the definition checked below
#include <thread>

// std::thread is not available on MinGW with Win32 threads
#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
#pragma message("Warning: Threads are not available with your compiler; check that you're using POSIX threads and not Win32 ones.")
#else
#define RAZ_THREADS_AVAILABLE
#endif

#if defined(RAZ_THREADS_AVAILABLE)

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

namespace Raz {

class ThreadPool {
public:
  /// Creates a thread pool with a number of threads determined by the current hardware.
  /// \param threadNamePrefix Name that will be given to each thread, followed by their respective index.
  explicit ThreadPool(const std::string& threadNamePrefix = "Pool worker");
  /// Creates a thread pool with the given number of threads.
  /// \param threadCount Number of threads to create.
  /// \param threadNamePrefix Name that will be given to each thread, followed by their respective index.
  explicit ThreadPool(unsigned int threadCount, const std::string& threadNamePrefix = "Pool worker");

  /// Enqueues a task to be executed by an available thread.
  /// \param task Task to be executed.
  void addTask(std::function<void()> task);

  ~ThreadPool();

private:
  std::vector<std::thread> m_threads {};
  bool m_shouldStop = false;

  std::mutex m_tasksMutex {};
  std::condition_variable m_condVar {};
  std::queue<std::function<void()>> m_tasks {};
};

} // namespace Raz

#endif // RAZ_THREADS_AVAILABLE

#endif // RAZ_THREADPOOL_HPP
