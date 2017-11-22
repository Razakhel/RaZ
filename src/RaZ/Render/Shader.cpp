#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

void Shader::read(const std::string& fileName) {
  std::ifstream shaderSource(fileName, std::ios::in | std::ios::binary | std::ios::ate);

  if (!shaderSource)
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");

  const std::ifstream::pos_type fileSize = shaderSource.tellg();
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(&bytes[0], fileSize);

  m_content = std::string(&bytes[0], fileSize);

  m_index = glCreateShader(m_type);
  const char* data = m_content.c_str();
  const GLint length = m_content.size();
  glShaderSource(m_index, 1, static_cast<const GLchar* const*>(&data), &length);
  glCompileShader(m_index);

  GLint success;
  glGetShaderiv(m_index, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog {};

    glGetShaderInfoLog(m_index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Vertex shader compilation failed.\n" << infoLog.data() << std::endl;
    std::cerr << m_content << std::endl;
  }
}

void ShaderProgram::attachShaders(const VertexShader& vertShader, const FragmentShader& fragShader) {
  glAttachShader(m_index, vertShader.getIndex());
  glAttachShader(m_index, fragShader.getIndex());

  glLinkProgram(m_index);

  GLint success;
  glGetProgramiv(m_index, GL_LINK_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog {};

    glGetProgramInfoLog(m_index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed.\n" << infoLog.data() << std::endl;
  }
}

} // namespace Raz
