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

void importOff(std::ifstream& file, Mesh& mesh) {
  unsigned int vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  mesh.getVbo().getVertices().resize(vertexCount * 3);
  mesh.getEbo().getVerticesIndices().resize(faceCount * 3);

  for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> mesh.getVbo().getVertices()[vertexIndex]
         >> mesh.getVbo().getVertices()[vertexIndex + 1]
         >> mesh.getVbo().getVertices()[vertexIndex + 2];

  for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> mesh.getEbo().getVerticesIndices()[faceIndex]
         >> mesh.getEbo().getVerticesIndices()[faceIndex + 1]
         >> mesh.getEbo().getVerticesIndices()[faceIndex + 2];
  }
}

void importObj(std::ifstream& file, Mesh& mesh) {
  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        mesh.getVbo().getNormals().resize(mesh.getVbo().getNormals().size() + 3);

        file >> mesh.getVbo().getNormals()[mesh.getVbo().getNormals().size() - 3]
             >> mesh.getVbo().getNormals()[mesh.getVbo().getNormals().size() - 2]
             >> mesh.getVbo().getNormals()[mesh.getVbo().getNormals().size() - 1];
      } else if (type[1] == 't') { // Texcoords
        mesh.getVbo().getTexcoords().resize(mesh.getVbo().getTexcoords().size() + 2);

        file >> mesh.getVbo().getTexcoords()[mesh.getVbo().getTexcoords().size() - 2]
             >> mesh.getVbo().getTexcoords()[mesh.getVbo().getTexcoords().size() - 1];
      } else { // Vertices
        mesh.getVbo().getVertices().resize(mesh.getVbo().getVertices().size() + 3);

        file >> mesh.getVbo().getVertices()[mesh.getVbo().getVertices().size() - 3]
             >> mesh.getVbo().getVertices()[mesh.getVbo().getVertices().size() - 2]
             >> mesh.getVbo().getVertices()[mesh.getVbo().getVertices().size() - 1];
      }
    } else if (type[0] == 'f') { // Faces
      mesh.getEbo().getVerticesIndices().resize(mesh.getEbo().getVerticesIndices().size() + 3);
      mesh.getEbo().getNormalsIndices().resize(mesh.getEbo().getNormalsIndices().size() + 3);
      mesh.getEbo().getTexcoordsIndices().resize(mesh.getEbo().getTexcoordsIndices().size() + 3);

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

        *(mesh.getEbo().getVerticesIndices().end() - facePartIndex) = std::stoul(vertIndices[0]);
        *(mesh.getEbo().getTexcoordsIndices().end() - facePartIndex) = std::stoul(vertIndices[1]);
        *(mesh.getEbo().getNormalsIndices().end() - facePartIndex) = std::stoul(vertIndices[2]);
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

} // namespace

void Mesh::import(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF") {
      importOff(file, *this);
    } else if (format == "obj" || format == "OBJ") {
      importObj(file, *this);
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

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getVertices().front()) * m_vbo.getVertices().size(),
               m_vbo.getVertices().data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  m_vao.unbind();
  getEbo().unbind();
  m_vbo.unbind();

  /*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  texture.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), , 0, GL_RGB, GL_UNSIGNED_BYTE, img.getData().data());*/
}

void Mesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, getFaceCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
