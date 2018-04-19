#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glew/include/GL/glew.h"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

void Shader::load() {
  std::ifstream shaderSource(m_path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + m_path + "'");

  const auto fileSize = static_cast<std::size_t>(shaderSource.tellg());
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(&bytes[0], fileSize);

  std::string content = std::string(&bytes[0], fileSize);

  const char* data = content.c_str();
  const auto length = static_cast<GLint>(content.size());
  glShaderSource(m_index, 1, static_cast<const GLchar* const*>(&data), &length);
}

void Shader::compile() const {
  glCompileShader(m_index);

  GLint success;
  glGetShaderiv(m_index, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog {};

    glGetShaderInfoLog(m_index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Vertex shader compilation failed.\n" << infoLog.data() << std::endl;
  }
}

VertexShader::VertexShader(const std::string& fileName) {
  m_path = fileName;
  create();
  load();
}

FragmentShader::FragmentShader(const std::string& fileName) {
  m_path = fileName;
  create();
  load();
}

} // namespace Raz
