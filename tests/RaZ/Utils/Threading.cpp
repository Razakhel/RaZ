#include "Catch.hpp"

#include "RaZ/Utils/Threading.hpp"

#include <numeric>
#include <random>

#ifdef RAZ_THREADS_AVAILABLE

namespace {

void fillRandom(std::vector<int>& values) {
  std::random_device randDevice;
  static std::mt19937 randGen(randDevice());
  std::uniform_int_distribution<int> randDist(0, static_cast<int>(values.size()) - 1);

  for (int& val : values)
    val = randDist(randGen);
}

std::size_t computeSum(const std::vector<int>& values) {
  return std::accumulate(values.cbegin(), values.cend(), static_cast<std::size_t>(0));
}

void indexParallelIncrementation(std::vector<int>& values) {
  Raz::Threading::parallelize(values, [&values] (Raz::Threading::IndexRange range) {
    for (std::size_t i = range.beginIndex; i < range.endIndex; ++i)
      ++values[i];
  }, 4);
}

void iteratorParallelIncrementation(std::vector<int>& values) {
  Raz::Threading::parallelize(values, [] (Raz::Threading::IterRange<std::vector<int>> range) {
    for (int& value : range)
      ++value;
  }, 4);
}

} // namespace

TEST_CASE("Async basic") {
  std::future<int> res = Raz::Threading::launchAsync([] () noexcept {
    return 42;
  });

  res.wait();

  CHECK(res.get() == 42);
}

TEST_CASE("Index parallelization - divisible size") {
  std::vector<int> values(2048); // Choosing a size that can be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  indexParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Index parallelization - indivisible size") {
  std::vector<int> values(2083); // Choosing a prime number as size, so that it can't be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  indexParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Iterator parallelization - divisible size") {
  std::vector<int> values(2048); // Choosing a size that can be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  iteratorParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Iterator parallelization - indivisible size") {
  std::vector<int> values(2083); // Choosing a prime number as size, so that it can't be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  iteratorParallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  CHECK(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

#endif // RAZ_THREADS_AVAILABLE
