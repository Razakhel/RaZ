#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

UniformBuffer::UniformBuffer() {
  Renderer::generateBuffer(m_index);
}

UniformBuffer::UniformBuffer(unsigned int size, unsigned int bindingIndex) : UniformBuffer() {
  Logger::debug("[UniformBuffer] Creating (with size: " + std::to_string(size) + ")...");

  bind();
  Renderer::bindBufferRange(BufferType::UNIFORM_BUFFER, bindingIndex, m_index, 0, size);
  Renderer::sendBufferData(BufferType::UNIFORM_BUFFER, size, nullptr, BufferDataUsage::STATIC_DRAW);
  unbind();

  Logger::debug("[UniformBuffer] Created (ID: " + std::to_string(m_index) + ')');
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
  : m_index{ std::exchange(ubo.m_index, std::numeric_limits<unsigned int>::max()) } {}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int bindingIndex) const {
  Renderer::bindUniformBlock(program.getIndex(), uboIndex, bindingIndex);
}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int bindingIndex) const {
  bindUniformBlock(program, Renderer::recoverUniformBlockIndex(program.getIndex(), uboName.c_str()), bindingIndex);
}

void UniformBuffer::bindBase(unsigned int bindingIndex) const {
  Renderer::bindBufferBase(BufferType::UNIFORM_BUFFER, bindingIndex, m_index);
}

void UniformBuffer::bindRange(unsigned int bindingIndex, std::ptrdiff_t offset, std::ptrdiff_t size) const {
  Renderer::bindBufferRange(BufferType::UNIFORM_BUFFER, bindingIndex, m_index, offset, size);
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

  Logger::debug("[UniformBuffer] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteBuffer(m_index);
  Logger::debug("[UniformBuffer] Destroyed");
}

} // namespace Raz
