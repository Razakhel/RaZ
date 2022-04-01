#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

UniformBuffer::UniformBuffer() {
  Renderer::generateBuffer(m_index);
}

UniformBuffer::UniformBuffer(unsigned int size) : UniformBuffer() {
  Logger::debug("[UniformBuffer] Creating (with size: " + std::to_string(size) + ")...");

  bind();
  Renderer::sendBufferData(BufferType::UNIFORM_BUFFER, size, nullptr, BufferDataUsage::STATIC_DRAW); // Allocating memory
  unbind();

  Logger::debug("[UniformBuffer] Created (ID: " + std::to_string(m_index) + ')');
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
  : m_index{ std::exchange(ubo.m_index, std::numeric_limits<unsigned int>::max()) } {}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int shaderBindingIndex) const {
  Renderer::bindUniformBlock(program.getIndex(), uboIndex, shaderBindingIndex);
}

void UniformBuffer::bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int shaderBindingIndex) const {
  bindUniformBlock(program, Renderer::recoverUniformBlockIndex(program.getIndex(), uboName.c_str()), shaderBindingIndex);
}

void UniformBuffer::bindBase(unsigned int bufferBindingIndex) const {
  Renderer::bindBufferBase(BufferType::UNIFORM_BUFFER, bufferBindingIndex, m_index);
}

void UniformBuffer::bindRange(unsigned int bufferBindingIndex, std::ptrdiff_t offset, std::ptrdiff_t size) const {
  Renderer::bindBufferRange(BufferType::UNIFORM_BUFFER, bufferBindingIndex, m_index, offset, size);
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
