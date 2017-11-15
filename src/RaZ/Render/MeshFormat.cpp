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

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> m_vbo.getVertices()[vertexIndex]
         >> m_vbo.getVertices()[vertexIndex + 1]
         >> m_vbo.getVertices()[vertexIndex + 2];

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

  std::map<std::vector<float>, unsigned int> indicesMap;

  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        normals.resize(normals.size() + 3);

        file >> normals[normals.size() - 3]
             >> normals[normals.size() - 2]
             >> normals[normals.size() - 1];

        m_hasNormals = true;
      } else if (type[1] == 't') { // Texcoords
        texcoords.resize(texcoords.size() + 2);

        file >> texcoords[texcoords.size() - 2]
             >> texcoords[texcoords.size() - 1];

        m_hasTexcoords = true;
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
      const uint8_t nbParts = 1 + m_hasTexcoords + m_hasNormals;
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
          else
            --partIndex;
          vertex.erase(0, delimPos + 1);
        }
      }

      uint8_t texcoordsStride = nbVertices * m_hasTexcoords;

      if (quadFaces) {
        posIndices.push_back(partIndices[2]);
        posIndices.push_back(partIndices[0]);
        posIndices.push_back(partIndices[3]);

        if (m_hasTexcoords) {
          texcoordsIndices.push_back(partIndices[6]);
          texcoordsIndices.push_back(partIndices[4]);
          texcoordsIndices.push_back(partIndices[7]);
        }

        if (m_hasNormals) {
          normalsIndices.push_back(partIndices[6 + texcoordsStride]);
          normalsIndices.push_back(partIndices[4 + texcoordsStride]);
          normalsIndices.push_back(partIndices[7 + texcoordsStride]);
        }
      }

      posIndices.push_back(partIndices[1]);
      posIndices.push_back(partIndices[0]);
      posIndices.push_back(partIndices[2]);

      if (m_hasTexcoords) {
        texcoordsIndices.push_back(partIndices[4 + quadFaces]);
        texcoordsIndices.push_back(partIndices[3 + quadFaces]);
        texcoordsIndices.push_back(partIndices[5 + quadFaces]);
      }

      if (m_hasNormals) {
        texcoordsStride += quadFaces;

        normalsIndices.push_back(partIndices[4 + texcoordsStride]);
        normalsIndices.push_back(partIndices[3 + texcoordsStride]);
        normalsIndices.push_back(partIndices[5 + texcoordsStride]);
      }
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

  const uint8_t nbFeatures = 3 + m_hasTexcoords * 2 + m_hasNormals * 3;

  for (std::size_t vertIndex = 0, partIndex = 0; partIndex < posIndices.size(); vertIndex += nbFeatures, ++partIndex) {
    const int64_t tempPosIndex = posIndices[partIndex] - 1;
    const std::size_t finalPosIndex = (tempPosIndex < 0 ? tempPosIndex + (vertices.size() / 3) + 1 : tempPosIndex) * 3;
    std::vector<float> values(nbFeatures);

    values[0] = vertices[finalPosIndex];
    values[1] = vertices[finalPosIndex + 1];
    values[2] = vertices[finalPosIndex + 2];

    if (m_hasTexcoords) {
      const int64_t tempTexIndex = texcoordsIndices[partIndex] - 1;
      const std::size_t finalTexIndex = (tempTexIndex < 0 ? tempTexIndex + (texcoords.size() / 2) + 1 : tempTexIndex) * 2;

      values[3] = texcoords[finalTexIndex];
      values[4] = texcoords[finalTexIndex + 1];
    }

    if (m_hasNormals) {
      const int64_t tempNormIndex = normalsIndices[partIndex] - 1;
      const std::size_t finalNormIndex = (tempNormIndex < 0 ? tempNormIndex + (normals.size() / 3) + 1 : tempNormIndex) * 3;
      const uint8_t texcoordsStride = m_hasTexcoords * 2;

      values[3 + texcoordsStride] = normals[finalNormIndex];
      values[4 + texcoordsStride] = normals[finalNormIndex + 1];
      values[5 + texcoordsStride] = normals[finalNormIndex + 2];
    }

    const auto indexIter = indicesMap.find(values);
    if (indexIter != indicesMap.cend()) {
      getEbo().getIndices().push_back(indexIter->second - 1);
    } else {
      indicesMap.emplace(values, indicesMap.size() + 1);
      m_vbo.getVertices().insert(m_vbo.getVertices().cend(), values.begin(), values.end());
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
