#include <array>
#include <iostream>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

ShaderProgram::ShaderProgram(VertexShaderPtr vertShader, FragmentShaderPtr fragShader) : m_index{ glCreateProgram() },
                                                                                         m_vertShader{ std::move(vertShader) },
                                                                                         m_fragShader{ std::move(fragShader) } {
  glAttachShader(m_index, m_vertShader->getIndex());
  glAttachShader(m_index, m_fragShader->getIndex());

  updateShaders();
}

void ShaderProgram::loadShaders() const {
  m_vertShader->load();
  m_fragShader->load();
}

void ShaderProgram::compileShaders() const {
  m_vertShader->compile();
  m_fragShader->compile();
}

void ShaderProgram::link() const {
  glLinkProgram(m_index);

  GLint success;
  glGetProgramiv(m_index, GL_LINK_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog {};

    glGetProgramInfoLog(m_index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed.\n" << infoLog.data() << std::endl;
  }
}

void ShaderProgram::updateShaders() const {
  loadShaders();
  compileShaders();
  link();
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, int value) const {
  glUniform1i(uniformIndex, value);
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, unsigned int value) const {
  glUniform1ui(uniformIndex, value);
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, std::size_t value) const {
  glUniform1ui(uniformIndex, static_cast<GLuint>(value));
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, float value) const {
  glUniform1f(uniformIndex, value);
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, const Vec3f& vec) const {
  glUniform3fv(uniformIndex, 1, vec.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, const Vec4f& vec) const {
  glUniform4fv(uniformIndex, 1, vec.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, const Mat3f& mat) const {
  glUniformMatrix3fv(uniformIndex, 1, GL_FALSE, mat.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(GLint uniformIndex, const Mat4f& mat) const {
  glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, mat.getDataPtr());
}

} // namespace Raz
