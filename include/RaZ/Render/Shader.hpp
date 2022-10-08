#pragma once

#ifndef RAZ_SHADER_HPP
#define RAZ_SHADER_HPP

#include "RaZ/Data/OwnerValue.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <limits>
#include <string>

namespace Raz {

class Shader {
public:
  Shader(const Shader&) = delete;
  Shader(Shader&&) noexcept = default;

  unsigned int getIndex() const noexcept { return m_index; }
  const FilePath& getPath() const noexcept { return m_path; }

  bool isValid() const noexcept { return m_index.isValid(); }
  void import(FilePath filePath);
  /// Reloads the shader file. The shader must have been previously imported from a file for this function to load anything.
  /// \see import()
  void load() const;
  void compile() const;
  bool isCompiled() const noexcept;
  void destroy();

  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&&) noexcept = default;

  ~Shader() { destroy(); }

protected:
  Shader() = default;

  void loadSource(const std::string& source) const;

  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
  FilePath m_path {};
};

class VertexShader final : public Shader {
public:
  VertexShader();
  explicit VertexShader(FilePath filePath) : VertexShader() { import(std::move(filePath)); }

  static VertexShader loadFromSource(const std::string& source);
  static VertexShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  VertexShader clone() const;
};

class TessellationControlShader final : public Shader {
public:
  TessellationControlShader();
  explicit TessellationControlShader(FilePath filePath) : TessellationControlShader() { import(std::move(filePath)); }

  static TessellationControlShader loadFromSource(const std::string& source);
  static TessellationControlShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  TessellationControlShader clone() const;
};

class TessellationEvaluationShader final : public Shader {
public:
  TessellationEvaluationShader();
  explicit TessellationEvaluationShader(FilePath filePath) : TessellationEvaluationShader() { import(std::move(filePath)); }

  static TessellationEvaluationShader loadFromSource(const std::string& source);
  static TessellationEvaluationShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  TessellationEvaluationShader clone() const;
};

class GeometryShader final : public Shader {
public:
  GeometryShader();
  explicit GeometryShader(FilePath filePath) : GeometryShader() { import(std::move(filePath)); }

  static GeometryShader loadFromSource(const std::string& source);
  static GeometryShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  GeometryShader clone() const;
};

class FragmentShader final : public Shader {
public:
  FragmentShader();
  explicit FragmentShader(FilePath filePath) : FragmentShader() { import(std::move(filePath)); }

  static FragmentShader loadFromSource(const std::string& source);
  static FragmentShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  FragmentShader clone() const;
};

class ComputeShader final : public Shader {
public:
  ComputeShader();
  explicit ComputeShader(FilePath filePath) : ComputeShader() { import(std::move(filePath)); }

  static ComputeShader loadFromSource(const std::string& source);
  static ComputeShader loadFromSource(std::string_view source) { return loadFromSource(std::string(source)); }

  ComputeShader clone() const;
};

} // namespace Raz

#endif // RAZ_SHADER_HPP
