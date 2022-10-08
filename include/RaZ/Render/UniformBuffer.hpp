#pragma once

#ifndef RAZ_UNIFORMBUFFER_HPP
#define RAZ_UNIFORMBUFFER_HPP

#include "RaZ/Data/OwnerValue.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Renderer.hpp"

#include <limits>

namespace Raz {

enum class UniformBufferUsage {
  STATIC,  ///< Data is assumed to never change.
  DYNAMIC, ///< Data is assumed to be changed often.
  STREAM   ///< Data is assumed to be given each frame.
};

class UniformBuffer {
public:
  UniformBuffer();
  explicit UniformBuffer(unsigned int size, UniformBufferUsage usage = UniformBufferUsage::DYNAMIC);
  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer(UniformBuffer&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }

  void bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int shaderBindingIndex) const;
  void bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int shaderBindingIndex) const;
  void bindBase(unsigned int bufferBindingIndex) const;
  void bindRange(unsigned int bufferBindingIndex, std::ptrdiff_t offset, std::ptrdiff_t size) const;
  void bind() const;
  void unbind() const;
  template <typename T> void sendData(T data, unsigned int offset) const { Renderer::sendBufferSubData(BufferType::UNIFORM_BUFFER, offset, sizeof(T), &data); }
  template <typename T, std::size_t Size>
  void sendData(const Vector<T, Size>& vec, unsigned int offset) const { Renderer::sendBufferSubData(BufferType::UNIFORM_BUFFER,
                                                                                                     offset, sizeof(vec), vec.getDataPtr()); }
  template <typename T, std::size_t W, std::size_t H>
  void sendData(const Matrix<T, W, H>& mat, unsigned int offset) const { Renderer::sendBufferSubData(BufferType::UNIFORM_BUFFER,
                                                                                                     offset, sizeof(mat), mat.getDataPtr()); }

  UniformBuffer& operator=(const UniformBuffer&) = delete;
  UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

  ~UniformBuffer();

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

} // namespace Raz

#endif // RAZ_UNIFORMBUFFER_HPP
