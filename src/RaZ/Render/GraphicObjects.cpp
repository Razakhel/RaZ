#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <utility>

namespace Raz {

VertexArray::VertexArray() {
  Logger::debug("[VertexArray] Creating...");
  Renderer::generateVertexArray(m_index);
  Logger::debug("[VertexArray] Created (ID: " + std::to_string(m_index) + ')');
}

VertexArray::VertexArray(VertexArray&& vertexArray) noexcept
  : m_index{ std::exchange(vertexArray.m_index, std::numeric_limits<unsigned int>::max()) } {}

void VertexArray::bind() const {
  Renderer::bindVertexArray(m_index);
}

void VertexArray::unbind() const {
  Renderer::unbindVertexArray();
}

VertexArray& VertexArray::operator=(VertexArray&& vertexArray) noexcept {
  std::swap(m_index, vertexArray.m_index);
  return *this;
}

VertexArray::~VertexArray() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Logger::debug("[VertexArray] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteVertexArray(m_index);
  Logger::debug("[VertexArray] Destroyed");
}

VertexBuffer::VertexBuffer() {
  Logger::debug("[VertexBuffer] Creating...");
  Renderer::generateBuffer(m_index);
  Logger::debug("[VertexBuffer] Created (ID: " + std::to_string(m_index) + ')');
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

  Logger::debug("[VertexBuffer] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteBuffer(m_index);
  Logger::debug("[VertexBuffer] Destroyed");
}

IndexBuffer::IndexBuffer() {
  Logger::debug("[IndexBuffer] Creating...");
  Renderer::generateBuffer(m_index);
  Logger::debug("[IndexBuffer] Created (ID: " + std::to_string(m_index) + ')');
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

  Logger::debug("[IndexBuffer] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteBuffer(m_index);
  Logger::debug("[IndexBuffer] Destroyed");
}

} // namespace Raz
