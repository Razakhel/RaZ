#include <array>
#include <iostream>

#include "GL/glew.h"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

ShaderProgram::ShaderProgram()
  : m_index{ glCreateProgram() } {}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_index{ std::exchange(program.m_index, GL_INVALID_INDEX) },
    m_vertShader{ std::move(program.m_vertShader) },
    m_fragShader{ std::move(program.m_fragShader) },
    m_geomShader{ std::move(program.m_geomShader) },
    m_uniforms{ std::move(program.m_uniforms) } {}

void ShaderProgram::setVertexShader(VertexShader&& vertShader) {
  m_vertShader = std::move(vertShader);
  glAttachShader(m_index, m_vertShader.getIndex());
}

void ShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  m_fragShader = std::move(fragShader);
  glAttachShader(m_index, m_fragShader.getIndex());
}

void ShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  m_geomShader = std::make_unique<GeometryShader>(std::move(geomShader));
  glAttachShader(m_index, m_geomShader->getIndex());
}

void ShaderProgram::setShaders(VertexShader&& vertShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));

  updateShaders();
}

void ShaderProgram::setShaders(VertexShader&& vertShader, FragmentShader&& fragShader, GeometryShader&& geomShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));
  setGeometryShader(std::move(geomShader));

  updateShaders();
}

void ShaderProgram::loadShaders() const {
  m_vertShader.load();
  m_fragShader.load();
  if (m_geomShader)
    m_geomShader->load();
}

void ShaderProgram::compileShaders() const {
  m_vertShader.compile();
  m_fragShader.compile();
  if (m_geomShader)
    m_geomShader->compile();
}

void ShaderProgram::link() const {
  glLinkProgram(m_index);

  if (!isLinked()) {
    std::array<char, 512> infoLog {};

    glGetProgramInfoLog(m_index, static_cast<int>(infoLog.size()), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed (ID " << m_index << ").\n" << infoLog.data() << std::endl;
  }
}

bool ShaderProgram::isLinked() const {
  int success;
  glGetProgramiv(m_index, GL_LINK_STATUS, &success);
  
  return static_cast<bool>(success);
}

void ShaderProgram::use() const {
  glUseProgram(m_index);
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

void ShaderProgram::destroyVertexShader() {
  glDetachShader(m_index, m_vertShader.getIndex());
  m_vertShader.destroy();
}

void ShaderProgram::destroyFragmentShader() {
  glDetachShader(m_index, m_fragShader.getIndex());
  m_fragShader.destroy();
}

void ShaderProgram::destroyGeometryShader() {
  if (!m_geomShader)
    return;

  glDetachShader(m_index, m_geomShader->getIndex());
  m_geomShader->destroy();
  m_geomShader.reset();
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& program) noexcept {
  std::swap(m_index, program.m_index);
  m_vertShader = std::move(program.m_vertShader);
  m_fragShader = std::move(program.m_fragShader);
  m_geomShader = std::move(program.m_geomShader);
  m_uniforms   = std::move(program.m_uniforms);

  return *this;
}

ShaderProgram::~ShaderProgram() {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteProgram(m_index);
}

} // namespace Raz
