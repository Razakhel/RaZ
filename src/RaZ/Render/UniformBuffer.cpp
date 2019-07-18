#include "GL/glew.h"
#include "RaZ/Render/UniformBuffer.hpp"

namespace Raz {

UniformBuffer::UniformBuffer() {
  Renderer::generateBuffers(1, &m_index);
}

UniformBuffer::UniformBuffer(unsigned int size, unsigned int bindingIndex) : UniformBuffer() {
  bind();
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, m_index, 0, size);
  unbind();
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
  : m_index{ std::exchange(ubo.m_index, GL_INVALID_INDEX) } {}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int bindingIndex) const {
  glUniformBlockBinding(program.getIndex(), uboIndex, bindingIndex);
}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int bindingIndex) const {
  bindUniformBlock(program, glGetUniformBlockIndex(program.getIndex(), uboName.c_str()), bindingIndex);
}

void UniformBuffer::bindBufferBase(unsigned int bindingIndex) const {
  Renderer::bindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, m_index);
}

void UniformBuffer::bind() const {
  Renderer::bindBuffer(GL_UNIFORM_BUFFER, m_index);
}

void UniformBuffer::unbind() const {
  Renderer::unbindBuffer(GL_UNIFORM_BUFFER);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& ubo) noexcept {
  std::swap(m_index, ubo.m_index);

  return *this;
}

UniformBuffer::~UniformBuffer() {
  if (m_index == GL_INVALID_INDEX)
    return;

  Renderer::deleteBuffers(1, &m_index);
}

} // namespace Raz
