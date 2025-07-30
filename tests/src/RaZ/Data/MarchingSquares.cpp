#include "RaZ/Data/Grid2.hpp"
#include "RaZ/Data/MarchingSquares.hpp"
#include "RaZ/Data/Mesh.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <numeric>

TEST_CASE("MarchingSquares computation", "[data]") {
  // Grids smaller than 2x2 are not allowed
  CHECK_THROWS(Raz::MarchingSquares::compute(Raz::Grid2b(1, 1)));
  CHECK_THROWS(Raz::MarchingSquares::compute(Raz::Grid2b(2, 1)));

  // Grids with all the same values produce no mesh data per se, as the configurations for all cells yield nothing
  CHECK(Raz::MarchingSquares::compute(Raz::Grid2b(2, 2, false)).getSubmeshes().front().getVertexCount() == 0);
  CHECK(Raz::MarchingSquares::compute(Raz::Grid2b(2, 2, true)).getSubmeshes().front().getVertexCount() == 0);

  // Creating a grid so that all configurations are represented; the number in each cell is the configuration index according to the following:
  //
  // 8 - 4
  // |   |
  // 1 - 2
  //
  // O - O - X - O - X - O - O - X -- X -- O - X -- X -- X -- X -- O - O - O
  // | 0 | 6 | 9 | 4 | 8 | 2 | 5 | 12 | 10 | 7 | 15 | 13 | 14 | 11 | 3 | 1 |
  // O - O - X - O - O - O - X - O -- O -- X - X -- X -- O -- X -- X - X - O

  Raz::Grid2b grid(17, 2);
  grid.setValue(2, 0, true);
  grid.setValue(2, 1, true);
  grid.setValue(4, 1, true);
  grid.setValue(6, 0, true);
  grid.setValue(7, 1, true);
  grid.setValue(8, 1, true);
  grid.setValue(9, 0, true);
  grid.setValue(10, 0, true);
  grid.setValue(10, 1, true);
  grid.setValue(11, 0, true);
  grid.setValue(11, 1, true);
  grid.setValue(12, 1, true);
  grid.setValue(13, 0, true);
  grid.setValue(13, 1, true);
  grid.setValue(14, 0, true);
  grid.setValue(15, 0, true);

  const Raz::Mesh mesh = Raz::MarchingSquares::compute(grid);
  const Raz::Submesh& submesh = mesh.getSubmeshes().front();
  const std::vector<Raz::Vertex>& vertices = submesh.getVertices();

  // O ----- O ----- X ----- O ----- X ----- O ----- O ----- X ----- X ----- O ----- X ----- X ----- X ----- X ----- O ----- O ----- O < height = 1
  // |       |   |   |   |   |    \  |  /    |       |  /    |       |    \  |  /    |       |       |       |    \  |       |       |
  // |       |   |   |   |   |      \|/      |      /|/     /|-------|\     \|/      |       |      /|\      |      \|-------|\      |
  // |       |   |   |   |   |       |       |    /  |    /  |       |  \    |       |       |    /  |  \    |       |       |  \    |
  // O ----- O ----- X ----- O ----- O ----- O ----- X ----- O ----- O ----- X ----- X ----- X ----- O ----- X ----- X ----- X ----- O < height = 0
  // ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^       ^
  // 0       1       2       3       4       5       6       7       8       9      10      11      12      13      14      15      16   = width

  static constexpr std::array<Raz::Vec2f, 32> expectedPositions = {
    // Nothing for configuration 0 (no corner)
    Raz::Vec2f(1.5f, 0.f), Raz::Vec2f(1.5f, 1.f),
    Raz::Vec2f(2.5f, 0.f), Raz::Vec2f(2.5f, 1.f),
    Raz::Vec2f(3.5f, 1.f), Raz::Vec2f(4.f, 0.5f),
    Raz::Vec2f(4.f, 0.5f), Raz::Vec2f(4.5f, 1.f),
    Raz::Vec2f(5.5f, 0.f), Raz::Vec2f(6.f, 0.5f),
    Raz::Vec2f(6.f, 0.5f), Raz::Vec2f(6.5f, 1.f), Raz::Vec2f(6.5f, 0.f), Raz::Vec2f(7.f, 0.5f),
    Raz::Vec2f(7.f, 0.5f), Raz::Vec2f(8.f, 0.5f),
    Raz::Vec2f(8.f, 0.5f), Raz::Vec2f(8.5f, 0.f), Raz::Vec2f(8.5f, 1.f), Raz::Vec2f(9.f, 0.5f),
    Raz::Vec2f(9.f, 0.5f), Raz::Vec2f(9.5f, 1.f),
    // Nothing for configuration 15 (all corners)
    Raz::Vec2f(11.5f, 0.f), Raz::Vec2f(12.f, 0.5f),
    Raz::Vec2f(12.f, 0.5f), Raz::Vec2f(12.5f, 0.f),
    Raz::Vec2f(13.5f, 1.f), Raz::Vec2f(14.f, 0.5f),
    Raz::Vec2f(14.f, 0.5f), Raz::Vec2f(15.f, 0.5f),
    Raz::Vec2f(15.f, 0.5f), Raz::Vec2f(15.5f, 0.f)
  };

  // For each triplet, the first & second vertices are the same
  CHECK(vertices.size() == expectedPositions.size() + 16);

  const Raz::Vec2f globalOffset(static_cast<float>(grid.getWidth() - 1) * 0.5f, static_cast<float>(grid.getHeight() - 1) * 0.5f);
  for (std::size_t i = 0; i < vertices.size() / 3; ++i) {
    const Raz::Vec2f firstExpectedPos  = expectedPositions[i * 2    ] - globalOffset;
    const Raz::Vec2f secondExpectedPos = expectedPositions[i * 2 + 1] - globalOffset;
    CHECK(vertices[i * 3    ].position == Raz::Vec3f(firstExpectedPos, 0.f));
    CHECK(vertices[i * 3 + 1].position == Raz::Vec3f(firstExpectedPos, 0.f));
    CHECK(vertices[i * 3 + 2].position == Raz::Vec3f(secondExpectedPos, 0.f));
  }

  // This should be lines but is done as flat triangles for now
  const std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  CHECK(indices.size() == vertices.size());

  std::vector<unsigned int> expectedIndices(indices.size());
  std::iota(expectedIndices.begin(), expectedIndices.end(), 0); // Indices are incremental (0, 1, 2, 3, ...)
  CHECK(indices == expectedIndices);
}
