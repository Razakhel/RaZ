#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include <memory>
#include <string>

#include "glew/include/GL/glew.h"

namespace Raz {

class Shader;
using ShaderPtr = std::unique_ptr<Shader>;

class VertexShader;
using VertexShaderPtr = std::unique_ptr<VertexShader>;

class FragmentShader;
using FragmentShaderPtr = std::unique_ptr<FragmentShader>;

class GeometryShader;
using GeometryShaderPtr = std::unique_ptr<GeometryShader>;

class Shader {
public:
  GLuint getIndex() const { return m_index; }
  const std::string& getPath() const { return m_path; }

  void load() const;
  void compile() const;
  void destroy() const { glDeleteShader(m_index); }

  ~Shader() { destroy(); }

protected:
  GLuint m_index {};
  std::string m_path {};
};

class VertexShader : public Shader {
public:
  VertexShader() { m_index = glCreateShader(GL_VERTEX_SHADER); }
  explicit VertexShader(std::string fileName);

  template <typename... Args>
  static VertexShaderPtr create(Args&&... args) { return std::make_unique<VertexShader>(std::forward<Args>(args)...); }
  static VertexShaderPtr loadFromSource(const std::string& source);
};

class FragmentShader : public Shader {
public:
  FragmentShader() { m_index = glCreateShader(GL_FRAGMENT_SHADER); }
  explicit FragmentShader(std::string fileName);

  template <typename... Args>
  static FragmentShaderPtr create(Args&&... args) { return std::make_unique<FragmentShader>(std::forward<Args>(args)...); }
  static FragmentShaderPtr loadFromSource(const std::string& source);
};

class GeometryShader : public Shader {
public:
  GeometryShader() { m_index = glCreateShader(GL_GEOMETRY_SHADER); }
  explicit GeometryShader(std::string fileName);

  template <typename... Args>
  static GeometryShaderPtr create(Args&&... args) { return std::make_unique<GeometryShader>(std::forward<Args>(args)...); }
  static GeometryShaderPtr loadFromSource(const std::string& source);
};

} // namespace Raz

#endif // RAZ_SHADER_HPP
