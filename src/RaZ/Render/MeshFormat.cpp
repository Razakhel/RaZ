#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "RaZ/Render/Mesh.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

} // namespace

void Mesh::importOff(std::ifstream& file) {
  std::size_t vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  m_vbo.getVertices().resize(vertexCount * 3);
  getEbo().getIndices().resize(faceCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    file >> m_vbo.getVertices()[vertexIndex].positions[0]
         >> m_vbo.getVertices()[vertexIndex].positions[1]
         >> m_vbo.getVertices()[vertexIndex].positions[2];

  for (std::size_t faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> getEbo().getIndices()[faceIndex]
         >> getEbo().getIndices()[faceIndex + 1]
         >> getEbo().getIndices()[faceIndex + 2];
  }
}

void Mesh::importObj(std::ifstream& file) {
  std::vector<Vec3f> positions;
  std::vector<Vec2f> texcoords;
  std::vector<Vec3f> normals;

  std::vector<int64_t> posIndices;
  std::vector<int64_t> texcoordsIndices;
  std::vector<int64_t> normalsIndices;

  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        Vec3f normalsTriplet {};

        file >> normalsTriplet[0]
             >> normalsTriplet[1]
             >> normalsTriplet[2];

        normals.push_back(normalsTriplet);
      } else if (type[1] == 't') { // Texcoords
        Vec2f texcoordsTriplet {};

        file >> texcoordsTriplet[0]
             >> texcoordsTriplet[1];

        texcoords.push_back(texcoordsTriplet);
      } else { // Vertices
        Vec3f positionTriplet {};

        file >> positionTriplet[0]
             >> positionTriplet[1]
             >> positionTriplet[2];

        positions.push_back(positionTriplet);
      }
    } else if (type[0] == 'f') { // Faces
      std::string indices;
      std::getline(file, indices);

      const char delim = '/';
      const uint8_t nbVertices = std::count(indices.cbegin(), indices.cend(), ' ');
      const uint8_t nbParts = std::count(indices.cbegin(), indices.cend(), delim) / nbVertices + 1;
      const bool quadFaces = (nbVertices == 4);

      std::stringstream indicesStream(indices);
      std::vector<int64_t> partIndices(nbParts * nbVertices);
      std::string vertex;

      for (std::size_t vertIndex = 0; vertIndex < nbVertices; ++vertIndex) {
        indicesStream >> vertex;

        std::stringstream vertParts(vertex);
        std::string part;
        uint8_t partIndex = 0;

        while (std::getline(vertParts, part, delim)) {
          if (!part.empty())
            partIndices[partIndex * nbParts + vertIndex + (partIndex * quadFaces)] = std::stol(part);

          ++partIndex;
        }
      }

      if (quadFaces) {
        posIndices.emplace_back(partIndices[2]);
        posIndices.emplace_back(partIndices[0]);
        posIndices.emplace_back(partIndices[3]);

        texcoordsIndices.emplace_back(partIndices[6]);
        texcoordsIndices.emplace_back(partIndices[4]);
        texcoordsIndices.emplace_back(partIndices[7]);

        normalsIndices.emplace_back(partIndices[10]);
        normalsIndices.emplace_back(partIndices[8]);
        normalsIndices.emplace_back(partIndices[11]);
      }

      posIndices.emplace_back(partIndices[1]);
      posIndices.emplace_back(partIndices[0]);
      posIndices.emplace_back(partIndices[2]);

      texcoordsIndices.emplace_back(partIndices[4 + quadFaces]);
      texcoordsIndices.emplace_back(partIndices[3 + quadFaces]);
      texcoordsIndices.emplace_back(partIndices[5 + quadFaces]);

      const uint8_t quadStride = quadFaces * 2;

      normalsIndices.emplace_back(partIndices[7 + quadStride]);
      normalsIndices.emplace_back(partIndices[6 + quadStride]);
      normalsIndices.emplace_back(partIndices[8 + quadStride]);
    } else if (type[0] == 'm') {
      // Import MTL
    } else if (type[0] == 'u') {
      // Use MTL
    } else if (type[0] == 'o') {
      // Create/use object
    } else if (type[0] == 'g') {
      // Create/use group
    } else if (type[0] == 's') {
      // Enable/disable smooth shading
    } else {
      std::getline(file, type); // Skip the rest of the line
    }
  }

  std::map<std::array<std::size_t, 3>, unsigned int> indicesMap;

  for (std::size_t partIndex = 0; partIndex < posIndices.size(); ++partIndex) {
    const int64_t tempPosIndex = posIndices[partIndex];
    const std::size_t posIndex = (tempPosIndex < 0 ? tempPosIndex + positions.size() : tempPosIndex - 1ul);

    const int64_t tempTexIndex = texcoordsIndices[partIndex];
    const std::size_t texIndex = (tempTexIndex < 0 ? tempTexIndex + texcoords.size() : tempTexIndex - 1ul);

    const int64_t tempNormIndex = normalsIndices[partIndex];
    const std::size_t normIndex = (tempNormIndex < 0 ? tempNormIndex + normals.size() : tempNormIndex - 1ul);

    const std::array<std::size_t, 3> vertIndices = { posIndex, texIndex, normIndex };

    const auto indexIter = indicesMap.find(vertIndices);
    if (indexIter != indicesMap.cend()) {
      getEbo().getIndices().emplace_back(indexIter->second);
    } else {
      Vertex vert {};

      vert.positions[0] = positions[posIndex][0];
      vert.positions[1] = positions[posIndex][1];
      vert.positions[2] = positions[posIndex][2];

      if (!texcoords.empty()) {
        vert.texcoords[0] = texcoords[texIndex][0];
        vert.texcoords[1] = texcoords[texIndex][1];
      }

      if (!normals.empty()) {
        vert.normals[0] = normals[normIndex][0];
        vert.normals[1] = normals[normIndex][1];
        vert.normals[2] = normals[normIndex][2];
      }

      getEbo().getIndices().emplace_back(indicesMap.size());
      indicesMap.emplace(vertIndices, indicesMap.size());
      m_vbo.getVertices().push_back(vert);
    }
  }
}

void Mesh::importFromFile(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF")
      importOff(file);
    else if (format == "obj" || format == "OBJ")
      importObj(file);
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");

    load();
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }
}

} // namespace Raz
