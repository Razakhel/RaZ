#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace Raz::Threading {

ThreadPool& getDefaultThreadPool() {
  static ThreadPool threadPool;
  return threadPool;
}

void parallelize(const std::function<void()>& action, unsigned int threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

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

} // namespace Raz::Threading

#endif // RAZ_THREADS_AVAILABLE
