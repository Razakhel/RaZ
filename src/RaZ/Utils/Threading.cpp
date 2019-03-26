#include <thread>

#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace Raz {

namespace Threading {

unsigned int getSystemThreadCount() {
  const unsigned int threadCount = std::thread::hardware_concurrency();
  return std::max(threadCount, 1u); // threadCount is 0 if undefined; returning 1 thread available in this case
}

} // namespace Threading

} // namespace Raz

#endif // RAZ_THREADS_AVAILABLE
