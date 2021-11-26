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

template <typename ContainerT, typename FuncT, typename>
void parallelize(const ContainerT& collection, FuncT&& action, unsigned int threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxThreadCount = std::min(static_cast<std::size_t>(threadCount), std::size(collection));
  const std::size_t rangeCount     = (std::size(collection) + maxThreadCount / 2) / maxThreadCount;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    const std::size_t beginIndex = rangeCount * threadIndex;
    const std::size_t endIndex   = std::min(beginIndex + rangeCount, std::size(collection));

    threadPool.addAction([&action, beginIndex, endIndex, &promises, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IndexRange>) {
      action(IndexRange{ beginIndex, endIndex });
      promises[threadIndex].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  action(IndexRange{ 0, collection.size() });
#endif
}

template <typename ContainerT, typename FuncT, typename>
void parallelize(ContainerT& collection, FuncT&& action, unsigned int threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ThreadPool& threadPool = getDefaultThreadPool();

  const std::size_t maxThreadCount = std::min(static_cast<std::size_t>(threadCount), std::size(collection));
  const std::size_t rangeCount     = (std::size(collection) + maxThreadCount / 2) / maxThreadCount;

  std::vector<std::promise<void>> promises;
  promises.resize(maxThreadCount);

  for (std::size_t threadIndex = 0; threadIndex < maxThreadCount; ++threadIndex) {
    typename ContainerT::iterator beginIter = std::begin(collection) + rangeCount * threadIndex;
    typename ContainerT::iterator endIter   = beginIter + std::min(static_cast<std::ptrdiff_t>(rangeCount), std::distance(beginIter, std::end(collection)));

    threadPool.addAction([&action, beginIter, endIter, &promises, threadIndex] () noexcept(std::is_nothrow_invocable_v<FuncT, IterRange<ContainerT>>) {
      action(IterRange<ContainerT>(beginIter, endIter));
      promises[threadIndex].set_value();
    });
  }

  // Blocking here to wait for all threads to finish their action
  for (std::promise<void>& promise : promises)
    promise.get_future().wait();
#else
  action(IterRange<ContainerT>(collection.begin(), collection.end()));
#endif
}

} // namespace Raz::Threading
