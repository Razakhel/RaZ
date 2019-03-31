#include <vector>

namespace Raz {

namespace Threading {

template <typename Func, typename... Args, typename ResultType>
std::future<ResultType> launchAsync(Func action, Args&&... args) {
  return std::async(std::move(action), std::forward<Args>(args)...);
}

template <typename T>
void parallelize(const T& collection, const std::function<void(IndexRange)>& action, std::size_t threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

  std::vector<std::thread> threads(std::min(threadCount, collection.size()));

  // This performs a mathematical round: if the result of the division gives a number below X.5, ceil it; otherwise, round it
  const std::size_t rangeCount = (collection.size() + threadCount / 2) / threadCount;

  std::size_t beginIndex = 0;
  std::size_t endIndex   = rangeCount;

  // Creating one thread per slice, minus one to avoid getting one more element in the last thread if the size is indivisible
  for (std::size_t threadIndex = 0; threadIndex < threads.size() - 1; ++threadIndex) {
    threads[threadIndex] = std::thread(action, IndexRange{ beginIndex, endIndex });

    beginIndex = endIndex;
    endIndex  += rangeCount;
  }

  // The last thread is created independently with just the remaining number of elements
  threads.back() = std::thread(action, IndexRange{ beginIndex, std::min(endIndex, collection.size()) });

  for (std::thread& thread : threads)
    thread.join();
}

} // namespace Threading

} // namespace Raz
