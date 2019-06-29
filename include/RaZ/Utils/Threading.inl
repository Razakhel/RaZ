#include <vector>

namespace Raz::Threading {

template <typename Func, typename... Args, typename ResultType>
std::future<ResultType> launchAsync(Func action, Args&&... args) {
  return std::async(std::move(action), std::forward<Args>(args)...);
}

template <typename ContainerType>
void parallelize(const ContainerType& collection, const std::function<void(IndexRange)>& action, std::size_t threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

  std::vector<std::thread> threads(std::min(threadCount, std::size(collection)));

  // This performs a mathematical round: if the result of the division gives a number below X.5, ceil it; otherwise, round it
  const std::size_t rangeCount = (std::size(collection) + threadCount / 2) / threadCount;

  std::size_t beginIndex = 0;
  std::size_t endIndex   = rangeCount;

  // Creating one thread per slice, minus one to avoid getting one more element in the last thread if the size is indivisible
  for (std::size_t threadIndex = 0; threadIndex < threads.size() - 1; ++threadIndex) {
    threads[threadIndex] = std::thread(action, IndexRange{ beginIndex, endIndex });

    beginIndex = endIndex;
    endIndex  += rangeCount;
  }

  // The last thread is created independently with just the remaining number of elements
  threads.back() = std::thread(action, IndexRange{ beginIndex, std::min(endIndex, std::size(collection)) });

  for (std::thread& thread : threads)
    thread.join();
}

template <typename ContainerType>
void parallelize(ContainerType& collection, const std::function<void(IterRange<std::common_type_t<ContainerType>>)>& action, std::size_t threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

  std::vector<std::thread> threads(std::min(threadCount, std::size(collection)));

  // This performs a mathematical round: if the result of the division gives a number below X.5, ceil it; otherwise, round it
  const std::size_t rangeCount = (std::size(collection) + threadCount / 2) / threadCount;

  for (std::size_t threadIndex = 0; threadIndex < threads.size(); ++threadIndex) {
    typename ContainerType::iterator beginIter = std::begin(collection) + rangeCount * threadIndex;
    typename ContainerType::iterator endIter   = beginIter + rangeCount;

    threads[threadIndex] = std::thread(action, IterRange<ContainerType>(beginIter, endIter));
  }

  for (std::thread& thread : threads)
    thread.join();
}

} // namespace Raz::Threading
