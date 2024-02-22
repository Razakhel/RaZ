#include "Catch.hpp"

#include "RaZ/Utils/Threading.hpp"

#include <numeric>
#include <random>

#ifdef RAZ_THREADS_AVAILABLE

namespace {

void fillRandom(std::vector<int>& values) {
  static std::mt19937 randGen(std::random_device{}());
  std::uniform_int_distribution<int> randDist(0, static_cast<int>(values.size()) - 1);

  for (int& val : values)
    val = randDist(randGen);
}

std::size_t computeSum(const std::vector<int>& values) {
  return std::accumulate(values.cbegin(), values.cend(), static_cast<std::size_t>(0));
}

void indexParallelIncrementation(std::vector<int>& values) {
  Raz::Threading::parallelize(0, values.size(), [&values] (const Raz::Threading::IndexRange& range) noexcept {
    for (std::size_t i = range.beginIndex; i < range.endIndex; ++i)
      ++values[i];
  }, 4);
}

void iteratorParallelIncrementation(std::vector<int>& values) {
  Raz::Threading::parallelize(values, [] (const Raz::Threading::IterRange<std::vector<int>::iterator>& range) noexcept {
    for (int& value : range)
      ++value;
  }, 4);
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

TEST_CASE("Threading index parallelization - divisible size", "[utils]") {
  std::vector<int> values(2048); // Choosing a size that can be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  indexParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Threading index parallelization - indivisible size", "[utils]") {
  std::vector<int> values(2083); // Choosing a prime number as size, so that it can't be divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  indexParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Threading iterator parallelization - divisible size", "[utils]") {
  std::vector<int> values(2048); // Choosing a size that can be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  iteratorParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Threading iterator parallelization - indivisible size", "[utils]") {
  std::vector<int> values(2083); // Choosing a prime number as size, so that it can't be divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  iteratorParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Threading parallelization empty input", "[utils]") {
  CHECK_NOTHROW(Raz::Threading::parallelize(std::initializer_list<std::function<void()>>()));

  CHECK_THROWS(Raz::Threading::parallelize(0, 0, [] (const Raz::Threading::IndexRange&) noexcept {}));

  std::vector<int> values;
  REQUIRE(values.begin() == values.end());
  CHECK_THROWS(Raz::Threading::parallelize(values.begin(), values.end(), [] (const Raz::Threading::IterRange<std::vector<int>::iterator>&) noexcept {}));
  CHECK_THROWS(Raz::Threading::parallelize(values, [] (const Raz::Threading::IterRange<std::vector<int>::iterator>&) noexcept {}));
}

#endif // RAZ_THREADS_AVAILABLE
