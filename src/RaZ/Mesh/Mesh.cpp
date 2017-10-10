#include <vector>
#include <fstream>

#include "RaZ/Mesh/Mesh.hpp"

namespace Raz {

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
  std::ifstream fs{ fileName };

  if (fs) {
    unsigned int vertexCount, faceCount;

    fs.ignore(3);
    fs >> vertexCount >> faceCount;
    fs.ignore(100, '\n');

    m_vbo.getVertices().resize(vertexCount * 3);
    m_ebo.getIndices().resize(faceCount * 3);

    for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
      fs >> m_vbo.getVertices()[vertexIndex] >> m_vbo.getVertices()[vertexIndex + 1] >> m_vbo.getVertices()[vertexIndex + 2];

    for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
      fs.ignore(2);
      fs >> m_ebo.getIndices()[faceIndex] >> m_ebo.getIndices()[faceIndex + 1] >> m_ebo.getIndices()[faceIndex + 2];
    }
  }
}

void Mesh::draw() const {
  m_texture.bind();
  m_vao.bind();

  glDrawElements(GL_TRIANGLES, getFaceCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
