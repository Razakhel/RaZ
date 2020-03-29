#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace Raz::Threading {

unsigned int getSystemThreadCount() noexcept {
  const unsigned int threadCount = std::thread::hardware_concurrency();
  return std::max(threadCount, 1u); // threadCount is 0 if undefined; returning 1 thread available in this case
}

void parallelize(const std::function<void()>& action, std::size_t threadCount) {
  assert("Error: The number of threads can't be 0." && threadCount != 0);

  std::vector<std::thread> threads(threadCount);

  for (std::thread& thread : threads)
    thread = std::thread(action);

  for (std::thread& thread : threads)
    thread.join();
}

} // namespace Raz::Threading

#endif // RAZ_THREADS_AVAILABLE
