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

  void load();
  void compile() const;

  ~Shader() { glDeleteShader(m_index); }

protected:

  virtual void create() = 0;

  GLuint m_index {};
  std::string m_path {};
};

class VertexShader : public Shader {
public:
  explicit VertexShader(const std::string& fileName);

private:
  void create() override { m_index = glCreateShader(GL_VERTEX_SHADER); }
};

using VertexShaderPtr = std::unique_ptr<VertexShader>;

class FragmentShader : public Shader {
public:
  explicit FragmentShader(const std::string& fileName);

private:
  void create() override { m_index = glCreateShader(GL_FRAGMENT_SHADER); }
};

using FragmentShaderPtr = std::unique_ptr<FragmentShader>;

} // namespace Raz

#endif // RAZ_SHADER_HPP
