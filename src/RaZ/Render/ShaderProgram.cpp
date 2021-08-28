#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <array>

namespace Raz {

namespace {

inline void checkProgramUsed([[maybe_unused]] const ShaderProgram& program) {
#if defined(RAZ_CONFIG_DEBUG)
  if (!program.isUsed())
    Logger::error("The current shader program must be defined as used before sending uniforms to it.");
#endif
}

} // namespace

ShaderProgram::ShaderProgram()
  : m_index{ Renderer::createProgram() } {}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_index{ std::exchange(program.m_index, std::numeric_limits<unsigned int>::max()) },
    m_vertShader{ std::move(program.m_vertShader) },
    m_geomShader{ std::move(program.m_geomShader) },
    m_fragShader{ std::move(program.m_fragShader) },
    m_uniforms{ std::move(program.m_uniforms) } {}

void ShaderProgram::setVertexShader(VertexShader&& vertShader) {
  if (Renderer::isShaderAttached(m_index, m_vertShader.getIndex()))
    Renderer::detachShader(m_index, m_vertShader.getIndex());

  m_vertShader = std::move(vertShader);
  m_vertShader.compile();

  Renderer::attachShader(m_index, m_vertShader.getIndex());
}

void ShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  if (m_geomShader && Renderer::isShaderAttached(m_index, m_geomShader->getIndex()))
    Renderer::detachShader(m_index, m_geomShader->getIndex());

  m_geomShader = std::move(geomShader);
  m_geomShader->compile();

  Renderer::attachShader(m_index, m_geomShader->getIndex());
}

void ShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  if (Renderer::isShaderAttached(m_index, m_fragShader.getIndex()))
    Renderer::detachShader(m_index, m_fragShader.getIndex());

  m_fragShader = std::move(fragShader);
  m_fragShader.compile();

  Renderer::attachShader(m_index, m_fragShader.getIndex());
}

void ShaderProgram::setShaders(VertexShader&& vertShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));

  updateShaders();
}

void ShaderProgram::setShaders(VertexShader&& vertShader, GeometryShader&& geomShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setGeometryShader(std::move(geomShader));
  setFragmentShader(std::move(fragShader));

  updateShaders();
}

void ShaderProgram::loadShaders() const {
  m_vertShader.load();
  if (m_geomShader)
    m_geomShader->load();
  m_fragShader.load();
}

void ShaderProgram::compileShaders() const {
  m_vertShader.compile();
  if (m_geomShader)
    m_geomShader->compile();
  m_fragShader.compile();
}

void ShaderProgram::link() const {
  assert("Error: A shader program needs at least one shader for it to be linked." && (m_vertShader.isValid()
                                                                                   || (m_geomShader && m_geomShader->isValid())
                                                                                   || m_fragShader.isValid()));
  assert("Error: A shader program's vertex shader must be compiled before being linked." && (m_vertShader.isValid() ? m_vertShader.isCompiled() : true));
  assert("Error: A shader program's geometry shader must be compiled before being linked."
    && ((m_geomShader && m_geomShader->isValid()) ? m_geomShader->isCompiled() : true));
  assert("Error: A shader program's fragment shader must be compiled before being linked." && (m_fragShader.isValid() ? m_fragShader.isCompiled() : true));

  Renderer::linkProgram(m_index);
}

bool ShaderProgram::isLinked() const {
  return Renderer::isProgramLinked(m_index);
}

void ShaderProgram::use() const {
  Renderer::useProgram(m_index);
}

bool ShaderProgram::isUsed() const {
  return (Renderer::getCurrentProgram() == m_index);
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

void ShaderProgram::sendUniform(int uniformIndex, int value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, unsigned int value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, float value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec2f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector2(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec3f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector3(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec4f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector4(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat2f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix2x2(uniformIndex, mat.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat3f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix3x3(uniformIndex, mat.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat4f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix4x4(uniformIndex, mat.getDataPtr());
}

void ShaderProgram::destroyVertexShader() {
  Renderer::detachShader(m_index, m_vertShader.getIndex());
  m_vertShader.destroy();
}

void ShaderProgram::destroyGeometryShader() {
  if (!m_geomShader)
    return;

  Renderer::detachShader(m_index, m_geomShader->getIndex());
  m_geomShader->destroy();
  m_geomShader.reset();
}

void ShaderProgram::destroyFragmentShader() {
  Renderer::detachShader(m_index, m_fragShader.getIndex());
  m_fragShader.destroy();
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& program) noexcept {
  std::swap(m_index, program.m_index);
  m_vertShader = std::move(program.m_vertShader);
  m_geomShader = std::move(program.m_geomShader);
  m_fragShader = std::move(program.m_fragShader);
  m_uniforms   = std::move(program.m_uniforms);

  return *this;
}

ShaderProgram::~ShaderProgram() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteProgram(m_index);
}

} // namespace Raz
