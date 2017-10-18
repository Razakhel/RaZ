#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Material {
public:
  Material() = default;
  Material(const VertexShader& vertShader, const FragmentShader& fragShader) : m_shaderProgram(vertShader, fragShader) {}

  const ShaderProgram& getShaderProgram() const { return m_shaderProgram; }
  ShaderProgram& getShaderProgram() { return m_shaderProgram; }
  const Texture& getTexture() const { return m_texture; }
  Texture& getTexture() { return m_texture; }

private:
  ShaderProgram m_shaderProgram;
  Texture m_texture;
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
