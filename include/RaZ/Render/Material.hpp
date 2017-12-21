#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Material {
public:
  Material() = default;
  explicit Material(TexturePtr texture) : m_texture{ std::move(texture) } {}
  explicit Material(const std::string& fileName) { m_texture = std::make_shared<Texture>(fileName); }

  const TexturePtr getTexture() const { return m_texture; }

private:
  TexturePtr m_texture {};
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
