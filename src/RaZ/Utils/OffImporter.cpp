#include "RaZ/Data/Mesh.hpp"

#include <fstream>

namespace Raz {

void Mesh::importOff(std::ifstream& file) {
  Submesh& submesh = m_submeshes.front();

  std::size_t vertexCount {};
  std::size_t faceCount {};

  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(vertexCount * 3);

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.reserve(faceCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    file >> vertices[vertexIndex].position[0]
         >> vertices[vertexIndex].position[1]
         >> vertices[vertexIndex].position[2];

  for (std::size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
    uint16_t partCount {};
    file >> partCount;

    std::vector<std::size_t> partsIndices(partCount);
    file >> partsIndices[0] >> partsIndices[1] >> partsIndices[2];

    indices.emplace_back(static_cast<unsigned int>(partsIndices[0]));
    indices.emplace_back(static_cast<unsigned int>(partsIndices[1]));
    indices.emplace_back(static_cast<unsigned int>(partsIndices[2]));

    for (uint16_t partIndex = 3; partIndex < partCount; ++partIndex) {
      file >> partsIndices[partIndex];

      indices.emplace_back(static_cast<unsigned int>(partsIndices[0]));
      indices.emplace_back(static_cast<unsigned int>(partsIndices[partIndex - 1]));
      indices.emplace_back(static_cast<unsigned int>(partsIndices[partIndex]));
    }
  }

  indices.shrink_to_fit();
}

} // namespace Raz
