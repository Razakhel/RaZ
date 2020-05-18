#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

#include <array>

namespace Raz {

ShaderProgram::ShaderProgram()
  : m_index{ Renderer::createProgram() } {}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_index{ std::exchange(program.m_index, std::numeric_limits<unsigned int>::max()) },
    m_vertShader{ std::move(program.m_vertShader) },
    m_geomShader{ std::move(program.m_geomShader) },
    m_fragShader{ std::move(program.m_fragShader) },
    m_uniforms{ std::move(program.m_uniforms) } {}

void ShaderProgram::setVertexShader(VertexShader&& vertShader) {
  m_vertShader = std::move(vertShader);
  Renderer::attachShader(m_index, m_vertShader.getIndex());
}

void ShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  m_geomShader = std::move(geomShader);
  Renderer::attachShader(m_index, m_geomShader->getIndex());
}

void ShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  m_fragShader = std::move(fragShader);
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
