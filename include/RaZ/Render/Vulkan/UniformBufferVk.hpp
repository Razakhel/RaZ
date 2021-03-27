#pragma once

#ifndef RAZ_UNIFORMBUFFERVK_HPP
#define RAZ_UNIFORMBUFFERVK_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"

using VkBuffer       = struct VkBuffer_T*;
using VkDeviceMemory = struct VkDeviceMemory_T*;

namespace Raz {

class UniformBuffer {
public:
  explicit UniformBuffer(uint64_t size, unsigned int bindingIndex);
  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer(UniformBuffer&& ubo) noexcept;

  uint32_t getBindingIndex() const { return m_bindingIndex; }

  void bind() const {}
  void bindBufferBase(int) const {}
  /// Sends some data as uniform.
  /// \tparam T Type of the data.
  /// \param data Data to be sent.
  /// \param offset Offset to start sending the data from.
  template <typename T>
  void sendData(T&& data, uint64_t offset = 0) const noexcept { sendData(sizeof(T), &data, offset); }
  /// Sends some data as uniform.
  /// \param dataSize Size of the data.
  /// \param data Data to be sent.
  /// \param offset Offset to start sending the data from.
  void sendData(uint64_t dataSize, const void* data, uint64_t offset = 0) const noexcept;
  /// Destroys the uniform buffer.
  void destroy();

  UniformBuffer& operator=(const UniformBuffer&) = delete;
  UniformBuffer& operator=(UniformBuffer&& ubo) noexcept;

  ~UniformBuffer() { destroy(); }

private:
  VkBuffer m_buffer {};
  VkDeviceMemory m_bufferMemory {};
  uint32_t m_bindingIndex {};
};

} // namespace Raz

#endif // RAZ_UNIFORMBUFFERVK_HPP
