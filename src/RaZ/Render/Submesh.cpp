#include "RaZ/Render/Submesh.hpp"

namespace Raz {

void Submesh::load() const {
  m_vao.bind();

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<int64_t>(sizeof(getIndices().front()) * getIndices().size()),
               getIndices().data(),
               GL_STATIC_DRAW);

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<int64_t>(sizeof(getVertices().front()) * getVertices().size()),
               getVertices().data(),
               GL_STATIC_DRAW);

  constexpr uint8_t stride = sizeof(getVertices().front());

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        nullptr);
  glEnableVertexAttribArray(0);

  constexpr std::size_t positionSize = sizeof(getVertices().front().position);
  glVertexAttribPointer(1, 2,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize));
  glEnableVertexAttribArray(1);

  constexpr std::size_t texcoordsSize = sizeof(getVertices().front().texcoords);
  glVertexAttribPointer(2, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize + texcoordsSize));
  glEnableVertexAttribArray(2);

  constexpr std::size_t normalSize = sizeof(getVertices().front().normal);
  glVertexAttribPointer(3, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize + texcoordsSize + normalSize));
  glEnableVertexAttribArray(3);

  m_vbo.unbind();
  m_vao.unbind();
}

void Submesh::draw() const {
  m_vao.bind();
  glDrawElements(GL_TRIANGLES, static_cast<int>(getIndexCount()), GL_UNSIGNED_INT, nullptr);
}

} // namespace Raz
