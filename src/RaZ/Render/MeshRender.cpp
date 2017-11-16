#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::load() {
  m_vao.bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(getEbo().getIndices().front()) * getEbo().getIndices().size(),
               getEbo().getIndices().data(),
               GL_STATIC_DRAW);

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getVertices().front()) * m_vbo.getVertices().size(),
               m_vbo.getVertices().data(),
               GL_STATIC_DRAW);

  const uint8_t stride = sizeof(m_vbo.getVertices().front());

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(3 * sizeof(m_vbo.getVertices().front().positions.getData().front())));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(5 * sizeof(m_vbo.getVertices().front().normals.getData().front())));
  glEnableVertexAttribArray(2);

  m_vbo.unbind();
  m_vao.unbind();
}

void Mesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
