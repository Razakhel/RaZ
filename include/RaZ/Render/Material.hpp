#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

enum class MaterialType {
  COOK_TORRANCE,     ///< Cook-Torrance (PBR) material.
  BLINN_PHONG,       ///< Blinn-Phong material.
  SINGLE_TEXTURE_2D, ///< Single 2D texture material; only displays the given texture as-is.
  SINGLE_TEXTURE_3D  ///< Single 3D texture material; only displays the given texture as-is.
};

/// Predefined material attributes names.
namespace MaterialAttribute {

// Common attributes
static constexpr const char* BaseColor = "uniMaterial.baseColor"; ///< Base color factor.
static constexpr const char* Emissive  = "uniMaterial.emissive";  ///< Emissive factor.

// PBR attributes
static constexpr const char* Metallic  = "uniMaterial.metallicFactor";  ///< Metallic factor.
static constexpr const char* Roughness = "uniMaterial.roughnessFactor"; ///< Roughness factor.

// Legacy attributes
static constexpr const char* Ambient      = "uniMaterial.ambient";      ///< Ambient factor.
static constexpr const char* Specular     = "uniMaterial.specular";     ///< Specular factor.
static constexpr const char* Transparency = "uniMaterial.transparency"; ///< Transparency factor.

}

/// Predefined material textures names.
namespace MaterialTexture {

// Common textures
static constexpr const char* BaseColor = "uniMaterial.baseColorMap"; ///< Base color (albedo or diffuse) map.
static constexpr const char* Emissive  = "uniMaterial.emissiveMap";  ///< Emissive map.
static constexpr const char* Ambient   = "uniMaterial.ambientMap";   ///< Ambient occlusion map (Cook-Torrance) or ambient map (legacy).

// PBR textures
static constexpr const char* Normal    = "uniMaterial.normalMap";    ///< Normal map.
static constexpr const char* Metallic  = "uniMaterial.metallicMap";  ///< Metalness map.
static constexpr const char* Roughness = "uniMaterial.roughnessMap"; ///< Roughness map.

// Legacy textures
static constexpr const char* Specular     = "uniMaterial.specularMap";     ///< Specular map
static constexpr const char* Transparency = "uniMaterial.transparencyMap"; ///< Transparency map.
static constexpr const char* Bump         = "uniMaterial.bumpMap";         ///< Bump map.

}

class Material {
public:
  Material() = default;
  /// Creates a material with a predefined type.
  /// \param type Material type to apply.
  explicit Material(MaterialType type) { loadType(type); }
  Material(Material&&) noexcept = default;

  const RenderShaderProgram& getProgram() const noexcept { return m_program; }
  RenderShaderProgram& getProgram() noexcept { return m_program; }
  /// Checks if the material has any attribute or texture.
  /// \return True if the material has no attribute and texture, false otherwise.
  bool isEmpty() const noexcept { return (m_program.getAttributeCount() == 0 && m_program.getTextureCount() == 0); }

  Material clone() const { return *this; }
  /// Loads a predefined material type, setting default shaders & adding all needed attributes & textures if they do not exist yet.
  /// \param type Material type to apply.
  void loadType(MaterialType type);

  Material& operator=(const Material&) = delete;
  Material& operator=(Material&&) noexcept = default;

private:
  Material(const Material& material) : m_program{ material.m_program.clone() } {}

  RenderShaderProgram m_program {};
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
