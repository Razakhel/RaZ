#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

Shader::Shader(Shader&& shader) noexcept
  : m_index{ std::exchange(shader.m_index, GL_INVALID_INDEX) }, m_path{ std::move(shader.m_path) } {}

void Shader::import(std::string fileName) {
  m_path = std::move(fileName);
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
  shaderSource.read(&bytes[0], static_cast<std::streamsize>(fileSize));

  const std::string content(&bytes[0], fileSize);

  const char* data  = content.c_str();
  const auto length = static_cast<int>(content.size());
  glShaderSource(m_index, 1, &data, &length);
}

void Shader::compile() const {
  glCompileShader(m_index);

  if (!isCompiled()) {
    std::array<char, 512> infoLog {};

    glGetShaderInfoLog(m_index, static_cast<int>(infoLog.size()), nullptr, infoLog.data());
    std::cerr << "Error: Shader compilation failed (ID " << m_index << ", path '" << m_path << "').\n" << infoLog.data() << std::endl;
  }
}

bool Shader::isCompiled() const {
  int success;
  glGetShaderiv(m_index, GL_COMPILE_STATUS, &success);

  return static_cast<bool>(success);
}

void Shader::loadSource(const std::string& source) {
  const char* data  = source.c_str();
  const auto length = static_cast<int>(source.size());

  glShaderSource(m_index, 1, &data, &length);
}

void Shader::destroy() const {
  if (m_index == GL_INVALID_INDEX)
    return;

  glDeleteShader(m_index);
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
