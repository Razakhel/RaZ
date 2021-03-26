#pragma once

#ifndef RAZ_SHADERVK_HPP
#define RAZ_SHADERVK_HPP

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/FilePath.hpp"

namespace Raz {

class Shader {
public:
  Shader() = default;
  explicit Shader(FilePath filePath) { import(std::move(filePath)); }
  Shader(const Shader&) = delete;
  Shader(Shader&& shader) noexcept;

  const FilePath& getPath() const { return m_path; }

  void import(FilePath filePath);
  void load();
  void destroy();

  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&& shader) noexcept;

  virtual ~Shader() { destroy(); }

protected:
  void loadSource(const std::string& source);

  VkShaderModule m_shaderModule {};
  FilePath m_path {};
};

using VertexShader   = Shader;
using GeometryShader = Shader;
using FragmentShader = Shader;

} // namespace Raz

#endif // RAZ_SHADERVK_HPP
