#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Submesh.hpp"

namespace Raz {

void Submesh::setRenderMode(RenderMode renderMode) {
  m_renderMode = renderMode;

  switch (m_renderMode) {
    case RenderMode::POINT:
      m_renderFunc = [] (const Submesh& submesh) { glDrawArrays(GL_POINTS, 0, static_cast<int>(submesh.getVertexCount())); };
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

    case RenderMode::TRIANGLE:
    default:
    {
      m_renderFunc = [] (const Submesh& submesh) {
        glDrawElements(GL_TRIANGLES, static_cast<int>(submesh.getTriangleIndexCount()), GL_UNSIGNED_INT, nullptr);
      };

      break;
    }
  }

  loadIndices();
}

const AABB& Submesh::computeBoundingBox() {
  Vec3f maxPos;
  Vec3f minPos;

  for (const Vertex& vert : m_vbo.getVertices()) {
    maxPos[0] = std::max(maxPos[0], vert.position[0]);
    maxPos[1] = std::max(maxPos[1], vert.position[1]);
    maxPos[2] = std::max(maxPos[2], vert.position[2]);

    minPos[0] = std::min(minPos[0], vert.position[0]);
    minPos[1] = std::min(minPos[1], vert.position[1]);
    minPos[2] = std::min(minPos[2], vert.position[2]);
  }

  m_boundingBox = AABB(minPos, maxPos);
  return m_boundingBox;
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

  Renderer::sendBufferData(BufferType::ARRAY_BUFFER,
                           static_cast<std::ptrdiff_t>(sizeof(vertices.front()) * vertices.size()),
                           vertices.data(),
                           BufferDataUsage::STATIC_DRAW);

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

  Renderer::sendBufferData(BufferType::ELEMENT_BUFFER,
                           static_cast<std::ptrdiff_t>(sizeof(indices.front()) * indices.size()),
                           indices.data(),
                           BufferDataUsage::STATIC_DRAW);

  m_ibo.unbind();
  m_vao.unbind();
}

} // namespace Raz
