#include "GL/glew.h"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_index);
}

VertexArray::VertexArray(VertexArray&& vertexArray) noexcept
  : m_index{ std::exchange(vertexArray.m_index, std::numeric_limits<unsigned int>::max()) } {}

void VertexArray::bind() const {
  glBindVertexArray(m_index);
}

void VertexArray::unbind() const {
  glBindVertexArray(0);
}

VertexArray& VertexArray::operator=(VertexArray&& vertexArray) noexcept {
  std::swap(m_index, vertexArray.m_index);
  return *this;
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_index);
}

VertexBuffer::VertexBuffer() {
  Renderer::generateBuffer(m_index);
}

VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer) noexcept
  : m_index{ std::exchange(vertexBuffer.m_index, std::numeric_limits<unsigned int>::max()) } {}

void VertexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ARRAY_BUFFER, m_index);
}

void VertexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ARRAY_BUFFER);
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& vertexBuffer) noexcept {
  std::swap(m_index, vertexBuffer.m_index);
  return *this;
}

VertexBuffer::~VertexBuffer() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteBuffer(m_index);
}

IndexBuffer::IndexBuffer() {
  Renderer::generateBuffer(m_index);
}

IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer) noexcept
  : m_index{ std::exchange(indexBuffer.m_index, std::numeric_limits<unsigned int>::max()) } {}

void IndexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ELEMENT_BUFFER, m_index);
}

void IndexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ELEMENT_BUFFER);
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& indexBuffer) noexcept {
  std::swap(m_index, indexBuffer.m_index);
  return *this;
}

IndexBuffer::~IndexBuffer() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteBuffer(m_index);
}

} // namespace Raz
