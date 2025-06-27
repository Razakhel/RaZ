#include "RaZ/Utils/Threading.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <numeric>

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

TEST_CASE("Threading parallelization index", "[utils]") {
  std::mutex mutex; // Catch's macros aren't thread-safe and must be protected

  Raz::Threading::parallelize(0, 9, [&mutex] (const Raz::Threading::IndexRange& range) {
    const std::scoped_lock lock(mutex);
    CHECK(((range.beginIndex == 0 && range.endIndex == 3)    // 3 elements (0, 1, 2)
        || (range.beginIndex == 3 && range.endIndex == 5)    // 2 elements (3, 4)
        || (range.beginIndex == 5 && range.endIndex == 7)    // 2 elements (5, 6)
        || (range.beginIndex == 7 && range.endIndex == 9))); // 2 elements (7, 8)
  }, 4);
  // Testing with a non-zero begin index
  Raz::Threading::parallelize(2, 9, [&mutex] (const Raz::Threading::IndexRange& range) {
    const std::scoped_lock lock(mutex);
    CHECK(((range.beginIndex == 2 && range.endIndex == 4)    // 2 elements (2, 4)
        || (range.beginIndex == 4 && range.endIndex == 6)    // 2 elements (4, 5)
        || (range.beginIndex == 6 && range.endIndex == 8)    // 2 elements (6, 7)
        || (range.beginIndex == 8 && range.endIndex == 9))); // 1 element (8)
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

TEST_CASE("Threading parallelization iterator", "[utils]") {
  {
    static constexpr std::array<int, 9> dummy {};
    std::mutex mutex; // Catch's macros aren't thread-safe and must be protected

    Raz::Threading::parallelize(dummy.begin(), dummy.end(), [&mutex] (const Raz::Threading::IterRange<std::array<int, 9>::const_iterator>& range) {
      const std::scoped_lock lock(mutex);
      CHECK(((range.begin() == dummy.begin() + 0 && range.end() == dummy.begin() + 3)    // 3 elements (0, 1, 2)
          || (range.begin() == dummy.begin() + 3 && range.end() == dummy.begin() + 5)    // 2 elements (3, 4)
          || (range.begin() == dummy.begin() + 5 && range.end() == dummy.begin() + 7)    // 2 elements (5, 6)
          || (range.begin() == dummy.begin() + 7 && range.end() == dummy.begin() + 9))); // 2 elements (7, 8)
    }, 4);
    // Testing with a subset of the whole range
    Raz::Threading::parallelize(dummy.begin() + 1, dummy.end() - 1, [&mutex] (const Raz::Threading::IterRange<std::array<int, 9>::const_iterator>& range) {
      const std::scoped_lock lock(mutex);
      CHECK(((range.begin() == dummy.begin() + 1 && range.end() == dummy.begin() + 3)    // 2 elements (1, 2)
          || (range.begin() == dummy.begin() + 3 && range.end() == dummy.begin() + 5)    // 2 elements (3, 4)
          || (range.begin() == dummy.begin() + 5 && range.end() == dummy.begin() + 7)    // 2 elements (5, 6)
          || (range.begin() == dummy.begin() + 7 && range.end() == dummy.begin() + 8))); // 1 element (7)
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

// Using a non-copyable type to show how it can be moved around with parallelization functions
struct NonCopyable {
  constexpr NonCopyable(int i) : val{ i } {}
  constexpr NonCopyable(const NonCopyable&) = delete;
  constexpr NonCopyable(NonCopyable&&) noexcept = default;
  constexpr NonCopyable& operator=(const NonCopyable&) = delete;
  constexpr NonCopyable& operator=(NonCopyable&&) noexcept = default;

  int val;
};

TEST_CASE("Threading parallel reduce basic", "[utils]") {
  CHECK(Raz::Threading::parallelizeReduce(0, 10, [] (const auto& range) {
    return range.endIndex - range.beginIndex;
  }, std::plus<>(), 4) == 10);

  constexpr std::array<int, 10> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

  CHECK(Raz::Threading::parallelizeReduce(data.begin(), data.end(), [] (const auto& range) {
    return std::accumulate(range.begin(), range.end(), 0);
  }, std::plus<>(), 4) == 45);

  CHECK(Raz::Threading::parallelizeReduce(data, [] (const auto& range) {
    return std::accumulate(range.begin(), range.end(), 0);
  }, std::plus<>(), 4) == 45);
}

TEST_CASE("Threading parallel reduce index", "[utils]") {
  std::array<NonCopyable, 9> data = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  std::mutex mutex; // Catch's macros aren't thread-safe and must be protected

  const std::vector<NonCopyable> result = Raz::Threading::parallelizeReduce(0, data.size(), [&mutex, &data] (const Raz::Threading::IndexRange& range) {
    {
      const std::scoped_lock lock(mutex);
      CHECK(((range.beginIndex == 0 && range.endIndex == 3)    // 3 elements (0, 1, 2)
          || (range.beginIndex == 3 && range.endIndex == 5)    // 2 elements (3, 4)
          || (range.beginIndex == 5 && range.endIndex == 7)    // 2 elements (5, 6)
          || (range.beginIndex == 7 && range.endIndex == 9))); // 2 elements (7, 8)
    }
    return std::vector<NonCopyable>(std::move_iterator(data.begin() + range.beginIndex), std::move_iterator(data.begin() + range.endIndex));
  }, [] (std::vector<NonCopyable> reduction, std::vector<NonCopyable>&& values) {
    reduction.insert(reduction.end(), std::move_iterator(values.begin()), std::move_iterator(values.end()));
    // If the returned value is either argument and taken as an rvalue ref, GCC (observed with 15.1.1) fails to move it automatically
    //  (MSVC and Clang are fine); it can still be moved explicitly. The second argument can freely be an rvalue ref if wanted
    return reduction;
  }, 4);

  CHECK(std::equal(result.begin(), result.end(), data.begin(), data.end(), [] (const NonCopyable& lhs, const NonCopyable& rhs) { return lhs.val == rhs.val; }));
}

TEST_CASE("Threading parallel reduce iterator", "[utils]") {
  std::array<NonCopyable, 9> data{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  std::mutex mutex; // Catch's macros aren't thread-safe and must be protected

  const std::vector<NonCopyable> result = Raz::Threading::parallelizeReduce(data.begin(), data.end(),
    [&mutex, &data] (const Raz::Threading::IterRange<std::array<NonCopyable, 9>::iterator>& range) {
      {
        const std::scoped_lock lock(mutex);
        CHECK(((range.begin() == data.begin() + 0 && range.end() == data.begin() + 3)    // 3 elements (0, 1, 2)
            || (range.begin() == data.begin() + 3 && range.end() == data.begin() + 5)    // 2 elements (3, 4)
            || (range.begin() == data.begin() + 5 && range.end() == data.begin() + 7)    // 2 elements (5, 6)
            || (range.begin() == data.begin() + 7 && range.end() == data.begin() + 9))); // 2 elements (7, 8)
      }
      return std::vector<NonCopyable>(std::move_iterator(range.begin()), std::move_iterator(range.end()));
    }, [] (std::vector<NonCopyable> reduction, std::vector<NonCopyable>&& values) {
      reduction.insert(reduction.end(), std::move_iterator(values.begin()), std::move_iterator(values.end()));
      return reduction;
    }, 4);

  CHECK(std::equal(result.begin(), result.end(), data.begin(), data.end(), [] (const NonCopyable& lhs, const NonCopyable& rhs) { return lhs.val == rhs.val; }));
}

TEST_CASE("Threading parallel reduce collection", "[utils]") {
  std::array<NonCopyable, 9> data{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  std::mutex mutex; // Catch's macros aren't thread-safe and must be protected

  const std::vector<NonCopyable> result = Raz::Threading::parallelizeReduce(data, [&mutex, &data] (const auto& range) {
      {
        const std::scoped_lock lock(mutex);
        CHECK(((range.begin() == data.begin() + 0 && range.end() == data.begin() + 3)    // 3 elements (0, 1, 2)
            || (range.begin() == data.begin() + 3 && range.end() == data.begin() + 5)    // 2 elements (3, 4)
            || (range.begin() == data.begin() + 5 && range.end() == data.begin() + 7)    // 2 elements (5, 6)
            || (range.begin() == data.begin() + 7 && range.end() == data.begin() + 9))); // 2 elements (7, 8)
      }
      return std::vector<NonCopyable>(std::move_iterator(range.begin()), std::move_iterator(range.end()));
    }, [] (std::vector<NonCopyable> reduction, std::vector<NonCopyable>&& values) {
      reduction.insert(reduction.end(), std::move_iterator(values.begin()), std::move_iterator(values.end()));
      return reduction;
    }, 4);

  CHECK(std::equal(result.begin(), result.end(), data.begin(), data.end(), [] (const NonCopyable& lhs, const NonCopyable& rhs) { return lhs.val == rhs.val; }));
}

#endif // RAZ_THREADS_AVAILABLE
