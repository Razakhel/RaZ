#include <array>
#include <iostream>

#include "GL/glew.h"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

ShaderProgram::ShaderProgram()
  : m_index{ Renderer::createProgram() } {}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_index{ std::exchange(program.m_index, std::numeric_limits<unsigned int>::max()) },
    m_vertShader{ std::move(program.m_vertShader) },
    m_fragShader{ std::move(program.m_fragShader) },
    m_geomShader{ std::move(program.m_geomShader) },
    m_uniforms{ std::move(program.m_uniforms) } {}

void ShaderProgram::setVertexShader(VertexShader&& vertShader) {
  m_vertShader = std::move(vertShader);
  Renderer::attachShader(m_index, m_vertShader.getIndex());
}

void ShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  m_fragShader = std::move(fragShader);
  Renderer::attachShader(m_index, m_fragShader.getIndex());
}

void ShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  m_geomShader = std::make_unique<GeometryShader>(std::move(geomShader));
  Renderer::attachShader(m_index, m_geomShader->getIndex());
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
  Renderer::linkProgram(m_index);
}

bool ShaderProgram::isLinked() const {
  return Renderer::isProgramLinked(m_index);
}

void ShaderProgram::use() const {
  Renderer::useProgram(m_index);
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

  return Renderer::recoverUniformLocation(m_index, uniformName.c_str());
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, int8_t value) const {
  glUniform1i(uniformIndex, static_cast<int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, int16_t value) const {
  glUniform1i(uniformIndex, static_cast<int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, int32_t value) const {
  glUniform1i(uniformIndex, static_cast<int>(value));
}

template <>
void ShaderProgram::sendUniform(int uniformIndex, int64_t value) const {
  glUniform1i(uniformIndex, static_cast<int>(value));
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
void ShaderProgram::sendUniform(int uniformIndex, const Vec2f& vec) const {
  glUniform2fv(uniformIndex, 1, vec.getDataPtr());
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
void ShaderProgram::sendUniform(int uniformIndex, const Mat2f& mat) const {
  glUniformMatrix2fv(uniformIndex, 1, GL_FALSE, mat.getDataPtr());
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
  Renderer::detachShader(m_index, m_vertShader.getIndex());
  m_vertShader.destroy();
}

void ShaderProgram::destroyFragmentShader() {
  Renderer::detachShader(m_index, m_fragShader.getIndex());
  m_fragShader.destroy();
}

void ShaderProgram::destroyGeometryShader() {
  if (!m_geomShader)
    return;

  Renderer::detachShader(m_index, m_geomShader->getIndex());
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
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteProgram(m_index);
}

} // namespace Raz
