#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include "RaZ/Utils/FilePath.hpp"

#include <memory>
#include <string>

namespace Raz {

class Shader {
public:
  Shader(const Shader&) = delete;
  Shader(Shader&& shader) noexcept;

  unsigned int getIndex() const { return m_index; }
  const FilePath& getPath() const { return m_path; }

  bool isValid() const;
  void import(FilePath filePath);
  void load() const;
  void compile() const;
  bool isCompiled() const;
  void destroy();

  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&& shader) noexcept;

  ~Shader() { destroy(); }

protected:
  Shader() = default;

  void loadSource(const std::string& source) const;

  unsigned int m_index {};
  FilePath m_path {};
};

class VertexShader final : public Shader {
public:
  VertexShader();
  explicit VertexShader(FilePath filePath) : VertexShader() { import(std::move(filePath)); }

  static VertexShader loadFromSource(const std::string& source);
  static VertexShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

class TessellationControlShader final : public Shader {
public:
  TessellationControlShader();
  explicit TessellationControlShader(FilePath filePath) : TessellationControlShader() { import(std::move(filePath)); }

  static TessellationControlShader loadFromSource(const std::string& source);
  static TessellationControlShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

class TessellationEvaluationShader final : public Shader {
public:
  TessellationEvaluationShader();
  explicit TessellationEvaluationShader(FilePath filePath) : TessellationEvaluationShader() { import(std::move(filePath)); }

  static TessellationEvaluationShader loadFromSource(const std::string& source);
  static TessellationEvaluationShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

class GeometryShader final : public Shader {
public:
  GeometryShader();
  explicit GeometryShader(FilePath filePath) : GeometryShader() { import(std::move(filePath)); }

  static GeometryShader loadFromSource(const std::string& source);
  static GeometryShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

class FragmentShader final : public Shader {
public:
  FragmentShader();
  explicit FragmentShader(FilePath filePath) : FragmentShader() { import(std::move(filePath)); }

  static FragmentShader loadFromSource(const std::string& source);
  static FragmentShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

class ComputeShader final : public Shader {
public:
  ComputeShader();
  explicit ComputeShader(FilePath filePath) : ComputeShader() { import(std::move(filePath)); }

  static ComputeShader loadFromSource(const std::string& source);
  static ComputeShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }
};

} // namespace Raz

#endif // RAZ_SHADER_HPP
