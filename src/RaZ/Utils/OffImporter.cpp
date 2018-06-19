#include <fstream>

#include "RaZ/Render/Model.hpp"

namespace Raz {

ModelPtr Model::importOff(std::ifstream& file) {
  MeshPtr mesh = std::make_unique<Mesh>();
  mesh->getSubmeshes().emplace_back(std::make_unique<Submesh>());

  const SubmeshPtr& submesh = mesh->getSubmeshes().front();

  std::size_t vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  submesh->getVertices().resize(vertexCount * 3);
  submesh->getIndices().reserve(faceCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    file >> submesh->getVertices()[vertexIndex].position[0]
         >> submesh->getVertices()[vertexIndex].position[1]
         >> submesh->getVertices()[vertexIndex].position[2];

  for (std::size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
    uint16_t partCount {};
    file >> partCount;

    std::vector<std::size_t> indices(partCount);
    file >> indices[0] >> indices[1] >> indices[2];

    submesh->getIndices().emplace_back(indices[0]);
    submesh->getIndices().emplace_back(indices[1]);
    submesh->getIndices().emplace_back(indices[2]);

    for (uint16_t partIndex = 3; partIndex < partCount; ++partIndex) {
      file >> indices[partIndex];

      submesh->getIndices().emplace_back(indices[0]);
      submesh->getIndices().emplace_back(indices[partIndex - 1]);
      submesh->getIndices().emplace_back(indices[partIndex]);
    }
  }

  submesh->getIndices().shrink_to_fit();

  return std::make_unique<Model>(std::move(mesh));
}

} // namespace Raz
