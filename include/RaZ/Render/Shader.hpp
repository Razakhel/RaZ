#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include <string>
#include <initializer_list>

#include "GL/gl.h"

enum ShaderType { RAZ_SHADER_TYPE_VERTEX = GL_VERTEX_SHADER,
                  RAZ_SHADER_TYPE_FRAGMENT = GL_FRAGMENT_SHADER,
                  RAZ_SHADER_TYPE_COMPUTE = GL_COMPUTE_SHADER,
                  RAZ_SHADER_TYPE_GEOMETRY = GL_GEOMETRY_SHADER };

namespace Raz {

class Shader {
public:
  GLuint getIndex() const { return m_index; }
  const std::string& getContent() const { return m_content; }

  void read(const std::string& fileName);

  ~Shader() { glDeleteShader(m_index); }

protected:
  Shader(ShaderType type) : m_type{ type } {}

  GLuint m_index;
  std::string m_content;

  ShaderType m_type;
};

class VertexShader : public Shader {
public:
  VertexShader() : Shader(RAZ_SHADER_TYPE_VERTEX) {}
  VertexShader(const std::string& fileName) : VertexShader() { read(fileName); }
};

class FragmentShader : public Shader {
public:
  FragmentShader() : Shader(RAZ_SHADER_TYPE_FRAGMENT) {}
  FragmentShader(const std::string& fileName) : FragmentShader() { read(fileName); }
};

class ShaderProgram {
public:
  ShaderProgram() = default;
  ShaderProgram(std::initializer_list<Shader> shadersList);

  void use() const { glUseProgram(m_index); }

  GLuint getIndex() const { return m_index; }

private:
  GLuint m_index;
};

} // namespace Raz

#endif // RAZ_SHADER_HPP
