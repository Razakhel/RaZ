#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/UniformBuffer.hpp"

namespace Raz {

UniformBuffer::UniformBuffer(uint64_t size, uint32_t bindingIndex) : m_bindingIndex{ bindingIndex } {
  Renderer::createBuffer(m_buffer,
                         m_bufferMemory,
                         BufferUsage::UNIFORM_BUFFER,
                         MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
                         size);
}

UniformBuffer::UniformBuffer(UniformBuffer&& ubo) noexcept
  : m_buffer{ std::exchange(ubo.m_buffer, nullptr) },
    m_bufferMemory{ std::exchange(ubo.m_bufferMemory, nullptr) },
    m_bindingIndex{ std::exchange(ubo.m_bindingIndex, std::numeric_limits<uint32_t>::max()) } {}

void UniformBuffer::sendData(uint64_t dataSize, const void* data, uint64_t offset) const noexcept {
  Renderer::copyMemory(m_bufferMemory, dataSize, data, offset);
}

void UniformBuffer::destroy() {
  if (m_buffer == nullptr)
    return;

  Renderer::destroyBuffer(m_buffer, m_bufferMemory);
  m_buffer       = nullptr;
  m_bufferMemory = nullptr;
  m_bindingIndex = std::numeric_limits<uint32_t>::max();
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& ubo) noexcept {
  std::swap(m_buffer, ubo.m_buffer);
  std::swap(m_bufferMemory, ubo.m_bufferMemory);
  std::swap(m_bindingIndex, ubo.m_bindingIndex);

  return *this;
}

} // namespace Raz
