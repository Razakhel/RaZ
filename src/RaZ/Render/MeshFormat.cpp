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
  std::vector<float> vertices;
  std::vector<float> texcoords;
  std::vector<float> normals;

  std::vector<int64_t> posIndices;
  std::vector<int64_t> texcoordsIndices;
  std::vector<int64_t> normalsIndices;

  std::map<Vertex, unsigned int, Vertex> indicesMap;

  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        normals.resize(normals.size() + 3);

        file >> normals[normals.size() - 3]
             >> normals[normals.size() - 2]
             >> normals[normals.size() - 1];
      } else if (type[1] == 't') { // Texcoords
        texcoords.resize(texcoords.size() + 2);

        file >> texcoords[texcoords.size() - 2]
             >> texcoords[texcoords.size() - 1];
      } else { // Vertices
        vertices.resize(vertices.size() + 3);

        file >> vertices[vertices.size() - 3]
             >> vertices[vertices.size() - 2]
             >> vertices[vertices.size() - 1];
      }
    } else if (type[0] == 'f') { // Faces
      std::string indices;
      std::getline(file, indices);

      const uint8_t nbVertices = std::count(indices.cbegin(), indices.cend(), ' ');
      const uint8_t nbParts = std::count(indices.cbegin(), indices.cend(), '/') / nbVertices + 1;
      const bool quadFaces = (nbVertices == 4);

      std::stringstream indicesStream(indices);
      std::vector<int64_t> partIndices(nbParts * nbVertices);
      std::string vertex;

      for (std::size_t vertIndex = 0; vertIndex < nbVertices; ++vertIndex) {
        indicesStream >> vertex;

        for (std::size_t partIndex = 0; partIndex < nbParts; ++partIndex) {
          const char delim = '/';
          const std::size_t delimPos = vertex.find(delim);
          const std::string part = vertex.substr(0, delimPos);

          if (!part.empty())
            partIndices[partIndex * nbParts + vertIndex + (partIndex * quadFaces)] = std::stol(part);

          vertex.erase(0, delimPos + 1);
        }
      }

      if (quadFaces) {
        posIndices.push_back(partIndices[2]);
        posIndices.push_back(partIndices[0]);
        posIndices.push_back(partIndices[3]);

        texcoordsIndices.push_back(partIndices[6]);
        texcoordsIndices.push_back(partIndices[4]);
        texcoordsIndices.push_back(partIndices[7]);

        normalsIndices.push_back(partIndices[10]);
        normalsIndices.push_back(partIndices[8]);
        normalsIndices.push_back(partIndices[11]);
      }

      const uint8_t quadStride = quadFaces * 2;

      posIndices.push_back(partIndices[1]);
      posIndices.push_back(partIndices[0]);
      posIndices.push_back(partIndices[2]);

      texcoordsIndices.push_back(partIndices[4 + quadFaces]);
      texcoordsIndices.push_back(partIndices[3 + quadFaces]);
      texcoordsIndices.push_back(partIndices[5 + quadFaces]);

      normalsIndices.push_back(partIndices[7 + quadStride]);
      normalsIndices.push_back(partIndices[6 + quadStride]);
      normalsIndices.push_back(partIndices[8 + quadStride]);
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

  for (std::size_t vertIndex = 0, partIndex = 0; partIndex < posIndices.size(); vertIndex += 8, ++partIndex) {
    const int64_t tempPosIndex = posIndices[partIndex] - 1;
    const std::size_t finalPosIndex = (tempPosIndex < 0 ? tempPosIndex + (vertices.size() / 3) + 1 : tempPosIndex) * 3;
    Vertex vert {};

    vert.positions[0] = vertices[finalPosIndex];
    vert.positions[1] = vertices[finalPosIndex + 1];
    vert.positions[2] = vertices[finalPosIndex + 2];

    if (!texcoords.empty()) {
      const int64_t tempTexIndex = texcoordsIndices[partIndex] - 1;
      const std::size_t finalTexIndex = (tempTexIndex < 0 ? tempTexIndex + (texcoords.size() / 2) + 1 : tempTexIndex) * 2;

      vert.texcoords[0] = texcoords[finalTexIndex];
      vert.texcoords[1] = texcoords[finalTexIndex + 1];
    }

    if (!normals.empty()) {
      const int64_t tempNormIndex = normalsIndices[partIndex] - 1;
      const std::size_t finalNormIndex = (tempNormIndex < 0 ? tempNormIndex + (normals.size() / 3) + 1 : tempNormIndex) * 3;

      vert.normals[0] = normals[finalNormIndex];
      vert.normals[1] = normals[finalNormIndex + 1];
      vert.normals[2] = normals[finalNormIndex + 2];
    }

    const auto indexIter = indicesMap.find(vert);
    if (indexIter != indicesMap.cend()) {
      getEbo().getIndices().push_back(indexIter->second - 1);
    } else {
      indicesMap.emplace(vert, indicesMap.size() + 1);
      m_vbo.getVertices().push_back(vert);
      getEbo().getIndices().push_back(indicesMap.size() - 1);
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
