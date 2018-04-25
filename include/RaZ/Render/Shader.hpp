#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include <memory>
#include <string>
#include <initializer_list>

#include "glew/include/GL/glew.h"

namespace Raz {

class Shader {
public:
  GLuint getIndex() const { return m_index; }

  void load() const;
  void compile() const;

  ~Shader() { glDeleteShader(m_index); }

protected:
  GLuint m_index {};
  std::string m_path {};
};

class VertexShader : public Shader {
public:
  VertexShader() { m_index = glCreateShader(GL_VERTEX_SHADER); }
  explicit VertexShader(const std::string& fileName);

  static std::unique_ptr<VertexShader> loadFromSource(const std::string& source);
};

using VertexShaderPtr = std::unique_ptr<VertexShader>;

class FragmentShader : public Shader {
public:
  FragmentShader() { m_index = glCreateShader(GL_FRAGMENT_SHADER); }
  explicit FragmentShader(const std::string& fileName);

  static std::unique_ptr<FragmentShader> loadFromSource(const std::string& source);
};

using FragmentShaderPtr = std::unique_ptr<FragmentShader>;

} // namespace Raz

#endif // RAZ_SHADER_HPP
