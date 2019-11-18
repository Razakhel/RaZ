#include "GL/glew.h"
#include "RaZ/Render/UniformBuffer.hpp"

namespace Raz {

UniformBuffer::UniformBuffer() {
  Renderer::generateBuffer(m_index);
}

UniformBuffer::UniformBuffer(unsigned int size, unsigned int bindingIndex) : UniformBuffer() {
  bind();
  Renderer::bindBufferRange(BufferType::UNIFORM_BUFFER, bindingIndex, m_index, 0, size);
  Renderer::sendBufferData(BufferType::UNIFORM_BUFFER, size, nullptr, BufferDataUsage::STATIC_DRAW);
  unbind();
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
  : m_index{ std::exchange(ubo.m_index, std::numeric_limits<unsigned int>::max()) } {}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int bindingIndex) const {
  glUniformBlockBinding(program.getIndex(), uboIndex, bindingIndex);
}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int bindingIndex) const {
  bindUniformBlock(program, glGetUniformBlockIndex(program.getIndex(), uboName.c_str()), bindingIndex);
}

void UniformBuffer::bindBufferBase(unsigned int bindingIndex) const {
  Renderer::bindBufferBase(BufferType::UNIFORM_BUFFER, bindingIndex, m_index);
}

void UniformBuffer::bind() const {
  Renderer::bindBuffer(BufferType::UNIFORM_BUFFER, m_index);
}

void UniformBuffer::unbind() const {
  Renderer::unbindBuffer(BufferType::UNIFORM_BUFFER);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& ubo) noexcept {
  std::swap(m_index, ubo.m_index);

  return *this;
}

UniformBuffer::~UniformBuffer() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteBuffer(m_index);
}

} // namespace Raz
