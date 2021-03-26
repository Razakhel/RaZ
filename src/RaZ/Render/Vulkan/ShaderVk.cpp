#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <fstream>
#include <vector>

namespace Raz {

Shader::Shader(Shader&& shader) noexcept
  : m_shaderModule{ std::exchange(shader.m_shaderModule, nullptr) }, m_path{ std::move(shader.m_path) } {}

void Shader::import(FilePath filePath) {
  m_path = std::move(filePath);
  load();
}

void Shader::load() {
  if (m_path.getPath().empty()) // Shader imported directly from source, no path available
    return;

  std::ifstream shaderSource(m_path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + m_path + "'");

  const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(bytes.data(), static_cast<std::streamsize>(fileSize));

  loadSource(std::string(bytes.data(), bytes.size()));
}

void Shader::loadSource(const std::string& source) {
  destroy(); // Resetting the shader
  Renderer::createShaderModule(m_shaderModule, source.size(), source.data());
}

void Shader::destroy() {
  if (m_shaderModule == nullptr)
    return;

  Renderer::destroyShaderModule(m_shaderModule);
  m_shaderModule = nullptr;
}

Shader& Shader::operator=(Shader&& shader) noexcept {
  std::swap(m_shaderModule, shader.m_shaderModule);
  m_path = std::move(shader.m_path);

  return *this;
}

} // namespace Raz
