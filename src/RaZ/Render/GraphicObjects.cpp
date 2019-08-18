#include "GL/glew.h"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_index);
}

VertexArray::VertexArray(VertexArray&& vao) noexcept
  : m_index{ std::exchange(vao.m_index, GL_INVALID_INDEX) } {}

void VertexArray::bind() const {
  glBindVertexArray(m_index);
}

void VertexArray::unbind() const {
  glBindVertexArray(0);
}

VertexArray& VertexArray::operator=(VertexArray&& vao) noexcept {
  std::swap(m_index, vao.m_index);

  return *this;
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_index);
}

VertexBuffer::VertexBuffer() {
  Renderer::generateBuffer(m_index);
}

VertexBuffer::VertexBuffer(VertexBuffer&& vbo) noexcept
  : m_index{ std::exchange(vbo.m_index, GL_INVALID_INDEX) }, m_vertices{ std::move(vbo.m_vertices) } {}

void VertexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ARRAY_BUFFER, m_index);
}

void VertexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ARRAY_BUFFER);
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vbo) noexcept {
  std::swap(m_index, vbo.m_index);
  m_vertices = std::move(vbo.m_vertices);

  return *this;
}

VertexBuffer::~VertexBuffer() {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteBuffers(1, &m_index);

  // This currently goes on an infinite error-printing loop
  //Renderer::deleteBuffer(m_index);
}

IndexBuffer::IndexBuffer() {
  Renderer::generateBuffer(m_index);
}

IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept
  : m_index{ std::exchange(ibo.m_index, GL_INVALID_INDEX) },
    m_lineIndices{ std::move(ibo.m_lineIndices) },
    m_triangleIndices{ std::move(ibo.m_triangleIndices) } {}

void IndexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ELEMENT_BUFFER, m_index);
}

void IndexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ELEMENT_BUFFER);
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept {
  std::swap(m_index, ibo.m_index);
  m_lineIndices     = std::move(ibo.m_lineIndices);
  m_triangleIndices = std::move(ibo.m_triangleIndices);

  return *this;
}

IndexBuffer::~IndexBuffer() {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteBuffers(1, &m_index);

  // This currently goes on an infinite error-printing loop
  //Renderer::deleteBuffer(m_index);
}

} // namespace Raz
