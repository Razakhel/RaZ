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

  submeshRenderer.m_renderMode     = m_renderMode;
  submeshRenderer.m_enabledAttribs = m_enabledAttribs;
  submeshRenderer.m_renderFunc     = m_renderFunc;
  submeshRenderer.m_materialIndex  = m_materialIndex;

  return submeshRenderer;
}

void SubmeshRenderer::load(const Submesh& submesh, RenderMode renderMode, VertexAttribute enabledAttribs) {
  ZoneScopedN("SubmeshRenderer::load");

  m_enabledAttribs = enabledAttribs;
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

  // Pack only enabled attributes into a dense float buffer
  const bool hasPosition  = static_cast<bool>(m_enabledAttribs & VertexAttribute::Position);
  const bool hasTexcoords = static_cast<bool>(m_enabledAttribs & VertexAttribute::Texcoords);
  const bool hasNormal    = static_cast<bool>(m_enabledAttribs & VertexAttribute::Normal);
  const bool hasTangent   = static_cast<bool>(m_enabledAttribs & VertexAttribute::Tangent);
  const bool hasColor     = static_cast<bool>(m_enabledAttribs & VertexAttribute::Color);

  const unsigned int stride = (hasPosition  ? 3 : 0)
                            + (hasTexcoords ? 2 : 0)
                            + (hasNormal    ? 3 : 0)
                            + (hasTangent   ? 3 : 0)
                            + (hasColor     ? 4 : 0);

  std::vector<float> packedData;
  packedData.reserve(vertices.size() * stride);

  for (const Vertex& vert : vertices) {
    if (hasPosition) {
      packedData.push_back(vert.position[0]);
      packedData.push_back(vert.position[1]);
      packedData.push_back(vert.position[2]);
    }
    if (hasTexcoords) {
      packedData.push_back(vert.texcoords[0]);
      packedData.push_back(vert.texcoords[1]);
    }
    if (hasNormal) {
      packedData.push_back(vert.normal[0]);
      packedData.push_back(vert.normal[1]);
      packedData.push_back(vert.normal[2]);
    }
    if (hasTangent) {
      packedData.push_back(vert.tangent[0]);
      packedData.push_back(vert.tangent[1]);
      packedData.push_back(vert.tangent[2]);
    }
    if (hasColor) {
      packedData.push_back(vert.color[0]);
      packedData.push_back(vert.color[1]);
      packedData.push_back(vert.color[2]);
      packedData.push_back(vert.color[3]);
    }
  }

  Renderer::sendBufferData(BufferType::ARRAY_BUFFER,
                           static_cast<std::ptrdiff_t>(packedData.size() * sizeof(float)),
                           packedData.data(),
                           BufferDataUsage::STATIC_DRAW);

  m_vbo.vertexCount = static_cast<unsigned int>(vertices.size());

  // Bind shader locations
  const auto byteStride   = stride * static_cast<unsigned int>(sizeof(float));
  unsigned int byteOffset = 0;

  auto bindAttrib = [&byteOffset, byteStride] (unsigned int location, uint8_t compCount, bool enabled) {
    if (enabled) {
      Renderer::setVertexAttrib(location, AttribDataType::FLOAT, compCount, byteStride, byteOffset);
      Renderer::enableVertexAttribArray(location);
      byteOffset += compCount * static_cast<unsigned int>(sizeof(float));
    } else {
      Renderer::disableVertexAttribArray(location);
    }
  };

  bindAttrib(0, 3, hasPosition);  // vec3 position
  bindAttrib(1, 2, hasTexcoords); // vec2 texcoords
  bindAttrib(2, 3, hasNormal);    // vec3 normal
  bindAttrib(3, 3, hasTangent);   // vec3 tangent
  bindAttrib(4, 4, hasColor);     // vec4 color

  // Color
  constexpr std::size_t colorOffset = tangentOffset + sizeof(vertices.front().tangent);
  Renderer::setVertexAttrib(4,
                            AttribDataType::FLOAT, 3, // vec3
                            stride, colorOffset);
  Renderer::enableVertexAttribArray(4);

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
