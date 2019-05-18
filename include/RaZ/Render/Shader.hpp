#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include <memory>
#include <string>

namespace Raz {

class Shader {
public:
  Shader(const Shader&) = delete;
  Shader(Shader&& shader) noexcept;

  unsigned int getIndex() const { return m_index; }
  const std::string& getPath() const { return m_path; }

  void import(std::string fileName);
  void load() const;
  void compile() const;
  bool isCompiled() const;
  void destroy() const;

  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&& shader) noexcept;

  ~Shader() { destroy(); }

protected:
  Shader() = default;

  void loadSource(const std::string& source);

  unsigned int m_index {};
  std::string m_path {};
};

class VertexShader : public Shader {
public:
  VertexShader();
  explicit VertexShader(std::string fileName) : VertexShader() { import(std::move(fileName)); }

  static VertexShader loadFromSource(const std::string& source);
};

class FragmentShader : public Shader {
public:
  FragmentShader();
  explicit FragmentShader(std::string fileName) : FragmentShader() { import(std::move(fileName)); }

  static FragmentShader loadFromSource(const std::string& source);
};

class GeometryShader : public Shader {
public:
  GeometryShader();
  explicit GeometryShader(std::string fileName) : GeometryShader() { import(std::move(fileName)); }

  static GeometryShader loadFromSource(const std::string& source);
};

} // namespace Raz

#endif // RAZ_SHADER_HPP
