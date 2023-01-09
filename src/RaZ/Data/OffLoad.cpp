#include "RaZ/Data/OffFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fstream>

namespace Raz::OffFormat {

Mesh load(const FilePath& filePath) {
  Logger::debug("[OffLoad] Loading OFF file ('" + filePath + "')...");

  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Could not open the OFF file '" + filePath + '\'');

  Mesh mesh;
  Submesh& submesh = mesh.addSubmesh();

  std::size_t vertexCount {};
  std::size_t faceCount {};

  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(vertexCount * 3);

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.reserve(faceCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex) {
    file >> vertices[vertexIndex].position.x()
         >> vertices[vertexIndex].position.y()
         >> vertices[vertexIndex].position.z();
  }

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

  mesh.computeTangents();

  Logger::debug("[OffLoad] Loaded OFF file (" + std::to_string(mesh.recoverVertexCount()) + " vertices, "
                                              + std::to_string(mesh.recoverTriangleCount()) + " triangles)");

  return mesh;
}

} // namespace Raz::OffFormat
