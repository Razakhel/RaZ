#include <vector>
#include <fstream>
#include <iostream>

#include "RaZ/Mesh/Mesh.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

void loadOff(std::ifstream& file, VertexBuffer& vbo, ElementBuffer& ebo) {
  unsigned int vertexCount, faceCount;

  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  vbo.getVertices().resize(vertexCount * 3);
  ebo.getIndices().resize(faceCount * 3);

  for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> vbo.getVertices()[vertexIndex] >> vbo.getVertices()[vertexIndex + 1] >> vbo.getVertices()[vertexIndex + 2];

  for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> ebo.getIndices()[faceIndex] >> ebo.getIndices()[faceIndex + 1] >> ebo.getIndices()[faceIndex + 2];
  }
}

void loadObj(const std::ifstream& file) {
  std::cerr << "Error: OBJ reading not yet implemented" << std::endl;
  exit(EXIT_FAILURE);
}

} // namespace

void ElementBuffer::setIndices(const std::vector<unsigned int>& indices) {
  m_indices.resize(indices.size());
  std::move(indices.begin(), indices.end(), m_indices.begin());
}

void Texture::load(const std::string& fileName) {

}

void Mesh::load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
  m_ebo.setIndices(indices);

  m_vao.bind();

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices.front()) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  m_ebo.bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices.front()) * indices.size(), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  /*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  texture.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), , 0, GL_RGB, GL_UNSIGNED_BYTE, img.getData().data());*/
}

void Mesh::load(const std::string& fileName) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF")
      loadOff(file, m_vbo, m_ebo);
    else if (format == "obj" || format == "OBJ")
      loadObj(file);
    else
      std::cerr << "Error: '" << format << "' format is not supported" << std::endl;
  } else {
    std::cerr << "Error: Couldn't open the file '" << fileName << "'" << std::endl;
  }
}

void Mesh::draw() const {
  //m_texture.bind();
  m_vao.bind();

  glDrawElements(GL_TRIANGLES, getFaceCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
