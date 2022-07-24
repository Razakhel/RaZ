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
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, FuncT&& action, unsigned int threadCount) {
  static_assert(std::is_invocable_v<FuncT, IndexRange>, "Error: The given action must take an IndexRange as parameter");

  assert("Error: The number of threads can't be 0." && threadCount != 0);

  if (static_cast<std::ptrdiff_t>(beginIndex) >= static_cast<std::ptrdiff_t>(endIndex))
    throw std::invalid_argument("Error: The given index range is invalid");

  const auto totalRangeCount = static_cast<std::size_t>(endIndex) - static_cast<std::size_t>(beginIndex);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxThreadCount   = std::min(static_cast<std::size_t>(threadCount), totalRangeCount);
  const std::size_t threadRangeCount = (totalRangeCount + maxThreadCount / 2) / maxThreadCount;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    const std::size_t threadBeginIndex = beginIndex + threadRangeCount * threadIndex;
    const std::size_t threadEndIndex   = std::min(threadBeginIndex + threadRangeCount, totalRangeCount);

    threadPool.addAction([&action, threadBeginIndex, threadEndIndex, &promises, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IndexRange>) {
      action(IndexRange{ threadBeginIndex, threadEndIndex });
      promises[threadIndex].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  static_cast<void>(threadCount);
  action(IndexRange{ static_cast<std::size_t>(beginIndex), static_cast<std::size_t>(endIndex) });
#endif
}

template <typename IterT, typename FuncT, typename>
void parallelize(IterT begin, IterT end, FuncT&& action, unsigned int threadCount) {
  static_assert(std::is_invocable_v<FuncT, IterRange<IterT>>, "Error: The given action must take an IterRange as parameter");

  assert("Error: The number of threads can't be 0." && threadCount != 0);

  const auto totalRangeCount = std::distance(begin, end);

  if (totalRangeCount <= 0)
    throw std::invalid_argument("Error: The given iterator range is invalid");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxThreadCount      = std::min(static_cast<std::size_t>(threadCount), static_cast<std::size_t>(totalRangeCount));
  const std::ptrdiff_t threadRangeCount = (totalRangeCount + maxThreadCount / 2) / maxThreadCount;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    const IterT threadBeginIter = begin + threadRangeCount * threadIndex;
    const IterT threadEndIter   = threadBeginIter + std::min(threadRangeCount, std::distance(threadBeginIter, end));

    threadPool.addAction([&action, threadBeginIter, threadEndIter, &promises, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IterRange<IterT>>) {
      action(IterRange<IterT>(threadBeginIter, threadEndIter));
      promises[threadIndex].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  static_cast<void>(threadCount);
  action(IterRange<IterT>(begin, end));
#endif
}

} // namespace Raz::Threading
