#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace Raz::Threading {

ThreadPool& getDefaultThreadPool() {
  static ThreadPool threadPool;
  return threadPool;
}

void parallelize(const std::function<void()>& action, unsigned int taskCount) {
  if (taskCount == 0)
    throw std::invalid_argument("[Threading] The number of tasks cannot be 0.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  std::vector<std::promise<void>> promises;
  promises.resize(taskCount);

  for (unsigned int taskIndex = 0; taskIndex < taskCount; ++taskIndex) {
    threadPool.addTask([&action, &promises, taskIndex] () {
      action();
      promises[taskIndex].set_value();
    });
  }

  // Blocking here waiting for all tasks to be finished
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  for (unsigned int i = 0; i < taskCount; ++i)
    action();
#endif
}

void parallelize(std::initializer_list<std::function<void()>> actions) {
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  std::vector<std::promise<void>> promises;
  promises.resize(actions.size());

  for (unsigned int taskIndex = 0; taskIndex < actions.size(); ++taskIndex) {
    threadPool.addTask([&actions, &promises, taskIndex] () {
      const std::function<void()>& action = *(actions.begin() + taskIndex);
      action();
      promises[taskIndex].set_value();
    });
  }

  // Blocking here waiting for all tasks to be finished
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  for (const std::function<void()>& action : actions)
    action();
#endif
}

} // namespace Raz::Threading

#endif // RAZ_THREADS_AVAILABLE
