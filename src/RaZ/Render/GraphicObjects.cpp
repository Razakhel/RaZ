#include "GL/glew.h"
#include "RaZ/Render/GraphicObjects.hpp"

namespace Raz {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_index);
}

void VertexArray::bind() const {
  glBindVertexArray(m_index);
}

void VertexArray::unbind() const {
  glBindVertexArray(0);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_index);
}

VertexBuffer::VertexBuffer() {
  glGenBuffers(1, &m_index);
}

void VertexBuffer::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_index);
}

void VertexBuffer::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &m_index);
}

IndexBuffer::IndexBuffer() {
  glGenBuffers(1, &m_index);
}

void IndexBuffer::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
}

void IndexBuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer() {
  glDeleteBuffers(1, &m_index);
}

} // namespace Raz
