#include "GL/glew.h"
#include "RaZ/Render/GraphicObjects.hpp"

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
  if (m_index == GL_INVALID_INDEX)
    return;

  glGenBuffers(1, &m_index);
}

VertexBuffer::VertexBuffer(VertexBuffer&& vbo) noexcept
  : m_index{ std::exchange(vbo.m_index, GL_INVALID_INDEX) }, m_vertices{ std::move(vbo.m_vertices) } {}

void VertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_index);
}

void VertexBuffer::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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
}

IndexBuffer::IndexBuffer() {
  glGenBuffers(1, &m_index);
}

IndexBuffer::IndexBuffer(IndexBuffer&& ibo) noexcept
  : m_index{ std::exchange(ibo.m_index, GL_INVALID_INDEX) },
    m_lineIndices{ std::move(ibo.m_lineIndices) },
    m_triangleIndices{ std::move(ibo.m_triangleIndices) } {}

void IndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
}

void IndexBuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ibo) noexcept {
  std::swap(m_index, ibo.m_index);
  m_lineIndices = std::move(ibo.m_lineIndices);
  m_triangleIndices = std::move(ibo.m_triangleIndices);

  return *this;
}

IndexBuffer::~IndexBuffer() {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteBuffers(1, &m_index);
}

} // namespace Raz
