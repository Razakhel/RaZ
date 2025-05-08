#include "RaZ/Data/Grid3.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

template <typename T>
class TestGrid3 : public Raz::Grid3<T> {
public:
  using Raz::Grid3<T>::Grid3;
  using Raz::Grid3<T>::computeIndex;
};

TEST_CASE("Grid3 basic", "[data]") {
  // A grid requires at least one element
  CHECK_THROWS(Raz::Grid3f(0, 0, 0));
  CHECK_THROWS(Raz::Grid3f(0, 1, 1));
  CHECK_NOTHROW(Raz::Grid3f(1, 1, 1));

  Raz::Grid3b grid3b(2, 1, 1);
  CHECK(grid3b.getWidth() == 2);
  CHECK(grid3b.getHeight() == 1);
  CHECK(grid3b.getDepth() == 1);
  grid3b.setValue(0, 0, 0, true);
  CHECK(grid3b.getValue(0, 0, 0) == true);
  CHECK(grid3b.getValue(1, 0, 0) == false);

  Raz::Grid3f grid3f(1, 1, 2, 42.f);
  CHECK(grid3f.getWidth() == 1);
  CHECK(grid3f.getHeight() == 1);
  CHECK(grid3f.getDepth() == 2);
  grid3f.setValue(0, 0, 0, 3.f);
  CHECK(grid3f.getValue(0, 0, 0) == 3.f);
  CHECK(grid3f.getValue(0, 0, 1) == 42.f);
}

TEST_CASE("Grid3 index computation", "[data]") {
  //  depth = 0    depth = 1
  // | 0  1  2 | | 12 13 14 | height = 0
  // | 3  4  5 | | 15 16 17 | height = 1
  // | 6  7  8 | | 18 19 20 | height = 2
  // | 9 10 11 | | 21 22 23 | height = 3

  const TestGrid3<int> testGrid3(3, 4, 2);
  CHECK(testGrid3.computeIndex(0, 0, 0) == 0);
  CHECK(testGrid3.computeIndex(1, 0, 0) == 1);
  CHECK(testGrid3.computeIndex(0, 1, 0) == 3);
  CHECK(testGrid3.computeIndex(0, 0, 1) == 12);
  CHECK(testGrid3.computeIndex(2, 3, 1) == 23);

}
