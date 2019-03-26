#include "catch/catch.hpp"
#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

TEST_CASE("Async basic") {
  std::future<int> res = Raz::Threading::launchAsync([] () noexcept {
    return 42;
  });

  res.wait();

  REQUIRE(res.get() == 42);
}

#endif // RAZ_THREADS_AVAILABLE
