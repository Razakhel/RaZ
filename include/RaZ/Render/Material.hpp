#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Material {
public:
  Material() = default;

  const TexturePtr getTexture() const { return m_texture; }
  TexturePtr getTexture() { return m_texture; }

  void setTexture(TexturePtr texture) { m_texture = std::move(texture); }

private:
  TexturePtr m_texture;
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
