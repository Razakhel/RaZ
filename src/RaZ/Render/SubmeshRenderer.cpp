#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/SubmeshRenderer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

void SubmeshRenderer::setRenderMode(RenderMode renderMode, const Submesh& submesh) {
  m_renderMode = renderMode;

  switch (m_renderMode) {
    case RenderMode::POINT:
      m_renderFunc = [] (const VertexBuffer& vertexBuffer, const IndexBuffer&) {
        glDrawArrays(GL_POINTS, 0, static_cast<int>(vertexBuffer.vertexCount));
      };

      break;

//    case RenderMode::LINE: {
//      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer) {
//        glDrawElements(GL_LINES, static_cast<int>(indexBuffer.lineIndexCount), GL_UNSIGNED_INT, nullptr);
//      };
//
//      break;
//    }

    case RenderMode::TRIANGLE:
    default:
      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer) {
        glDrawElements(GL_TRIANGLES, static_cast<int>(indexBuffer.triangleIndexCount), GL_UNSIGNED_INT, nullptr);
      };

      break;
  }

  loadIndices(submesh);
}

SubmeshRenderer SubmeshRenderer::clone() const {
  SubmeshRenderer submeshRenderer;

  submeshRenderer.m_renderMode    = m_renderMode;
  submeshRenderer.m_renderFunc    = m_renderFunc;
  submeshRenderer.m_materialIndex = m_materialIndex;

  return submeshRenderer;
}

void SubmeshRenderer::load(const Submesh& submesh, RenderMode renderMode) {
  loadVertices(submesh);
  setRenderMode(renderMode, submesh);
}

void SubmeshRenderer::draw() const {
  m_vao.bind();
  m_ibo.bind();

  m_renderFunc(m_vbo, m_ibo);
}

void SubmeshRenderer::loadVertices(const Submesh& submesh) {
  Logger::debug("[SubmeshRenderer] Loading submesh vertices...");

  m_vao.bind();
  m_vbo.bind();

  const std::vector<Vertex>& vertices = submesh.getVertices();

  Renderer::sendBufferData(BufferType::ARRAY_BUFFER,
                           static_cast<std::ptrdiff_t>(sizeof(vertices.front()) * vertices.size()),
                           vertices.data(),
                           BufferDataUsage::STATIC_DRAW);

  m_vbo.vertexCount = static_cast<unsigned int>(vertices.size());

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

  m_vbo.unbind();
  m_vao.unbind();

  Logger::debug("[SubmeshRenderer] Loaded submesh vertices (" + std::to_string(vertices.size()) + " vertices loaded)");
}

void SubmeshRenderer::loadIndices(const Submesh& submesh) {
  Logger::debug("[SubmeshRenderer] Loading submesh indices...");

  m_vao.bind();
  m_ibo.bind();

  // Mapping the indices to lines' if asked, and triangles' otherwise
  const std::vector<unsigned int>& indices = (/*m_renderMode == RenderMode::LINE ? submesh.getLineIndices() : */submesh.getTriangleIndices());

  Renderer::sendBufferData(BufferType::ELEMENT_BUFFER,
                           static_cast<std::ptrdiff_t>(sizeof(indices.front()) * indices.size()),
                           indices.data(),
                           BufferDataUsage::STATIC_DRAW);

  m_ibo.lineIndexCount     = static_cast<unsigned int>(submesh.getLineIndexCount());
  m_ibo.triangleIndexCount = static_cast<unsigned int>(submesh.getTriangleIndexCount());

  m_ibo.unbind();
  m_vao.unbind();

  Logger::debug("[SubmeshRenderer] Loaded submesh indices (" + std::to_string(indices.size()) + " indices loaded)");
}

} // namespace Raz
