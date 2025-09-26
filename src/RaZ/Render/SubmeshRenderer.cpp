#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/SubmeshRenderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

void SubmeshRenderer::setRenderMode(RenderMode renderMode, const Submesh& submesh) {
  ZoneScopedN("SubmeshRenderer::setRenderMode");

  m_renderMode = renderMode;

  switch (m_renderMode) {
    case RenderMode::POINT:
      m_renderFunc = [] (const VertexBuffer& vertexBuffer, const IndexBuffer&) {
        Renderer::drawArrays(PrimitiveType::POINTS, vertexBuffer.vertexCount);
      };
      break;

//    case RenderMode::LINE: {
//      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer) {
//        Renderer::drawElements(PrimitiveType::LINES, indexBuffer.lineIndexCount);
//      };
//      break;
//    }

    case RenderMode::TRIANGLE:
    default:
      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer) {
        Renderer::drawElements(PrimitiveType::TRIANGLES, indexBuffer.triangleIndexCount);
      };
      break;

#if !defined(USE_OPENGL_ES)
    case RenderMode::PATCH:
      m_renderFunc = [] (const VertexBuffer& vertexBuffer, const IndexBuffer&) {
        Renderer::drawArrays(PrimitiveType::PATCHES, vertexBuffer.vertexCount);
      };
      Renderer::setPatchVertexCount(3); // Should be the default, but just in case
      break;
#endif
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
  ZoneScopedN("SubmeshRenderer::load");

  loadVertices(submesh);
  setRenderMode(renderMode, submesh);
}

void SubmeshRenderer::draw() const {
  ZoneScopedN("SubmeshRenderer::draw");
  TracyGpuZone("SubmeshRenderer::draw")

  m_vao.bind();
  m_ibo.bind();

  m_renderFunc(m_vbo, m_ibo);
}

void SubmeshRenderer::loadVertices(const Submesh& submesh) {
  ZoneScopedN("SubmeshRenderer::loadVertices");

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
  Renderer::setVertexAttrib(0,
                            AttribDataType::FLOAT, 3, // vec3
                            stride, 0);
  Renderer::enableVertexAttribArray(0);

  // Texcoords
  constexpr std::size_t texcoordsOffset = sizeof(vertices.front().position);
  Renderer::setVertexAttrib(1,
                            AttribDataType::FLOAT, 2, // vec2
                            stride, texcoordsOffset);
  Renderer::enableVertexAttribArray(1);

  // Normal
  constexpr std::size_t normalOffset = texcoordsOffset + sizeof(vertices.front().texcoords);
  Renderer::setVertexAttrib(2,
                            AttribDataType::FLOAT, 3, // vec3
                            stride, normalOffset);
  Renderer::enableVertexAttribArray(2);

  // Tangent
  constexpr std::size_t tangentOffset = normalOffset + sizeof(vertices.front().normal);
  Renderer::setVertexAttrib(3,
                            AttribDataType::FLOAT, 3, // vec3
                            stride, tangentOffset);
  Renderer::enableVertexAttribArray(3);

  m_vbo.unbind();
  m_vao.unbind();

  Logger::debug("[SubmeshRenderer] Loaded submesh vertices ({} vertices loaded)", vertices.size());
}

void SubmeshRenderer::loadIndices(const Submesh& submesh) {
  ZoneScopedN("SubmeshRenderer::loadIndices");

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

  Logger::debug("[SubmeshRenderer] Loaded submesh indices ({} indices loaded)", indices.size());
}

} // namespace Raz
