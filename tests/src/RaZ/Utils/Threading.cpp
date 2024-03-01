#include "RaZ/Utils/Threading.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>

#ifdef RAZ_THREADS_AVAILABLE

namespace {

void checkParallelIncrementation(void (*incrementParallel)(std::vector<int>&, unsigned int)) {
  constexpr std::size_t maxValueCount = 20;

  std::vector<int> values;
  values.reserve(maxValueCount);

  for (unsigned int threadCount = 1; threadCount < 8; ++threadCount) {
    for (std::size_t valueCount = 1; valueCount < maxValueCount; ++valueCount) {
      values.resize(valueCount);
      std::fill(values.begin(), values.end(), 0);

      incrementParallel(values, threadCount);
      CHECK(std::all_of(values.begin(), values.end(), [] (int val) { return val == 1; }));
    }
  }
}

} // namespace

TEST_CASE("Threading async basic", "[utils]") {
  std::future<int> res = Raz::Threading::launchAsync([] () noexcept {
    return 42;
  });

  res.wait();

  CHECK(res.get() == 42);
}

TEST_CASE("Threading simple parallelization", "[utils]") {
  std::atomic<int> i = 0;
  Raz::Threading::parallelize([&i] () noexcept { ++i; }, 10);

  CHECK(i == 10);
}

TEST_CASE("Threading multi parallelization", "[utils]") {
  CHECK_NOTHROW(Raz::Threading::parallelize(std::initializer_list<std::function<void()>>()));

  std::atomic<int> i = 0;
  Raz::Threading::parallelize({
    [&i] () noexcept { ++i; },
    [&i] () noexcept { ++i; },
    [&i] () noexcept { ++i; },
    [&i] () noexcept { ++i; },
    [&i] () noexcept { ++i; }
  });

  CHECK(i == 5);
}

TEST_CASE("Threading index parallelization", "[utils]") {
  Raz::Threading::parallelize(0, 9, [] (const Raz::Threading::IndexRange& range) {
    CHECK(((range.beginIndex == 0 && range.endIndex == 3)
        || (range.beginIndex == 3 && range.endIndex == 5)
        || (range.beginIndex == 5 && range.endIndex == 7)
        || (range.beginIndex == 7 && range.endIndex == 9)));
  }, 4);
  CHECK_THROWS(Raz::Threading::parallelize(0, 1, [] (const auto&) noexcept {}, 0)); // Given 0 as thread count
  CHECK_THROWS(Raz::Threading::parallelize(0, 0, [] (const auto&) noexcept {}, 1)); // Invalid range

  checkParallelIncrementation([] (std::vector<int>& values, unsigned int threadCount) {
    Raz::Threading::parallelize(0, values.size(), [&values] (const Raz::Threading::IndexRange& range) noexcept {
      for (std::size_t i = range.beginIndex; i < range.endIndex; ++i)
        ++values[i];
    }, threadCount);
  });
}

TEST_CASE("Threading iterator parallelization", "[utils]") {
  {
    static constexpr std::array<int, 9> dummy {};
    Raz::Threading::parallelize(dummy.begin(), dummy.end(), [] (const Raz::Threading::IterRange<std::array<int, 9>::const_iterator>& range) {
      CHECK(((range.begin() == dummy.begin() + 0 && range.end() == dummy.begin() + 3)
          || (range.begin() == dummy.begin() + 3 && range.end() == dummy.begin() + 5)
          || (range.begin() == dummy.begin() + 5 && range.end() == dummy.begin() + 7)
          || (range.begin() == dummy.begin() + 7 && range.end() == dummy.begin() + 9)));
    }, 4);
    CHECK_THROWS(Raz::Threading::parallelize(dummy.begin(), dummy.end(), [](const auto&) noexcept {}, 0)); // Given 0 as thread count
    CHECK_THROWS(Raz::Threading::parallelize(dummy.begin(), dummy.begin(), [](const auto&) noexcept {}, 1)); // Invalid range
  }

  checkParallelIncrementation([] (std::vector<int>& values, unsigned int threadCount) {
    Raz::Threading::parallelize(values.begin(), values.end(), [] (const Raz::Threading::IterRange<std::vector<int>::iterator>& range) noexcept {
      for (int& value : range)
        ++value;
    }, threadCount);
  });
}

#endif // RAZ_THREADS_AVAILABLE
