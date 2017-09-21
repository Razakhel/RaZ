#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

#include "RaZ/Mesh/Shader.hpp"

namespace Raz {

void Shader::read(const std::string& fileName) {
  std::ifstream shaderSource(fileName, std::ios::in | std::ios::binary | std::ios::ate);

  std::ifstream::pos_type fileSize = shaderSource.tellg();
  shaderSource.seekg(0, std::ios::beg);

  std::vector<char> bytes(fileSize);
  shaderSource.read(&bytes[0], fileSize);

  content = std::string(&bytes[0], fileSize);

  index = glCreateShader(type);
  glShaderSource(index, 1, reinterpret_cast<const GLchar* const*>(&content), nullptr);
  glCompileShader(index);

  GLint success;
  glGetShaderiv(index, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog;

    glGetShaderInfoLog(index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Vertex shader compilation failed.\n" << infoLog.data() << std::endl;
  }
}

ShaderProgram::ShaderProgram(std::initializer_list<Shader> shadersList) {
  index = glCreateProgram();

  for (auto shaderIndex : shadersList)
    glAttachShader(index, shaderIndex.getIndex());

  glLinkProgram(index);

  GLint success;
  glGetProgramiv(index, GL_LINK_STATUS, &success);

  if (!success) {
    std::array<GLchar, 512> infoLog;

    glGetProgramInfoLog(index, infoLog.size(), nullptr, infoLog.data());
    std::cerr << "Error: Shader program link failed.\n" << infoLog.data() << std::endl;
  }
}

} // namespace Raz
