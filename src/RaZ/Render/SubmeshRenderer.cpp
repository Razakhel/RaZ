#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/SubmeshRenderer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

void SubmeshRenderer::setRenderMode(RenderMode renderMode, const Submesh& submesh) {
  m_renderMode = renderMode;

  switch (m_renderMode) {
    case RenderMode::POINT:
      m_renderFunc = [] (const VertexBuffer& vertexBuffer, const IndexBuffer&, unsigned int instanceCount) {
        Renderer::drawArraysInstanced(PrimitiveType::POINTS, vertexBuffer.vertexCount, instanceCount);
      };
      break;

//    case RenderMode::LINE:
//      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer, unsigned int instanceCount) {
//        Renderer::drawElementsInstanced(PrimitiveType::LINES, indexBuffer.lineIndexCount, instanceCount);
//      };
//      break;

    case RenderMode::TRIANGLE:
    default:
      m_renderFunc = [] (const VertexBuffer&, const IndexBuffer& indexBuffer, unsigned int instanceCount) {
        Renderer::drawElementsInstanced(PrimitiveType::TRIANGLES, indexBuffer.triangleIndexCount, instanceCount);
      };
      break;

#if !defined(USE_OPENGL_ES)
    case RenderMode::PATCH:
      m_renderFunc = [] (const VertexBuffer& vertexBuffer, const IndexBuffer&, unsigned int instanceCount) {
        Renderer::drawArraysInstanced(PrimitiveType::PATCHES, vertexBuffer.vertexCount, instanceCount);
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
  loadVertices(submesh);
  setRenderMode(renderMode, submesh);
}

void SubmeshRenderer::draw(unsigned int instanceCount) const {
  m_vao.bind();
  m_ibo.bind();

  m_renderFunc(m_vbo, m_ibo, instanceCount);
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

  // Instance matrix (4 rows of vec4)

  const VertexBuffer& instanceBuffer = MeshRenderer::getInstanceBuffer();

  instanceBuffer.bind();

  for (uint8_t i = 0; i < 4; ++i) {
    const unsigned int newIndex = 4 + i;

    Renderer::setVertexAttrib(newIndex,
                              AttribDataType::FLOAT, 4, // vec4
                              sizeof(Mat4f), sizeof(Vec4f) * i);
    Renderer::setVertexAttribDivisor(newIndex, 1);
    Renderer::enableVertexAttribArray(newIndex);
  }

  instanceBuffer.unbind();

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
