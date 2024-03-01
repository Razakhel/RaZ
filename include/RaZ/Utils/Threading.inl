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
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, const FuncT& action, unsigned int threadCount) {
  static_assert(std::is_invocable_v<FuncT, IndexRange>, "Error: The given action must take an IndexRange as parameter");

  if (threadCount == 0)
    throw std::invalid_argument("[Threading] The number of threads cannot be 0.");

  if (static_cast<std::ptrdiff_t>(beginIndex) >= static_cast<std::ptrdiff_t>(endIndex))
    throw std::invalid_argument("[Threading] The given index range is invalid.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const auto totalRangeCount       = static_cast<std::size_t>(endIndex) - static_cast<std::size_t>(beginIndex);
  const std::size_t maxThreadCount = std::min(static_cast<std::size_t>(threadCount), totalRangeCount);

  const std::size_t perThreadRangeCount = totalRangeCount / maxThreadCount;
  std::size_t remainderElementCount     = totalRangeCount % maxThreadCount;
  std::size_t threadBeginIndex          = 0;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    const std::size_t threadEndIndex = threadBeginIndex + perThreadRangeCount + (remainderElementCount > 0 ? 1 : 0);

    threadPool.addAction([&action, &promises, threadBeginIndex, threadEndIndex, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IndexRange>) {
      action(IndexRange{ threadBeginIndex, threadEndIndex });
      promises[threadIndex].set_value();
    });

    threadBeginIndex = threadEndIndex;

    if (remainderElementCount > 0)
      --remainderElementCount;
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
void parallelize(IterT begin, IterT end, const FuncT& action, unsigned int threadCount) {
  static_assert(std::is_invocable_v<FuncT, IterRange<IterT>>, "Error: The given action must take an IterRange as parameter");

  if (threadCount == 0)
    throw std::invalid_argument("[Threading] The number of threads cannot be 0.");

  const auto totalRangeCount = std::distance(begin, end);

  if (totalRangeCount <= 0)
    throw std::invalid_argument("[Threading] The given iterator range is invalid.");

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxThreadCount = std::min(static_cast<std::size_t>(threadCount), static_cast<std::size_t>(totalRangeCount));

  const std::size_t perThreadRangeCount = static_cast<std::size_t>(totalRangeCount) / maxThreadCount;
  std::size_t remainderElementCount     = static_cast<std::size_t>(totalRangeCount) % maxThreadCount;
  IterT threadBeginIter                 = begin;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    const IterT threadEndIter = std::next(threadBeginIter, static_cast<std::ptrdiff_t>(perThreadRangeCount + (remainderElementCount > 0 ? 1 : 0)));

    threadPool.addAction([&action, &promises, threadBeginIter, threadEndIter, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IterRange<IterT>>) {
      action(IterRange<IterT>(threadBeginIter, threadEndIter));
      promises[threadIndex].set_value();
    });

    threadBeginIter = threadEndIter;

    if (remainderElementCount > 0)
      --remainderElementCount;
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
