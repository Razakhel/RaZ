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

  std::vector<std::size_t> posIndices;
  std::vector<std::size_t> texcoordsIndices;
  std::vector<std::size_t> normalsIndices;

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
      std::vector<std::size_t> partIndices(nbParts * nbVertices);
      std::string vertex;

      for (std::size_t vertIndex = 0; vertIndex < nbVertices; ++vertIndex) {
        indicesStream >> vertex;

        for (std::size_t partIndex = 0; partIndex < nbParts; ++partIndex) {
          const char delim = '/';
          const std::size_t delimPos = vertex.find(delim);

          partIndices[partIndex * nbParts + vertIndex + (partIndex * quadFaces)] = std::stoul(vertex.substr(0, delimPos));
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
    const std::size_t finalVertIndex = (posIndices[partIndex] - 1) * 3;
    std::vector<float> values(nbFeatures);

    values[0] = vertices[finalVertIndex];
    values[1] = vertices[finalVertIndex + 1];
    values[2] = vertices[finalVertIndex + 2];

    if (m_hasTexcoords) {
      const std::size_t finalTexcoordsIndex = (texcoordsIndices[partIndex] - 1) * 2;

      values[3] = texcoords[finalTexcoordsIndex];
      values[4] = texcoords[finalTexcoordsIndex + 1];
    }

    if (m_hasNormals) {
      const std::size_t finalNormalsIndex = (normalsIndices[partIndex] - 1) * 3;
      const uint8_t texcoordsStride = m_hasTexcoords * 2;

      values[3 + texcoordsStride] = normals[finalNormalsIndex];
      values[4 + texcoordsStride] = normals[finalNormalsIndex + 1];
      values[5 + texcoordsStride] = normals[finalNormalsIndex + 2];
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
