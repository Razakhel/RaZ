#include "Catch.hpp"

#include "RaZ/Utils/Threading.hpp"
#include "RaZ/Utils/ThreadPool.hpp"

#ifdef RAZ_THREADS_AVAILABLE

TEST_CASE("ThreadPool basic", "[utils]") {
  Raz::ThreadPool pool;
  std::atomic<int> i = 0;

  pool.addAction([&i] () noexcept { ++i; });
  pool.addAction([&i] () noexcept { ++i; });
  pool.addAction([&i] () noexcept { ++i; });
  Raz::Threading::sleep(10); // Waiting a bit for the result to be available

  CHECK(i == 3);
}

#endif // RAZ_THREADS_AVAILABLE
