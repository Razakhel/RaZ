#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fstream>
#include <limits>
#include <sstream>
#include <vector>

namespace Raz {

Shader::Shader(Shader&& shader) noexcept
  : m_index{ std::exchange(shader.m_index, std::numeric_limits<unsigned int>::max()) }, m_path{ std::move(shader.m_path) } {}

bool Shader::isValid() const {
  return (m_index != std::numeric_limits<unsigned int>::max());
}

void Shader::import(FilePath filePath) {
  m_path = std::move(filePath);
  load();
}

void Shader::load() const {
  if (m_path.getPath().empty()) // Shader imported directly from source, no path available
    return;

  Logger::debug("[Shader] Loading (ID: " + std::to_string(m_index) + ", path: '" + m_path + "')...");

  std::ifstream shaderSource(m_path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + m_path + "'");

  const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(bytes.data(), static_cast<std::streamsize>(fileSize));

  Renderer::sendShaderSource(m_index, bytes.data(), static_cast<int>(fileSize));

  Logger::debug("[Shader] Loaded");
}

void Shader::compile() const {
  Logger::debug("[Shader] Compiling (ID: " + std::to_string(m_index) + ")...");
  Renderer::compileShader(m_index);
  Logger::debug("[Shader] Compiled");
}

bool Shader::isCompiled() const {
  return Renderer::isShaderCompiled(m_index);
}

void Shader::loadSource(const std::string& source) const {
  Logger::debug("[Shader] Loading source (ID: " + std::to_string(m_index) + ")...");
  Renderer::sendShaderSource(m_index, source);
  Logger::debug("[Shader] Loaded source");
}

void Shader::destroy() {
  if (!isValid())
    return;

  Logger::debug("[Shader] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteShader(m_index);
  m_index = std::numeric_limits<unsigned int>::max();
  Logger::debug("[Shader] Destroyed");
}

Shader& Shader::operator=(Shader&& shader) noexcept {
  std::swap(m_index, shader.m_index);
  m_path = std::move(shader.m_path);

  return *this;
}

VertexShader::VertexShader() {
  Logger::debug("[Shader] Creating vertex shader...");
  m_index = Renderer::createShader(ShaderType::VERTEX);
  Logger::debug("[Shader] Created vertex shader (ID: " + std::to_string(m_index) + ')');
}

VertexShader VertexShader::loadFromSource(const std::string& source) {
  VertexShader vertShader;
  vertShader.loadSource(source);
  return vertShader;
}

FragmentShader::FragmentShader() {
  Logger::debug("[Shader] Creating fragment shader...");
  m_index = Renderer::createShader(ShaderType::FRAGMENT);
  Logger::debug("[Shader] Created fragment shader (ID: " + std::to_string(m_index) + ')');
}

FragmentShader FragmentShader::loadFromSource(const std::string& source) {
  FragmentShader fragShader;
  fragShader.loadSource(source);
  return fragShader;
}

GeometryShader::GeometryShader() {
  Logger::debug("[Shader] Creating geometry shader...");
  m_index = Renderer::createShader(ShaderType::GEOMETRY);
  Logger::debug("[Shader] Created geometry shader (ID: " + std::to_string(m_index) + ')');
}

GeometryShader GeometryShader::loadFromSource(const std::string& source) {
  GeometryShader geomShader;
  geomShader.loadSource(source);
  return geomShader;
}

ComputeShader::ComputeShader() {
  Logger::debug("[Shader] Creating compute shader...");
  m_index = Renderer::createShader(ShaderType::COMPUTE);
  Logger::debug("[Shader] Created compute shader (ID: " + std::to_string(m_index) + ')');
}

ComputeShader ComputeShader::loadFromSource(const std::string& source) {
  ComputeShader compShader;
  compShader.loadSource(source);
  return compShader;
}

} // namespace Raz
