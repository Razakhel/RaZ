#include <fstream>

#include "RaZ/Mesh/Mesh.hpp"

namespace Raz {

void Mesh::load(const std::string& fileName) {
  std::ifstream fs{ fileName };

  if (fs) {
    unsigned int vertexCount, faceCount;

    fs.ignore(3);
    fs >> vertexCount >> faceCount;
    fs.ignore(100, '\n');

    vbo.getVertices().resize(vertexCount * 3);
    ebo.getIndices().resize(faceCount * 3);

    for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
      fs >> vbo.getVertices()[vertexIndex] >> vbo.getVertices()[vertexIndex + 1] >> vbo.getVertices()[vertexIndex + 2];

    for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
      fs.ignore(2);
      fs >> ebo.getIndices()[faceIndex] >> ebo.getIndices()[faceIndex + 1] >> ebo.getIndices()[faceIndex + 2];
    }
  }
}

} // namespace Raz
