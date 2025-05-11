#include "RaZ/Data/Grid2.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

template <typename T>
class TestGrid2 : public Raz::Grid2<T> {
public:
  using Raz::Grid2<T>::Grid2;
  using Raz::Grid2<T>::computeIndex;
};

TEST_CASE("Grid2 basic", "[data]") {
  // A grid requires at least one element
  CHECK_THROWS(Raz::Grid2f(0, 0));
  CHECK_THROWS(Raz::Grid2f(0, 1));
  CHECK_NOTHROW(Raz::Grid2f(1, 1));

  Raz::Grid2b grid2b(2, 1);
  CHECK(grid2b.getWidth() == 2);
  CHECK(grid2b.getHeight() == 1);
  grid2b.setValue(0, 0, true);
  CHECK(grid2b.getValue(0, 0) == true);
  CHECK(grid2b.getValue(1, 0) == false);

  Raz::Grid2f grid2f(1, 2, 42.f);
  CHECK(grid2f.getWidth() == 1);
  CHECK(grid2f.getHeight() == 2);
  grid2f.setValue(0, 0, 3.f);
  CHECK(grid2f.getValue(0, 0) == 3.f);
  CHECK(grid2f.getValue(0, 1) == 42.f);
}

TEST_CASE("Grid2 index computation", "[data]") {
  // | 0  1  2 | height = 0
  // | 3  4  5 | height = 1

  const TestGrid2<int> testGrid2(3, 2);
  CHECK(testGrid2.computeIndex(0, 0) == 0);
  CHECK(testGrid2.computeIndex(1, 0) == 1);
  CHECK(testGrid2.computeIndex(0, 1) == 3);
  CHECK(testGrid2.computeIndex(2, 1) == 5);

}
