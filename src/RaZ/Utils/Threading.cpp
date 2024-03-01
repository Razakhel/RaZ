#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace Raz::Threading {

ThreadPool& getDefaultThreadPool() {
  static ThreadPool threadPool;
  return threadPool;
}

void parallelize(const std::function<void()>& action, unsigned int threadCount) {
  if (threadCount == 0)
    throw std::invalid_argument("[Threading] The number of threads cannot be 0.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  std::vector<std::promise<void>> promises;
  promises.resize(threadCount);

  for (unsigned int i = 0; i < threadCount; ++i) {
    threadPool.addAction([&action, &promises, i] () {
      action();
      promises[i].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  for (unsigned int i = 0; i < threadCount; ++i)
    action();
#endif
}

void parallelize(std::initializer_list<std::function<void()>> actions) {
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  std::vector<std::promise<void>> promises;
  promises.resize(actions.size());

  for (unsigned int i = 0; i < actions.size(); ++i) {
    threadPool.addAction([&actions, &promises, i] () {
      const std::function<void()>& action = *(actions.begin() + i);
      action();
      promises[i].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  for (const std::function<void()>& action : actions)
    action();
#endif
}

} // namespace Raz::Threading

#endif // RAZ_THREADS_AVAILABLE
