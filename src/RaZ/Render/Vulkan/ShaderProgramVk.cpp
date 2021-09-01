#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

#include <array>

namespace Raz {

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_vertShader{ std::move(program.m_vertShader) },
    m_geomShader{ std::move(program.m_geomShader) },
    m_fragShader{ std::move(program.m_fragShader) },
    m_uniformBuffers{ std::move(program.m_uniformBuffers) } {}

void ShaderProgram::setVertexShader(VertexShader&& vertShader) {
  m_vertShader = std::move(vertShader);
}

void ShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  m_geomShader = std::move(geomShader);
}

void ShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  m_fragShader = std::move(fragShader);
}

void ShaderProgram::setShaders(VertexShader&& vertShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));

  loadShaders();
}

void ShaderProgram::setShaders(VertexShader&& vertShader, GeometryShader&& geomShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setGeometryShader(std::move(geomShader));
  setFragmentShader(std::move(fragShader));

  loadShaders();
}

void ShaderProgram::loadShaders() {
  m_vertShader.load();
  if (m_geomShader)
    m_geomShader->load();
  m_fragShader.load();
}

void ShaderProgram::destroyVertexShader() {
  m_vertShader.destroy();
}

void ShaderProgram::destroyGeometryShader() {
  if (!m_geomShader)
    return;

  m_geomShader->destroy();
  m_geomShader.reset();
}

void ShaderProgram::destroyFragmentShader() {
  m_fragShader.destroy();
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& program) noexcept {
  m_vertShader     = std::move(program.m_vertShader);
  m_geomShader     = std::move(program.m_geomShader);
  m_fragShader     = std::move(program.m_fragShader);
  m_uniformBuffers = std::move(program.m_uniformBuffers);

  return *this;
}

ShaderProgram::~ShaderProgram() {

}

} // namespace Raz
