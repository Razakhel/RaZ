#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Render/Mesh.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

} // namespace

void Mesh::importOff(std::ifstream& file) {
  unsigned int vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  m_vbo.getVertices().resize(vertexCount * 3);
  getEbo().getIndices().resize(faceCount * 3);

  for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> m_vbo.getVertices()[vertexIndex]
         >> m_vbo.getVertices()[vertexIndex + 1]
         >> m_vbo.getVertices()[vertexIndex + 2];

  for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
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

  std::vector<std::size_t> texcoordsIndices;
  std::vector<std::size_t> normalsIndices;

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

      const unsigned int nbVertices = std::count(indices.cbegin(), indices.cend(), ' ');
      const unsigned int nbIndices = std::count(indices.cbegin(), indices.cend(), '/') / nbVertices + 1;

      std::stringstream indicesStream(indices);
      std::array<std::string, 3> partIndices;
      std::string vertex;

      for (std::size_t vertIndex = 0; vertIndex < nbVertices; ++vertIndex) {
        indicesStream >> vertex;

        for (std::size_t partIndex = 0; partIndex < 3; ++partIndex) {
          const char delim = '/';
          const std::size_t delimPos = vertex.find(delim);

          partIndices[partIndex] = vertex.substr(0, delimPos);
          vertex.erase(0, delimPos + 1);
        }

        getEbo().getIndices().push_back(std::stoul(partIndices[0]));

        if (nbIndices > 1) {
          if (!partIndices[1].empty())
            texcoordsIndices.push_back(std::stoul(partIndices[1]));

          if (nbIndices > 2)
            normalsIndices.push_back(std::stoul(partIndices[2]));
        }
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

  for (std::size_t vertIndex = 0, partIndex = 0; partIndex < getEbo().getIndices().size(); vertIndex += 8, ++partIndex) {
    const std::size_t finalVertIndex = (getEbo().getIndices()[partIndex] - 1) * 3;
    const std::size_t finalTexcoordsIndex = (texcoordsIndices[partIndex] - 1) * 2;
    const std::size_t finalNormalsIndex = (normalsIndices[partIndex] - 1) * 3;
    std::array<float, 8> values;

    values[0] = vertices[finalVertIndex];
    values[1] = vertices[finalVertIndex + 1];
    values[2] = vertices[finalVertIndex + 2];

    values[3] = texcoords[finalTexcoordsIndex];
    values[4] = texcoords[finalTexcoordsIndex + 1];

    values[5] = normals[finalNormalsIndex];
    values[6] = normals[finalNormalsIndex + 1];
    values[7] = normals[finalNormalsIndex + 2];

    if (std::search(m_vbo.getVertices().cbegin(), m_vbo.getVertices().cend(), values.cbegin(), values.cend()) == m_vbo.getVertices().cend())
      m_vbo.getVertices().insert(m_vbo.getVertices().cend(), values.begin(), values.end());
  }
}

void Mesh::import(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF")
      importOff(file);
    else if (format == "obj" || format == "OBJ")
      importObj(file);
    else
      std::cerr << "Error: '" << format << "' format is not supported" << std::endl;

    load();
  } else {
    std::cerr << "Error: Couldn't open the file '" << fileName << "'" << std::endl;
  }
}

void Mesh::load() {
  m_vao.bind();

  getEbo().bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(getEbo().getIndices().front()) * getEbo().getIndices().size(),
               getEbo().getIndices().data(),
               GL_STATIC_DRAW);
  getEbo().unbind();

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getVertices().front()) * m_vbo.getVertices().size(),
               m_vbo.getVertices().data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        8 * sizeof(m_vbo.getVertices().front()),
                        nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2,
                        GL_FLOAT, GL_FALSE,
                        8 * sizeof(m_vbo.getVertices().front()),
                        reinterpret_cast<void*>(3 * sizeof(m_vbo.getVertices().front())));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3,
                        GL_FLOAT, GL_FALSE,
                        8 * sizeof(m_vbo.getVertices().front()),
                        reinterpret_cast<void*>(5 * sizeof(m_vbo.getVertices().front())));
  glEnableVertexAttribArray(2);

  m_vbo.unbind();
  m_vao.unbind();
}

void Mesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
