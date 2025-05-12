#include "RaZ/Data/Grid3.hpp"
#include "RaZ/Data/MarchingCubes.hpp"
#include "RaZ/Data/Mesh.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

#include <numeric>

TEST_CASE("MarchingCubes computation", "[data]") {
  // Grids smaller than 2x2x2 are not allowed
  CHECK_THROWS(Raz::MarchingCubes::compute(Raz::Grid3b(1, 1, 1)));
  CHECK_THROWS(Raz::MarchingCubes::compute(Raz::Grid3b(2, 1, 2)));

  // Grids with all the same values produce no mesh data per se, as the configurations for all cells yield nothing
  CHECK(Raz::MarchingCubes::compute(Raz::Grid3b(3, 3, 3, false)).getSubmeshes().front().getVertexCount() == 0);
  CHECK(Raz::MarchingCubes::compute(Raz::Grid3b(3, 3, 3, true)).getSubmeshes().front().getVertexCount() == 0);

  // Creating a grid so that all configurations are represented; the number in each cell is the configuration index according to the following:
  //
  // front      back
  // 8 - 4    128 - 64
  // |   |     |     |
  // 1 - 2    16 -- 32
  //
  // O - O - X - O - X - O - O - X -- X -- O - X -- X -- X -- X -- O - O - O
  // | 0 | 6 | 9 | 4 | 8 | 2 | 5 | 12 | 10 | 7 | 15 | 13 | 14 | 11 | 3 | 1 |
  // O - O - X - O - O - O - X - O -- O -- X - X -- X -- O -- X -- X - X - O
  //
  // All back vertices to false

  Raz::Grid3b grid(17, 2, 2);
  grid.setValue(2, 0, 0, true);
  grid.setValue(2, 1, 0, true);
  grid.setValue(4, 1, 0, true);
  grid.setValue(6, 0, 0, true);
  grid.setValue(7, 1, 0, true);
  grid.setValue(8, 1, 0, true);
  grid.setValue(9, 0, 0, true);
  grid.setValue(10, 0, 0, true);
  grid.setValue(10, 1, 0, true);
  grid.setValue(11, 0, 0, true);
  grid.setValue(11, 1, 0, true);
  grid.setValue(12, 1, 0, true);
  grid.setValue(13, 0, 0, true);
  grid.setValue(13, 1, 0, true);
  grid.setValue(14, 0, 0, true);
  grid.setValue(15, 0, 0, true);

  {
    const Raz::Mesh mesh = Raz::MarchingCubes::compute(grid);
    const Raz::Submesh& submesh = mesh.getSubmeshes().front();
    const std::vector<Raz::Vertex>& vertices = submesh.getVertices();

    constexpr std::array<Raz::Vec3f, 90> expectedPositions = {
      // Nothing for configuration 0 (no corner)
      Raz::Vec3f(2.f, 0.f, 0.5f), Raz::Vec3f(1.5f, 1.f, 0.f), Raz::Vec3f(2.f, 1.f, 0.5f),
      Raz::Vec3f(1.5f, 0.f, 0.f), Raz::Vec3f(1.5f, 1.f, 0.f), Raz::Vec3f(2.f, 0.f, 0.5f),
      Raz::Vec3f(2.f, 1.f, 0.5f), Raz::Vec3f(2.5f, 0.f, 0.f), Raz::Vec3f(2.f, 0.f, 0.5f),
      Raz::Vec3f(2.5f, 1.f, 0.f), Raz::Vec3f(2.5f, 0.f, 0.f), Raz::Vec3f(2.f, 1.f, 0.5f),
      Raz::Vec3f(4.f, 0.5f, 0.f), Raz::Vec3f(3.5f, 1.f, 0.f), Raz::Vec3f(4.f, 1.f, 0.5f),
      Raz::Vec3f(4.f, 1.f, 0.5f), Raz::Vec3f(4.5f, 1.f, 0.f), Raz::Vec3f(4.f, 0.5f, 0.f),
      Raz::Vec3f(6.f, 0.f, 0.5f), Raz::Vec3f(5.5f, 0.f, 0.f), Raz::Vec3f(6.f, 0.5f, 0.f),
      Raz::Vec3f(6.5f, 0.f, 0.f), Raz::Vec3f(6.f, 0.f, 0.5f), Raz::Vec3f(6.f, 0.5f, 0.f),
      Raz::Vec3f(7.f, 0.5f, 0.f), Raz::Vec3f(6.5f, 1.f, 0.f), Raz::Vec3f(7.f, 1.f, 0.5f),
      Raz::Vec3f(7.f, 0.5f, 0.f), Raz::Vec3f(8.f, 1.f, 0.5f), Raz::Vec3f(8.f, 0.5f, 0.f),
      Raz::Vec3f(7.f, 1.f, 0.5f), Raz::Vec3f(8.f, 1.f, 0.5f), Raz::Vec3f(7.f, 0.5f, 0.f),
      Raz::Vec3f(9.f, 0.5f, 0.f), Raz::Vec3f(9.f, 0.f, 0.5f), Raz::Vec3f(8.5f, 0.f, 0.f),
      Raz::Vec3f(8.5f, 1.f, 0.f), Raz::Vec3f(8.f, 0.5f, 0.f), Raz::Vec3f(8.f, 1.f, 0.5f),
      Raz::Vec3f(9.5f, 1.f, 0.f), Raz::Vec3f(9.f, 0.f, 0.5f), Raz::Vec3f(9.f, 0.5f, 0.f),
      Raz::Vec3f(9.5f, 1.f, 0.f), Raz::Vec3f(10.f, 1.f, 0.5f), Raz::Vec3f(9.f, 0.f, 0.5f),
      Raz::Vec3f(10.f, 1.f, 0.5f), Raz::Vec3f(10.f, 0.f, 0.5f), Raz::Vec3f(9.f, 0.f, 0.5f),
      Raz::Vec3f(11.f, 0.f, 0.5f), Raz::Vec3f(10.f, 0.f, 0.5f), Raz::Vec3f(11.f, 1.f, 0.5f),
      Raz::Vec3f(10.f, 0.f, 0.5f), Raz::Vec3f(10.f, 1.f, 0.5f), Raz::Vec3f(11.f, 1.f, 0.5f),
      Raz::Vec3f(11.5f, 0.f, 0.f), Raz::Vec3f(12.f, 1.f, 0.5f), Raz::Vec3f(12.f, 0.5f, 0.f),
      Raz::Vec3f(11.5f, 0.f, 0.f), Raz::Vec3f(11.f, 0.f, 0.5f), Raz::Vec3f(12.f, 1.f, 0.5f),
      Raz::Vec3f(11.f, 0.f, 0.5f), Raz::Vec3f(11.f, 1.f, 0.5f), Raz::Vec3f(12.f, 1.f, 0.5f),
      Raz::Vec3f(12.f, 0.5f, 0.f), Raz::Vec3f(13.f, 0.f, 0.5f), Raz::Vec3f(12.5f, 0.f, 0.f),
      Raz::Vec3f(12.f, 0.5f, 0.f), Raz::Vec3f(12.f, 1.f, 0.5f), Raz::Vec3f(13.f, 0.f, 0.5f),
      Raz::Vec3f(12.f, 1.f, 0.5f), Raz::Vec3f(13.f, 1.f, 0.5f), Raz::Vec3f(13.f, 0.f, 0.5f),
      Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(13.f, 1.f, 0.5f), Raz::Vec3f(13.5f, 1.f, 0.f),
      Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(14.f, 0.f, 0.5f), Raz::Vec3f(13.f, 1.f, 0.5f),
      Raz::Vec3f(14.f, 0.f, 0.5f), Raz::Vec3f(13.f, 0.f, 0.5f), Raz::Vec3f(13.f, 1.f, 0.5f),
      Raz::Vec3f(14.f, 0.f, 0.5f), Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(15.f, 0.f, 0.5f),
      Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(14.f, 0.f, 0.5f),
      Raz::Vec3f(15.f, 0.f, 0.5f), Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(15.5f, 0.f, 0.f)
    };

    // All 3 vertices in a triangle have the same normal
    constexpr std::array<Raz::Vec3f, 30> expectedNormals = {
      Raz::Vec3f(0.7071067691f, 0.f, -0.7071067691f),
      Raz::Vec3f(0.7071067691f, 0.f, -0.7071067691f),
      Raz::Vec3f(-0.7071067691f, 0.f, -0.7071067691f),
      Raz::Vec3f(-0.7071067691f, 0.f, -0.7071067691f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, -0.5773502588f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.f, 0.7071067691f, -0.7071067691f),
      Raz::Vec3f(0.f, 0.7071067691f, -0.7071067691f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, -0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, -0.5773502588f),
      -Raz::Axis::Z,
      -Raz::Axis::Z,
      -Raz::Axis::Z,
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, -0.5773502588f),
      -Raz::Axis::Z,
      Raz::Vec3f(0.5773502588f, 0.5773502588f, -0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, -0.5773502588f),
      -Raz::Axis::Z,
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, -0.5773502588f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, -0.5773502588f),
      -Raz::Axis::Z,
      Raz::Vec3f(0.f, -0.7071067691f, -0.7071067691f),
      Raz::Vec3f(0.f, -0.7071067691f, -0.7071067691f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, -0.5773502588f)
    };

    CHECK(vertices.size() == expectedPositions.size());

    const Raz::Vec3f globalOffset(static_cast<float>(grid.getWidth() - 1) * 0.5f,
                                  static_cast<float>(grid.getHeight() - 1) * 0.5f,
                                  static_cast<float>(grid.getDepth() - 1) * 0.5f);
    for (std::size_t i = 0; i < vertices.size(); i += 3) {
      const Raz::Vec3f firstExpectedPos  = expectedPositions[i    ] - globalOffset;
      const Raz::Vec3f secondExpectedPos = expectedPositions[i + 1] - globalOffset;
      const Raz::Vec3f thirdExpectedPos  = expectedPositions[i + 2] - globalOffset;
      CHECK(vertices[i    ].position == firstExpectedPos);
      CHECK(vertices[i + 1].position == secondExpectedPos);
      CHECK(vertices[i + 2].position == thirdExpectedPos);

      const Raz::Vec3f expectedNormal = expectedNormals[i / 3];
      CHECK(vertices[i    ].normal == expectedNormal);
      CHECK(vertices[i + 1].normal == expectedNormal);
      CHECK(vertices[i + 2].normal == expectedNormal);
    }

    const std::vector<unsigned int>& indices = submesh.getTriangleIndices();
    CHECK(indices.size() == vertices.size());

    std::vector<unsigned int> expectedIndices(indices.size());
    std::iota(expectedIndices.begin(), expectedIndices.end(), 0); // Indices are incremental (0, 1, 2, 3, ...)
    CHECK(indices == expectedIndices);
  }

  // Setting all back vertices to true
  for (std::size_t widthIndex = 0; widthIndex < grid.getWidth(); ++widthIndex) {
    grid.setValue(widthIndex, 0, 1, true);
    grid.setValue(widthIndex, 1, 1, true);
  }

  {
    const Raz::Mesh mesh = Raz::MarchingCubes::compute(grid);
    const Raz::Submesh& submesh = mesh.getSubmeshes().front();
    const std::vector<Raz::Vertex>& vertices = submesh.getVertices();

    constexpr std::array<Raz::Vec3f, 102> expectedPositions = {
      Raz::Vec3f(1.f, 0.f, 0.5f), Raz::Vec3f(1.f, 1.f, 0.5f), Raz::Vec3f(0.f, 0.f, 0.5f),
      Raz::Vec3f(0.f, 0.f, 0.5f), Raz::Vec3f(1.f, 1.f, 0.5f), Raz::Vec3f(0.f, 1.f, 0.5f),
      Raz::Vec3f(1.f, 1.f, 0.5f), Raz::Vec3f(1.f, 0.f, 0.5f), Raz::Vec3f(1.5f, 0.f, 0.f),
      Raz::Vec3f(1.5f, 1.f, 0.f), Raz::Vec3f(1.f, 1.f, 0.5f), Raz::Vec3f(1.5f, 0.f, 0.f),
      Raz::Vec3f(3.f, 0.f, 0.5f), Raz::Vec3f(3.f, 1.f, 0.5f), Raz::Vec3f(2.5f, 1.f, 0.f),
      Raz::Vec3f(2.5f, 0.f, 0.f), Raz::Vec3f(3.f, 0.f, 0.5f), Raz::Vec3f(2.5f, 1.f, 0.f),
      Raz::Vec3f(4.f, 0.5f, 0.f), Raz::Vec3f(3.5f, 1.f, 0.f), Raz::Vec3f(3.f, 1.f, 0.5f),
      Raz::Vec3f(4.f, 0.5f, 0.f), Raz::Vec3f(3.f, 1.f, 0.5f), Raz::Vec3f(4.f, 0.f, 0.5f),
      Raz::Vec3f(4.f, 0.f, 0.5f), Raz::Vec3f(3.f, 1.f, 0.5f), Raz::Vec3f(3.f, 0.f, 0.5f),
      Raz::Vec3f(4.5f, 1.f, 0.f), Raz::Vec3f(4.f, 0.5f, 0.f), Raz::Vec3f(4.f, 0.f, 0.5f),
      Raz::Vec3f(4.5f, 1.f, 0.f), Raz::Vec3f(4.f, 0.f, 0.5f), Raz::Vec3f(5.f, 1.f, 0.5f),
      Raz::Vec3f(5.f, 1.f, 0.5f), Raz::Vec3f(4.f, 0.f, 0.5f), Raz::Vec3f(5.f, 0.f, 0.5f),
      Raz::Vec3f(5.5f, 0.f, 0.f), Raz::Vec3f(6.f, 0.5f, 0.f), Raz::Vec3f(6.f, 1.f, 0.5f),
      Raz::Vec3f(5.5f, 0.f, 0.f), Raz::Vec3f(6.f, 1.f, 0.5f), Raz::Vec3f(5.f, 0.f, 0.5f),
      Raz::Vec3f(5.f, 0.f, 0.5f), Raz::Vec3f(6.f, 1.f, 0.5f), Raz::Vec3f(5.f, 1.f, 0.5f),
      Raz::Vec3f(6.f, 0.5f, 0.f), Raz::Vec3f(6.5f, 0.f, 0.f), Raz::Vec3f(7.f, 0.f, 0.5f),
      Raz::Vec3f(6.f, 0.5f, 0.f), Raz::Vec3f(7.f, 0.f, 0.5f), Raz::Vec3f(6.f, 1.f, 0.5f),
      Raz::Vec3f(7.f, 0.5f, 0.f), Raz::Vec3f(6.5f, 1.f, 0.f), Raz::Vec3f(7.f, 0.f, 0.5f),
      Raz::Vec3f(6.5f, 1.f, 0.f), Raz::Vec3f(6.f, 1.f, 0.5f), Raz::Vec3f(7.f, 0.f, 0.5f),
      Raz::Vec3f(7.f, 0.f, 0.5f), Raz::Vec3f(8.f, 0.f, 0.5f), Raz::Vec3f(8.f, 0.5f, 0.f),
      Raz::Vec3f(7.f, 0.5f, 0.f), Raz::Vec3f(7.f, 0.f, 0.5f), Raz::Vec3f(8.f, 0.5f, 0.f),
      Raz::Vec3f(8.5f, 1.f, 0.f), Raz::Vec3f(8.f, 0.5f, 0.f), Raz::Vec3f(8.f, 0.f, 0.5f),
      Raz::Vec3f(8.5f, 1.f, 0.f), Raz::Vec3f(8.f, 0.f, 0.5f), Raz::Vec3f(9.f, 1.f, 0.5f),
      Raz::Vec3f(8.5f, 0.f, 0.f), Raz::Vec3f(9.f, 0.5f, 0.f), Raz::Vec3f(8.f, 0.f, 0.5f),
      Raz::Vec3f(9.f, 0.5f, 0.f), Raz::Vec3f(9.f, 1.f, 0.5f), Raz::Vec3f(8.f, 0.f, 0.5f),
      Raz::Vec3f(9.f, 1.f, 0.5f), Raz::Vec3f(9.f, 0.5f, 0.f), Raz::Vec3f(9.5f, 1.f, 0.f),
      // Nothing for configuration 255 (all corners)
      Raz::Vec3f(12.f, 0.f, 0.5f), Raz::Vec3f(12.f, 0.5f, 0.f), Raz::Vec3f(11.5f, 0.f, 0.f),
      Raz::Vec3f(12.f, 0.f, 0.5f), Raz::Vec3f(12.5f, 0.f, 0.f), Raz::Vec3f(12.f, 0.5f, 0.f),
      Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(14.f, 1.f, 0.5f), Raz::Vec3f(13.5f, 1.f, 0.f),
      Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(15.f, 1.f, 0.5f),
      Raz::Vec3f(14.f, 1.f, 0.5f), Raz::Vec3f(14.f, 0.5f, 0.f), Raz::Vec3f(15.f, 1.f, 0.5f),
      Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(15.5f, 0.f, 0.f), Raz::Vec3f(16.f, 0.f, 0.5f),
      Raz::Vec3f(15.f, 0.5f, 0.f), Raz::Vec3f(16.f, 0.f, 0.5f), Raz::Vec3f(15.f, 1.f, 0.5f),
      Raz::Vec3f(15.f, 1.f, 0.5f), Raz::Vec3f(16.f, 0.f, 0.5f), Raz::Vec3f(16.f, 1.f, 0.5f)
    };

    // All 3 vertices in a triangle have the same normal
    constexpr std::array<Raz::Vec3f, 34> expectedNormals = {
      Raz::Axis::Z,
      Raz::Axis::Z,
      Raz::Vec3f(0.7071067691f, 0.f, 0.7071067691f),
      Raz::Vec3f(0.7071067691f, 0.f, 0.7071067691f),
      Raz::Vec3f(-0.7071067691f, 0.f, 0.7071067691f),
      Raz::Vec3f(-0.7071067691f, 0.f, 0.7071067691f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Axis::Z,
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Axis::Z,
      Raz::Vec3f(0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Axis::Z,
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.f, 0.7071067691f, 0.7071067691f),
      Raz::Vec3f(0.f, 0.7071067691f, 0.7071067691f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.5773502588f, 0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(0.f, -0.7071067691f, 0.7071067691f),
      Raz::Vec3f(0.f, -0.7071067691f, 0.7071067691f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Vec3f(-0.5773502588f, -0.5773502588f, 0.5773502588f),
      Raz::Axis::Z
    };

    CHECK(vertices.size() == expectedPositions.size());

    const Raz::Vec3f globalOffset(static_cast<float>(grid.getWidth() - 1) * 0.5f,
                                  static_cast<float>(grid.getHeight() - 1) * 0.5f,
                                  static_cast<float>(grid.getDepth() - 1) * 0.5f);
    for (std::size_t i = 0; i < vertices.size(); i += 3) {
      const Raz::Vec3f firstExpectedPos  = expectedPositions[i    ] - globalOffset;
      const Raz::Vec3f secondExpectedPos = expectedPositions[i + 1] - globalOffset;
      const Raz::Vec3f thirdExpectedPos  = expectedPositions[i + 2] - globalOffset;
      CHECK(vertices[i    ].position == firstExpectedPos);
      CHECK(vertices[i + 1].position == secondExpectedPos);
      CHECK(vertices[i + 2].position == thirdExpectedPos);

      const Raz::Vec3f expectedNormal = expectedNormals[i / 3];
      CHECK(vertices[i    ].normal == expectedNormal);
      CHECK(vertices[i + 1].normal == expectedNormal);
      CHECK(vertices[i + 2].normal == expectedNormal);
    }

    const std::vector<unsigned int>& indices = submesh.getTriangleIndices();
    CHECK(indices.size() == vertices.size());

    std::vector<unsigned int> expectedIndices(indices.size());
    std::iota(expectedIndices.begin(), expectedIndices.end(), 0); // Indices are incremental (0, 1, 2, 3, ...)
    CHECK(indices == expectedIndices);
  }
}
