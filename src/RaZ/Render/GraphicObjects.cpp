#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

VertexArray::VertexArray() {
  Logger::debug("[VertexArray] Creating...");
  Renderer::generateVertexArray(m_index);
  Logger::debug("[VertexArray] Created (ID: " + std::to_string(m_index) + ')');
}

void VertexArray::bind() const {
  Renderer::bindVertexArray(m_index);
}

void VertexArray::unbind() const {
  Renderer::unbindVertexArray();
}

VertexArray::~VertexArray() {
  if (!m_index.isValid())
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

void VertexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ARRAY_BUFFER, m_index);
}

void VertexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ARRAY_BUFFER);
}

VertexBuffer::~VertexBuffer() {
  if (!m_index.isValid())
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

void IndexBuffer::bind() const {
  Renderer::bindBuffer(BufferType::ELEMENT_BUFFER, m_index);
}

void IndexBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::ELEMENT_BUFFER);
}

IndexBuffer::~IndexBuffer() {
  if (!m_index.isValid())
    return;

  Logger::debug("[IndexBuffer] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteBuffer(m_index);
  Logger::debug("[IndexBuffer] Destroyed");
}

} // namespace Raz
