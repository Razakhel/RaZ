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
  Vec3f maxPos(std::numeric_limits<float>::lowest());
  Vec3f minPos(std::numeric_limits<float>::max());

  for (const Vertex& vert : m_vbo.getVertices()) {
    maxPos.x() = std::max(maxPos.x(), vert.position.x());
    maxPos.y() = std::max(maxPos.y(), vert.position.y());
    maxPos.z() = std::max(maxPos.z(), vert.position.z());

    minPos.x() = std::min(minPos.x(), vert.position.x());
    minPos.y() = std::min(minPos.y(), vert.position.y());
    minPos.z() = std::min(minPos.z(), vert.position.z());
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

  // Position
  glVertexAttribPointer(0,
                        3, GL_FLOAT, // vec3
                        GL_FALSE,
                        stride,
                        nullptr); // Position offset is 0
  glEnableVertexAttribArray(0);

  // Texcoords
  constexpr std::size_t texcoordsOffset = sizeof(vertices.front().position);
  glVertexAttribPointer(1,
                        2, GL_FLOAT, // vec2
                        GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(texcoordsOffset));
  glEnableVertexAttribArray(1);

  // Normal
  constexpr std::size_t normalOffset = texcoordsOffset + sizeof(vertices.front().texcoords);
  glVertexAttribPointer(2,
                        3, GL_FLOAT, // vec3
                        GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(normalOffset));
  glEnableVertexAttribArray(2);

  // Tangent
  constexpr std::size_t tangentOffset = normalOffset + sizeof(vertices.front().normal);
  glVertexAttribPointer(3,
                        3, GL_FLOAT, // vec3
                        GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(tangentOffset));
  glEnableVertexAttribArray(3);

  // Bone indices
  constexpr std::size_t boneIndicesOffset = tangentOffset + sizeof(vertices.front().tangent);
  glVertexAttribIPointer(4,
                         4, GL_INT, // ivec4
                         stride,
                         reinterpret_cast<void*>(boneIndicesOffset));
  glEnableVertexAttribArray(4);

  // Bone weights
  constexpr std::size_t boneWeightsOffset = boneIndicesOffset + sizeof(vertices.front().boneIndices);
  glVertexAttribPointer(5,
                        4, GL_FLOAT, // vec4
                        GL_FALSE,
                        stride,
                        reinterpret_cast<void*>(boneWeightsOffset));
  glEnableVertexAttribArray(5);

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
