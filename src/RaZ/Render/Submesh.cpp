#include "GL/glew.h"
#include "RaZ/Render/Submesh.hpp"

namespace Raz {

void Submesh::setRenderMode(RenderMode renderMode) {
  m_renderMode = renderMode;

  switch (m_renderMode) {
    case RenderMode::POINT:
      m_renderFunc = [] (const Submesh& submesh) { glDrawArrays(GL_POINTS, 0, submesh.getVertexCount()); };
      break;

//    case RenderMode::LINE: {
//      if (m_ibo.getLineIndices().empty())
//        computeLineIndices();
//
//      m_renderFunc = [] (const Submesh& submesh) {
//        glDrawElements(GL_LINES, static_cast<int>(submesh.getLineIndexCount()), GL_UNSIGNED_INT, nullptr);
//      };
//
//      break;
//    }

    case RenderMode::TRIANGLE: {
      m_renderFunc = [] (const Submesh& submesh) {
        glDrawElements(GL_TRIANGLES, static_cast<int>(submesh.getTriangleIndexCount()), GL_UNSIGNED_INT, nullptr);
      };

      break;
    }
  }

  loadIndices();
}

void Submesh::load() const {
  loadVertices();
  loadIndices();
}

void Submesh::draw() const {
  m_vao.bind();
  m_ibo.bind();

  m_renderFunc(*this);
}

void Submesh::loadVertices() const {
  m_vao.bind();
  m_vbo.bind();

  const std::vector<Vertex>& vertices = getVertices();

  glBufferData(GL_ARRAY_BUFFER,
               static_cast<int64_t>(sizeof(vertices.front()) * vertices.size()),
               vertices.data(),
               GL_STATIC_DRAW);

  constexpr uint8_t stride = sizeof(vertices.front());

  glVertexAttribPointer(0, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        nullptr);
  glEnableVertexAttribArray(0);

  constexpr std::size_t positionSize = sizeof(vertices.front().position);
  glVertexAttribPointer(1, 2,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize));
  glEnableVertexAttribArray(1);

  constexpr std::size_t texcoordsSize = sizeof(vertices.front().texcoords);
  glVertexAttribPointer(2, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize + texcoordsSize));
  glEnableVertexAttribArray(2);

  constexpr std::size_t normalSize = sizeof(vertices.front().normal);
  glVertexAttribPointer(3, 3,
                        GL_FLOAT, GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(positionSize + texcoordsSize + normalSize));
  glEnableVertexAttribArray(3);

  m_vbo.unbind();
  m_vao.unbind();
}

void Submesh::loadIndices() const {
  m_vao.bind();
  m_ibo.bind();

  // Mapping the indices to lines' if asked, and triangles' otherwise
  const std::vector<unsigned int>& indices = (/*m_renderMode == RenderMode::LINE ? getLineIndices() : */getTriangleIndices());

  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<int64_t>(sizeof(indices.front()) * indices.size()),
               indices.data(),
               GL_STATIC_DRAW);

  m_ibo.unbind();
  m_vao.unbind();
}

} // namespace Raz
