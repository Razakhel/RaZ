#include <array>
#include <iostream>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

void ShaderProgram::setVertexShader(VertexShaderPtr vertShader) {
  glAttachShader(m_index, vertShader->getIndex());
  m_vertShader = std::move(vertShader);
}

void ShaderProgram::setFragmentShader(FragmentShaderPtr fragShader) {
  glAttachShader(m_index, fragShader->getIndex());
  m_fragShader = std::move(fragShader);
}

void ShaderProgram::setGeometryShader(GeometryShaderPtr geomShader) {
  glAttachShader(m_index, geomShader->getIndex());
  m_geomShader = std::move(geomShader);
}

void ShaderProgram::setShaders(VertexShaderPtr vertShader, FragmentShaderPtr fragShader, GeometryShaderPtr geomShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));
  if (geomShader)
    setGeometryShader(std::move(geomShader));

  updateShaders();
}

void ShaderProgram::loadShaders() const {
  m_vertShader->load();
  m_fragShader->load();
  if (m_geomShader)
    m_fragShader->load();
}

void ShaderProgram::compileShaders() const {
  m_vertShader->compile();
  m_fragShader->compile();
  if (m_geomShader)
    m_geomShader->compile();
}

void ShaderProgram::link() const {
  glLinkProgram(m_index);

  int success;
  glGetProgramiv(m_index, GL_LINK_STATUS, &success);

  if (!success) {
    std::array<char, 512> infoLog {};

    glGetProgramInfoLog(m_index, static_cast<int>(infoLog.size()), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed.\n" << infoLog.data() << std::endl;
  }
}

void ShaderProgram::updateShaders() const {
  loadShaders();
  compileShaders();
  link();
  use();
}

void ShaderProgram::createUniform(const std::string& uniformName) {
  m_uniforms.emplace(uniformName, recoverUniformLocation(uniformName));
}

int ShaderProgram::recoverUniformLocation(const std::string& uniformName) const {
  const auto uniform = m_uniforms.find(uniformName);

  if (uniform != m_uniforms.cend())
    return uniform->second;

  return glGetUniformLocation(m_index, uniformName.c_str());
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, int value) const {
  glUniform1i(uniformIndex, value);
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, uint8_t value) const {
  glUniform1ui(uniformIndex, static_cast<unsigned int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, uint16_t value) const {
  glUniform1ui(uniformIndex, static_cast<unsigned int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, uint32_t value) const {
  glUniform1ui(uniformIndex, static_cast<unsigned int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, uint64_t value) const {
  glUniform1ui(uniformIndex, static_cast<unsigned int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, float value) const {
  glUniform1f(uniformIndex, value);
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, const Vec3f& vec) const {
  glUniform3fv(uniformIndex, 1, vec.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, const Vec4f& vec) const {
  glUniform4fv(uniformIndex, 1, vec.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, const Mat3f& mat) const {
  glUniformMatrix3fv(uniformIndex, 1, GL_FALSE, mat.getDataPtr());
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, const Mat4f& mat) const {
  glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, mat.getDataPtr());
}

} // namespace Raz
