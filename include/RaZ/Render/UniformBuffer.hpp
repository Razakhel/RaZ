#pragma once

#ifndef RAZ_UNIFORMBUFFER_HPP
#define RAZ_UNIFORMBUFFER_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

class UniformBuffer {
public:
  UniformBuffer();
  explicit UniformBuffer(unsigned int size, unsigned int bindingIndex);
  UniformBuffer(const UniformBuffer&) = delete;
  UniformBuffer(UniformBuffer&& ubo) noexcept;

  unsigned int getIndex() const { return m_index; }

  void bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int bindingIndex) const;
  void bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int bindingIndex) const;
  void bindBufferBase(unsigned int bindingIndex) const;
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
  UniformBuffer& operator=(UniformBuffer&& ubo) noexcept;

  ~UniformBuffer();

private:
  unsigned int m_index {};
};

} // namespace Raz

#endif // RAZ_UNIFORMBUFFER_HPP
