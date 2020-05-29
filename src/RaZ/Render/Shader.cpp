#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"

#include <fstream>
#include <limits>
#include <sstream>
#include <vector>

namespace Raz {

Shader::Shader(Shader&& shader) noexcept
  : m_index{ std::exchange(shader.m_index, std::numeric_limits<unsigned int>::max()) }, m_path{ std::move(shader.m_path) } {}

void Shader::import(std::string filePath) {
  m_path = std::move(filePath);
  load();
}

void Shader::load() const {
  if (m_path.empty()) // Shader imported directly from source, no path available
    return;

  std::ifstream shaderSource(m_path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + m_path + "'");

  const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(bytes.data(), static_cast<std::streamsize>(fileSize));

  Renderer::sendShaderSource(m_index, bytes.data(), static_cast<int>(fileSize));
}

void Shader::compile() const {
  Renderer::compileShader(m_index);
}

bool Shader::isCompiled() const {
  return Renderer::isShaderCompiled(m_index);
}

void Shader::loadSource(const std::string& source) const {
  Renderer::sendShaderSource(m_index, source);
}

void Shader::destroy() const {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteShader(m_index);
}

Shader& Shader::operator=(Shader&& shader) noexcept {
  std::swap(m_index, shader.m_index);
  m_path = std::move(shader.m_path);

  return *this;
}

VertexShader::VertexShader() {
  m_index = Renderer::createShader(ShaderType::VERTEX);
}

VertexShader VertexShader::loadFromSource(const std::string& source) {
  VertexShader vertShader;
  vertShader.loadSource(source);
  return vertShader;
}

FragmentShader::FragmentShader() {
  m_index = Renderer::createShader(ShaderType::FRAGMENT);
}

FragmentShader FragmentShader::loadFromSource(const std::string& source) {
  FragmentShader fragShader;
  fragShader.loadSource(source);
  return fragShader;
}

GeometryShader::GeometryShader() {
  m_index = Renderer::createShader(ShaderType::GEOMETRY);
}

GeometryShader GeometryShader::loadFromSource(const std::string& source) {
  GeometryShader geomShader;
  geomShader.loadSource(source);
  return geomShader;
}

} // namespace Raz
