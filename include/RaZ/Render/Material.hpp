#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

#include <unordered_map>

namespace Raz {

class Material;
using MaterialPtr = std::unique_ptr<Material>;

class MaterialBlinnPhong;
using MaterialBlinnPhongPtr = std::unique_ptr<MaterialBlinnPhong>;

class MaterialCookTorrance;
using MaterialCookTorrancePtr = std::unique_ptr<MaterialCookTorrance>;

enum class MaterialType {
  BLINN_PHONG = 0,
  COOK_TORRANCE
};

enum class MaterialPreset : uint16_t {
  CHARCOAL = 0, GRASS, SAND, ICE, SNOW,                                                // Dielectric presets
  IRON, SILVER, ALUMINIUM, GOLD, COPPER, CHROMIUM, NICKEL, TITANIUM, COBALT, PLATINUM, // Metallic presets

  PRESET_COUNT
};

class Material {
public:
  Material(Material&&) noexcept = default;

  virtual MaterialType getType() const = 0;
  const Vec3f& getBaseColor() const { return m_baseColor; }

  const TexturePtr& getBaseColorMap() const { return m_baseColorMap; }

  void setBaseColor(float red, float green, float blue) { setBaseColor(Vec3f(red, green, blue)); }
  void setBaseColor(const Vec3f& color) { m_baseColor = color; }

  void setBaseColorMap(TexturePtr baseColorMap) { m_baseColorMap = std::move(baseColorMap); }

  void loadBaseColorMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);

  static MaterialCookTorrancePtr recoverMaterial(MaterialPreset preset, float roughnessFactor);
  virtual MaterialPtr clone() const = 0;
  virtual void initTextures(const ShaderProgram& program) const = 0;
  virtual void bindAttributes(const ShaderProgram& program) const = 0;

  virtual ~Material() = default;

protected:
  Material() = default;
  Material(const Material&) noexcept = default;
  explicit Material(TexturePtr baseColorMap) : m_baseColorMap{ std::move(baseColorMap) } {}
  explicit Material(const FilePath& filePath, bool flipVertically = true) : Material(Texture::create(filePath, flipVertically)) {}

  Vec3f m_baseColor = Vec3f(1.f);

  TexturePtr m_baseColorMap = Texture::create(ColorPreset::WHITE, 0);
};

class MaterialBlinnPhong final : public Material {
public:
  MaterialBlinnPhong() = default;
  explicit MaterialBlinnPhong(TexturePtr diffuseMap) : Material(std::move(diffuseMap)) {}
  explicit MaterialBlinnPhong(const FilePath& filePath, int bindingIndex, bool flipVertically = true)
    : MaterialBlinnPhong(Texture::create(filePath, bindingIndex, flipVertically)) {}

  MaterialType getType() const override { return MaterialType::BLINN_PHONG; }
  const Vec3f& getAmbient() const { return m_ambient; }
  const Vec3f& getSpecular() const { return m_specular; }
  const Vec3f& getEmissive() const { return m_emissive; }
  float getTransparency() const { return m_transparency; }

  const TexturePtr& getDiffuseMap() const { return getBaseColorMap(); }
  const TexturePtr& getAmbientMap() const { return m_ambientMap; }
  const TexturePtr& getSpecularMap() const { return m_specularMap; }
  const TexturePtr& getEmissiveMap() const { return m_emissiveMap; }
  const TexturePtr& getTransparencyMap() const { return m_transparencyMap; }
  const TexturePtr& getBumpMap() const { return m_bumpMap; }

  void setDiffuse(float red, float green, float blue) { setBaseColor(red, green, blue); }
  void setDiffuse(const Vec3f& color) { setBaseColor(color); }
  void setAmbient(float red, float green, float blue) { setAmbient(Vec3f(red, green, blue)); }
  void setAmbient(const Vec3f& val) { m_ambient = val; }
  void setSpecular(float red, float green, float blue) { setSpecular(Vec3f(red, green, blue)); }
  void setSpecular(const Vec3f& val) { m_specular = val; }
  void setEmissive(float red, float green, float blue) { setEmissive(Vec3f(red, green, blue)); }
  void setEmissive(const Vec3f& val) { m_emissive = val; }
  void setTransparency(float transparency) { m_transparency = transparency; }

  void setDiffuseMap(TexturePtr diffuseMap) { setBaseColorMap(std::move(diffuseMap)); }
  void setAmbientMap(TexturePtr ambientMap) { m_ambientMap = std::move(ambientMap); }
  void setSpecularMap(TexturePtr specularMap) { m_specularMap = std::move(specularMap); }
  void setEmissiveMap(TexturePtr emissiveMap) { m_emissiveMap = std::move(emissiveMap); }
  void setTransparencyMap(TexturePtr transparencyMap) { m_transparencyMap = std::move(transparencyMap); }
  void setBumpMap(TexturePtr bumpMap) { m_bumpMap = std::move(bumpMap); }

  template <typename... Args>
  static MaterialBlinnPhongPtr create(Args&&... args) { return std::make_unique<MaterialBlinnPhong>(std::forward<Args>(args)...); }

  void loadDiffuseMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadAmbientMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadSpecularMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadEmissiveMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadTransparencyMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadBumpMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);

  MaterialPtr clone() const override { return MaterialBlinnPhong::create(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_ambient      = Vec3f(1.f);
  Vec3f m_specular     = Vec3f(1.f);
  Vec3f m_emissive     = Vec3f(0.f);
  float m_transparency = 1.f;

  TexturePtr m_ambientMap      = Texture::create(ColorPreset::WHITE, 1);
  TexturePtr m_specularMap     = Texture::create(ColorPreset::WHITE, 2);
  TexturePtr m_emissiveMap     = Texture::create(ColorPreset::WHITE, 3);
  TexturePtr m_transparencyMap = Texture::create(ColorPreset::WHITE, 4);
  TexturePtr m_bumpMap         = Texture::create(ColorPreset::WHITE, 5);
};

class MaterialCookTorrance final : public Material {
public:
  MaterialCookTorrance() = default;
  explicit MaterialCookTorrance(TexturePtr albedoMap) : Material(std::move(albedoMap)) {}
  explicit MaterialCookTorrance(const FilePath& filePath, int bindingIndex, bool flipVertically = true)
    : MaterialCookTorrance(Texture::create(filePath, bindingIndex, flipVertically)) {}
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor)
    : m_metallicFactor{ metallicFactor }, m_roughnessFactor{ roughnessFactor } { setBaseColor(baseColor); }

  MaterialType getType() const override { return MaterialType::COOK_TORRANCE; }
  float getMetallicFactor() const { return m_metallicFactor; }
  float getRoughnessFactor() const { return m_roughnessFactor; }

  const TexturePtr& getAlbedoMap() const { return getBaseColorMap(); }
  const TexturePtr& getMetallicMap() const { return m_metallicMap; }
  const TexturePtr& getNormalMap() const { return m_normalMap; }
  const TexturePtr& getRoughnessMap() const { return m_roughnessMap; }
  const TexturePtr& getAmbientOcclusionMap() const { return m_ambientOcclusionMap; }

  void setMetallicFactor(float metallicFactor) { m_metallicFactor = metallicFactor; }
  void setRoughnessFactor(float roughnessFactor) { m_roughnessFactor = roughnessFactor; }

  void setAlbedoMap(TexturePtr albedoMap) { setBaseColorMap(std::move(albedoMap)); }
  void setNormalMap(TexturePtr normalMap) { m_normalMap = std::move(normalMap); }
  void setMetallicMap(TexturePtr metallicMap) { m_metallicMap = std::move(metallicMap); }
  void setRoughnessMap(TexturePtr roughnessMap) { m_roughnessMap = std::move(roughnessMap); }
  void setAmbientOcclusionMap(TexturePtr ambientOcclusionMap) { m_ambientOcclusionMap = std::move(ambientOcclusionMap); }

  template <typename... Args>
  static MaterialCookTorrancePtr create(Args&&... args) { return std::make_unique<MaterialCookTorrance>(std::forward<Args>(args)...); }

  void loadAlbedoMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadNormalMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadMetallicMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadRoughnessMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);
  void loadAmbientOcclusionMap(const FilePath& filePath, int bindingIndex, bool flipVertically = true);

  MaterialPtr clone() const override { return MaterialCookTorrance::create(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  float m_metallicFactor  = 1.f;
  float m_roughnessFactor = 1.f;

  TexturePtr m_normalMap           = Texture::create(ColorPreset::MEDIUM_BLUE, 1); // Representing a [ 0; 0; 1 ] vector
  TexturePtr m_metallicMap         = Texture::create(ColorPreset::RED, 2);
  TexturePtr m_roughnessMap        = Texture::create(ColorPreset::RED, 3);
  TexturePtr m_ambientOcclusionMap = Texture::create(ColorPreset::RED, 4);
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
