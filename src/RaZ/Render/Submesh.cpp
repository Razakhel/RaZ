#include "RaZ/Render/Submesh.hpp"

namespace Raz {

void Submesh::load() const {
  m_vao.bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(getIndices().front()) * getIndices().size(),
               getIndices().data(),
               GL_STATIC_DRAW);

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(getVertices().front()) * getVertices().size(),
               getVertices().data(),
               GL_STATIC_DRAW);

  const uint8_t stride = sizeof(getVertices().front());

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(3 * sizeof(getVertices().front().position.getData().front())));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(5 * sizeof(getVertices().front().normal.getData().front())));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(8 * sizeof(getVertices().front().tangent.getData().front())));
  glEnableVertexAttribArray(3);

  m_vbo.unbind();
  m_vao.unbind();
}

void Submesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
