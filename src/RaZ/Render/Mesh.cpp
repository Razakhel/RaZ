#include <fstream>
#include <sstream>
#include <iostream>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Render/Mesh.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

} // namespace

VertexBuffer::VertexBuffer() {
  glGenBuffers(1, &m_verticesIndex);
  glGenBuffers(1, &m_texcoordsIndex);
  glGenBuffers(1, &m_normalsIndex);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &m_verticesIndex);
  glDeleteBuffers(1, &m_texcoordsIndex);
  glDeleteBuffers(1, &m_normalsIndex);
}

void Mesh::importOff(std::ifstream& file) {
  unsigned int vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  m_vbo.getVertices().resize(vertexCount * 3);
  getEbo().getVerticesIndices().resize(faceCount * 3);

  for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> m_vbo.getVertices()[vertexIndex]
         >> m_vbo.getVertices()[vertexIndex + 1]
         >> m_vbo.getVertices()[vertexIndex + 2];

  for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> getEbo().getVerticesIndices()[faceIndex]
         >> getEbo().getVerticesIndices()[faceIndex + 1]
         >> getEbo().getVerticesIndices()[faceIndex + 2];
  }
}

void Mesh::importObj(std::ifstream& file) {
  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        m_vbo.getNormals().resize(m_vbo.getNormals().size() + 3);

        file >> m_vbo.getNormals()[m_vbo.getNormals().size() - 3]
             >> m_vbo.getNormals()[m_vbo.getNormals().size() - 2]
             >> m_vbo.getNormals()[m_vbo.getNormals().size() - 1];
      } else if (type[1] == 't') { // Texcoords
        m_vbo.getTexcoords().resize(m_vbo.getTexcoords().size() + 2);

        file >> m_vbo.getTexcoords()[m_vbo.getTexcoords().size() - 2]
             >> m_vbo.getTexcoords()[m_vbo.getTexcoords().size() - 1];
      } else { // Vertices
        m_vbo.getVertices().resize(m_vbo.getVertices().size() + 3);

        file >> m_vbo.getVertices()[m_vbo.getVertices().size() - 3]
             >> m_vbo.getVertices()[m_vbo.getVertices().size() - 2]
             >> m_vbo.getVertices()[m_vbo.getVertices().size() - 1];
      }
    } else if (type[0] == 'f') { // Faces
      getEbo().getVerticesIndices().resize(getEbo().getVerticesIndices().size() + 3);
      getEbo().getNormalsIndices().resize(getEbo().getNormalsIndices().size() + 3);
      getEbo().getTexcoordsIndices().resize(getEbo().getTexcoordsIndices().size() + 3);

      for (uint8_t facePartIndex = 3; facePartIndex > 0; --facePartIndex) {
        std::array<std::string, 3> vertIndices;
        const char delim = '/';
        std::string indices;

        file >> indices;
        for (uint8_t i = 0; i < 3; ++i) {
          const unsigned long delimPos = indices.find(delim);

          vertIndices[i] = indices.substr(0, delimPos);
          indices.erase(0, delimPos + 1);
        }

        *(getEbo().getVerticesIndices().end() - facePartIndex) = std::stoul(vertIndices[0]);
        *(getEbo().getTexcoordsIndices().end() - facePartIndex) = std::stoul(vertIndices[1]);
        *(getEbo().getNormalsIndices().end() - facePartIndex) = std::stoul(vertIndices[2]);
      }
    } else if (type[0] == 'm') { // Import MTL
      //file >> type;
    } else if (type[0] == 'u') { // Use MTL
      //file >> type;
    } else {
      std::getline(file, type); // Skip the rest of the line
    }
  }
}

void Mesh::import(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF") {
      importOff(file);
    } else if (format == "obj" || format == "OBJ") {
      importObj(file);
    } else {
      std::cerr << "Error: '" << format << "' format is not supported" << std::endl;
    }

    load();
  } else {
    std::cerr << "Error: Couldn't open the file '" << fileName << "'" << std::endl;
  }
}

void Mesh::load() {
  m_vao.bind();

  getEbo().bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(getEbo().getVerticesIndices().front()) * getEbo().getVerticesIndices().size(),
               getEbo().getVerticesIndices().data(),
               GL_STATIC_DRAW);
  getEbo().unbind();

  m_vbo.bindVertices();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getVertices().front()) * m_vbo.getVertices().size(),
               m_vbo.getVertices().data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  m_vbo.bindTexcoords();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getTexcoords().front()) * m_vbo.getTexcoords().size(),
               m_vbo.getTexcoords().data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(1);

  m_vbo.bindNormals();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getNormals().front()) * m_vbo.getNormals().size(),
               m_vbo.getNormals().data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(2);

  m_vbo.unbind();
  m_vao.unbind();
}

void Mesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, getFaceCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
