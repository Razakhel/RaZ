#include "RaZ/Utils/ThreadPool.hpp"

#include <cassert>
#include <vector>

namespace Raz::Threading {

template <typename FuncT, typename... Args, typename ResultT>
std::future<ResultT> launchAsync(FuncT&& action, Args&&... args) {
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  return std::async(std::forward<FuncT>(action), std::forward<Args>(args)...);
#else
  std::promise<ResultT> promise;
  promise.set_value(action(std::forward<Args>(args)...));
  return promise.get_future();
#endif
}

template <typename BegIndexT, typename EndIndexT, typename FuncT, typename>
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, const FuncT& action, unsigned int taskCount) {
  static_assert(std::is_invocable_v<FuncT, IndexRange>, "Error: The given action must take an IndexRange as parameter");

  if (taskCount == 0)
    throw std::invalid_argument("[Threading] The number of tasks cannot be 0.");

  if (static_cast<std::ptrdiff_t>(beginIndex) >= static_cast<std::ptrdiff_t>(endIndex))
    throw std::invalid_argument("[Threading] The given index range is invalid.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const auto totalRangeCount     = static_cast<std::size_t>(endIndex) - static_cast<std::size_t>(beginIndex);
  const std::size_t maxTaskCount = std::min(static_cast<std::size_t>(taskCount), totalRangeCount);

  const std::size_t perTaskRangeCount = totalRangeCount / maxTaskCount;
  std::size_t remainderElementCount   = totalRangeCount % maxTaskCount;
  auto taskBeginIndex                 = static_cast<std::size_t>(beginIndex);

  std::vector<std::promise<void>> promises;
  promises.resize(maxTaskCount);

  for (std::size_t taskIndex = 0; taskIndex < maxTaskCount; ++taskIndex) {
    const std::size_t taskEndIndex = taskBeginIndex + perTaskRangeCount + (remainderElementCount > 0 ? 1 : 0);

    threadPool.addTask([&action, &promises, taskBeginIndex, taskEndIndex, taskIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IndexRange>) {
      action(IndexRange{ taskBeginIndex, taskEndIndex });
      promises[taskIndex].set_value();
    });

    taskBeginIndex = taskEndIndex;

    if (remainderElementCount > 0)
      --remainderElementCount;
  }

  // Blocking here waiting for all tasks to be finished
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  static_cast<void>(taskCount);
  action(IndexRange{ static_cast<std::size_t>(beginIndex), static_cast<std::size_t>(endIndex) });
#endif
}

template <typename IterT, typename FuncT, typename>
void parallelize(IterT begin, IterT end, const FuncT& action, unsigned int taskCount) {
  static_assert(std::is_invocable_v<FuncT, IterRange<IterT>>, "Error: The given action must take an IterRange as parameter");

  if (taskCount == 0)
    throw std::invalid_argument("[Threading] The number of tasks cannot be 0.");

  const auto totalRangeCount = std::distance(begin, end);

  if (totalRangeCount <= 0)
    throw std::invalid_argument("[Threading] The given iterator range is invalid.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxTaskCount = std::min(static_cast<std::size_t>(taskCount), static_cast<std::size_t>(totalRangeCount));

  const std::size_t perTaskRangeCount = static_cast<std::size_t>(totalRangeCount) / maxTaskCount;
  std::size_t remainderElementCount   = static_cast<std::size_t>(totalRangeCount) % maxTaskCount;
  IterT taskBeginIter                 = begin;

  std::vector<std::promise<void>> promises;
  promises.resize(maxTaskCount);

  for (std::size_t taskIndex = 0; taskIndex < maxTaskCount; ++taskIndex) {
    const IterT taskEndIter = std::next(taskBeginIter, static_cast<std::ptrdiff_t>(perTaskRangeCount + (remainderElementCount > 0 ? 1 : 0)));

    threadPool.addTask([&action, &promises, taskBeginIter, taskEndIter, taskIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IterRange<IterT>>) {
      action(IterRange<IterT>(taskBeginIter, taskEndIter));
      promises[taskIndex].set_value();
    });

    taskBeginIter = taskEndIter;

    if (remainderElementCount > 0)
      --remainderElementCount;
  }

  // Blocking here waiting for all tasks to be finished
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  static_cast<void>(taskCount);
  action(IterRange<IterT>(begin, end));
#endif
}

} // namespace Raz::Threading
