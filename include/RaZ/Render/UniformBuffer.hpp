#pragma once

#ifndef RAZ_UNIFORMBUFFER_HPP
#define RAZ_UNIFORMBUFFER_HPP

#include "glew/include/GL/glew.h"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

class UniformBuffer {
public:
  UniformBuffer() { glGenBuffers(1, &m_index); }
  explicit UniformBuffer(unsigned int size, unsigned int bindingIndex);
  UniformBuffer(const UniformBuffer&) = default;

  GLuint getIndex() const { return m_index; }

  void bindUniformBlock(const ShaderProgram& program, unsigned int uboIndex, unsigned int bindingIndex) const;
  void bindUniformBlock(const ShaderProgram& program, const std::string& uboName, unsigned int bindingIndex) const;
  void bindBufferBase(unsigned int bindingIndex) const { glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, m_index); }
  void bind() const { glBindBuffer(GL_UNIFORM_BUFFER, m_index); }
  void unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
  template <typename T> void sendData(T data, unsigned int offset) const;
  template <typename T, std::size_t Size> void sendData(const Vector<T, Size>& vec, unsigned int offset) const;
  template <typename T, std::size_t W, std::size_t H> void sendData(const Matrix<T, W, H>& mat, unsigned int offset) const;

  ~UniformBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index {};
};

} // namespace Raz

#include "RaZ/Render/UniformBuffer.inl"

#endif // RAZ_UNIFORMBUFFER_HPP
