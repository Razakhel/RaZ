#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glew/include/GL/glew.h"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

void Shader::load() const {
  std::ifstream shaderSource(m_path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + m_path + "'");

  const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(&bytes[0], static_cast<std::streamsize>(fileSize));

  std::string content = std::string(&bytes[0], fileSize);

  const char* data  = content.c_str();
  const auto length = static_cast<int>(content.size());
  glShaderSource(m_index, 1, &data, &length);
}

void Shader::compile() const {
  glCompileShader(m_index);

  int success;
  glGetShaderiv(m_index, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::array<char, 512> infoLog {};

    glGetShaderInfoLog(m_index, static_cast<int>(infoLog.size()), nullptr, infoLog.data());
    std::cerr << "Error: Vertex shader compilation failed.\n" << infoLog.data() << std::endl;
  }
}

VertexShader::VertexShader(std::string fileName) : VertexShader() {
  m_path = std::move(fileName);
  load();
}

VertexShaderPtr VertexShader::loadFromSource(const std::string& source) {
  auto vertShader = std::make_unique<VertexShader>();

  const char* data  = source.c_str();
  const auto length = static_cast<int>(source.size());

  glShaderSource(vertShader->getIndex(), 1, &data, &length);

  vertShader->compile();

  return vertShader;
}

FragmentShader::FragmentShader(std::string fileName) : FragmentShader() {
  m_path = std::move(fileName);
  load();
}

FragmentShaderPtr FragmentShader::loadFromSource(const std::string& source) {
  auto fragShader = std::make_unique<FragmentShader>();

  const char* data  = source.c_str();
  const auto length = static_cast<int>(source.size());

  glShaderSource(fragShader->getIndex(), 1, &data, &length);

  fragShader->compile();

  return fragShader;
}

GeometryShader::GeometryShader(std::string fileName) : GeometryShader() {
  m_path = std::move(fileName);
  load();
}

GeometryShaderPtr GeometryShader::loadFromSource(const std::string& source) {
  auto fragShader = std::make_unique<GeometryShader>();

  const char* data  = source.c_str();
  const auto length = static_cast<int>(source.size());

  glShaderSource(fragShader->getIndex(), 1, &data, &length);

  fragShader->compile();

  return fragShader;
}

} // namespace Raz
