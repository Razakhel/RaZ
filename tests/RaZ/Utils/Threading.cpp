#include <numeric>
#include <random>

#include "catch/catch.hpp"
#include "RaZ/Utils/Threading.hpp"

#ifdef RAZ_THREADS_AVAILABLE

namespace {

void fillRandom(std::vector<int>& values) {
  std::random_device randDevice;
  std::mt19937 randGen(randDevice());
  std::uniform_int_distribution<int> randDist(0, static_cast<int>(values.size()) - 1);

  for (int& val : values)
    val = randDist(randGen);
}

std::size_t computeSum(const std::vector<int>& values) {
  return std::accumulate(values.cbegin(), values.cend(), static_cast<std::size_t>(0));
}

void parallelIncrementation(std::vector<int>& values) {
  Raz::Threading::parallelize(values, [&values] (Raz::Threading::IndexRange range) {
    for (std::size_t i = range.beginIndex; i < range.endIndex; ++i)
      ++values[i];
  }, 4);
}

} // namespace

TEST_CASE("Async basic") {
  std::future<int> res = Raz::Threading::launchAsync([] () noexcept {
    return 42;
  });

  res.wait();

  REQUIRE(res.get() == 42);
}

TEST_CASE("Parallelize (with indices) divisible vector increment") {
  std::vector<int> values(2048); // Choosing a size that can be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  parallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  REQUIRE(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

TEST_CASE("Parallelize (with indices) indivisible vector increment") {
  std::vector<int> values(2083); // Choosing a prime number as size, so that it can't be easily divided
  fillRandom(values);

  const std::size_t sumBeforeIncrement = computeSum(values);
  parallelIncrementation(values);
  const std::size_t sumAfterIncrement = computeSum(values);

  REQUIRE(sumBeforeIncrement + values.size() == sumAfterIncrement);
}

#endif // RAZ_THREADS_AVAILABLE
