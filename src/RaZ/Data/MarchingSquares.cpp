#include "RaZ/Data/Grid2.hpp"
#include "RaZ/Data/MarchingSquares.hpp"
#include "RaZ/Data/Mesh.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

namespace {

constexpr std::array<Vec2f, 4> edgeVertices = {
  Vec2f(0.5f, 0.f), // Bottom
  Vec2f(1.f, 0.5f), // Right
  Vec2f(0.5f, 1.f), // Top
  Vec2f(0.f, 0.5f)  // Left
};

enum VertexPosition {
  NONE   = -1,
  BOTTOM = 0,
  RIGHT  = 1,
  TOP    = 2,
  LEFT   = 3
};

constexpr std::array<std::array<int8_t, 4>, 16> linesIndices = {{
  { NONE,   NONE,   NONE,   NONE  }, // 0000
  { LEFT,   BOTTOM, NONE,   NONE  }, // 0001
  { BOTTOM, RIGHT,  NONE,   NONE  }, // 0010
  { LEFT,   RIGHT,  NONE,   NONE  }, // 0011
  { TOP,    RIGHT,  NONE,   NONE  }, // 0100
  { LEFT,   TOP,    BOTTOM, RIGHT }, // 0101
  { BOTTOM, TOP,    NONE,   NONE  }, // 0110
  { LEFT,   TOP,    NONE,   NONE  }, // 0111
  { LEFT,   TOP,    NONE,   NONE  }, // 1000
  { BOTTOM, TOP,    NONE,   NONE  }, // 1001
  { LEFT,   BOTTOM, TOP,    RIGHT }, // 1010
  { TOP,    RIGHT,  NONE,   NONE  }, // 1011
  { LEFT,   RIGHT,  NONE,   NONE  }, // 1100
  { BOTTOM, RIGHT,  NONE,   NONE  }, // 1101
  { LEFT,   BOTTOM, NONE,   NONE  }, // 1110
  { NONE,   NONE,   NONE,   NONE  }  // 1111
}};

uint8_t computeCellConfiguration(const Grid2b& grid, std::size_t widthIndex, std::size_t heightIndex) {
  // Computing a single number according to the corners' values:
  //
  // 8 - 4
  // |   |
  // 1 - 2
  //
  // O - O            O - O            O - X            X - X
  // |   |  -> 0      |   |  -> 1      |   |  -> 5      |   |  -> 15
  // O - O            X - O            X - O            X - X

  return static_cast<uint8_t>(grid.getValue(widthIndex,     heightIndex    ))        // Bottom-left
       | static_cast<uint8_t>(grid.getValue(widthIndex + 1, heightIndex    ) << 1u)  // Bottom-right
       | static_cast<uint8_t>(grid.getValue(widthIndex + 1, heightIndex + 1) << 2u)  // Top-right
       | static_cast<uint8_t>(grid.getValue(widthIndex,     heightIndex + 1) << 3u); // Top-left
}

} // namespace

Mesh MarchingSquares::compute(const Grid2b& grid) {
  ZoneScopedN("MarchingSquares::compute");

  if (grid.getWidth() < 2 || grid.getHeight() < 2)
    throw std::invalid_argument("[MarchingSquares] The input grid's width & height must be at least 2.");

  Mesh mesh;
  Submesh& submesh = mesh.addSubmesh();
  std::vector<Vertex>& vertices = submesh.getVertices();
  std::vector<unsigned int>& indices = submesh.getTriangleIndices();

  const Vec2f globalOffset(static_cast<float>(grid.getWidth() - 1) * 0.5f, static_cast<float>(grid.getHeight() - 1) * 0.5f);

  for (std::size_t heightIndex = 0; heightIndex < grid.getHeight() - 1; ++heightIndex) {
    for (std::size_t widthIndex = 0; widthIndex < grid.getWidth() - 1; ++widthIndex) {
      const uint8_t cellConfig = computeCellConfiguration(grid, widthIndex, heightIndex);
      const std::array<int8_t, 4>& edgeIndices = linesIndices[cellConfig];
      const Vec2f localOffset(static_cast<float>(widthIndex), static_cast<float>(heightIndex));

      for (std::size_t edgeIndex = 0; edgeIndex < edgeIndices.size(); edgeIndex += 2) {
        if (edgeIndices[edgeIndex] == NONE)
          break;

        const Vec2f firstPoint  = edgeVertices[edgeIndices[edgeIndex    ]] - globalOffset + localOffset;
        const Vec2f secondPoint = edgeVertices[edgeIndices[edgeIndex + 1]] - globalOffset + localOffset;
        vertices.emplace_back(Vertex{ Vec3f(firstPoint, 0.f) });
        vertices.emplace_back(Vertex{ Vec3f(firstPoint, 0.f) });
        vertices.emplace_back(Vertex{ Vec3f(secondPoint, 0.f) });

        indices.emplace_back(static_cast<unsigned int>(vertices.size() - 3));
        indices.emplace_back(static_cast<unsigned int>(vertices.size() - 2));
        indices.emplace_back(static_cast<unsigned int>(vertices.size() - 1));
      }
    }
  }

  return mesh;
}

} // namespace Raz
