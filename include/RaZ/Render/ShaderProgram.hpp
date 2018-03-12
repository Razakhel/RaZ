#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#include <string>

#include "glew/include/GL/glew.h"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

class ShaderProgram {
public:
  ShaderProgram() : m_index{ glCreateProgram() } {}
  ShaderProgram(const VertexShader& vertShader, const FragmentShader& fragShader)
    : ShaderProgram() { attachShaders(vertShader, fragShader); }

  GLuint getIndex() const { return m_index; }

  void attachShaders(const VertexShader& vertShader, const FragmentShader& fragShader) const;
  void use() const { glUseProgram(m_index); }
  GLint recoverUniformLocation(const std::string& uniformName) const { return glGetUniformLocation(m_index, uniformName.c_str()); }
  template <typename T> void sendUniform(GLint uniformIndex, T value) const;
  template <typename T, std::size_t Size> void sendUniform(GLint uniformIndex, const Vector<T, Size>& vec) const;
  template <typename T, std::size_t W, std::size_t H> void sendUniform(GLint uniformIndex, const Matrix<T, W, H>& mat) const;
  template <typename T> void sendUniform(const std::string& uniformName, T value) const {
    sendUniform(recoverUniformLocation(uniformName), value);
  }

private:
  GLuint m_index;
};

} // namespace Raz

#endif // RAZ_SHADERPROGRAM_HPP
