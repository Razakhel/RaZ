#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include <unordered_map>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

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
  Material(const Material&) = default;

  virtual MaterialType getType() const = 0;

  static MaterialCookTorrancePtr recoverMaterial(MaterialPreset preset, float roughnessFactor);
  virtual MaterialPtr clone() const = 0;
  virtual void initTextures(const ShaderProgram& program) const = 0;
  virtual void bindAttributes(const ShaderProgram& program) const = 0;

  virtual ~Material() = default;

protected:
  Material() = default;
};

class MaterialBlinnPhong : public Material {
public:
  MaterialBlinnPhong() = default;
  explicit MaterialBlinnPhong(TexturePtr diffuseMap) : m_diffuseMap{ std::move(diffuseMap) } {}
  explicit MaterialBlinnPhong(const std::string& fileName) : MaterialBlinnPhong(Texture::create(fileName)) {}

  MaterialType getType() const override { return MaterialType::BLINN_PHONG; }
  const Vec3f& getAmbient() const { return m_ambient; }
  const Vec3f& getDiffuse() const { return m_diffuse; }
  const Vec3f& getSpecular() const { return m_specular; }
  const Vec3f& getEmissive() const { return m_emissive; }
  float getTransparency() const { return m_transparency; }

  const TexturePtr& getAmbientMap() const { return m_ambientMap; }
  const TexturePtr& getDiffuseMap() const { return m_diffuseMap; }
  const TexturePtr& getSpecularMap() const { return m_specularMap; }
  const TexturePtr& getEmissiveMap() const { return m_emissiveMap; }
  const TexturePtr& getTransparencyMap() const { return m_transparencyMap; }
  const TexturePtr& getBumpMap() const { return m_bumpMap; }

  void setDiffuse(float red, float green, float blue) { setDiffuse(Vec3f({ red, green, blue })); }
  void setDiffuse(const Vec3f& val) { m_diffuse = val; }
  void setAmbient(float red, float green, float blue) { setAmbient(Vec3f({ red, green, blue })); }
  void setAmbient(const Vec3f& val) { m_ambient = val; }
  void setSpecular(float red, float green, float blue) { setSpecular(Vec3f({ red, green, blue })); }
  void setSpecular(const Vec3f& val) { m_specular = val; }
  void setEmissive(float red, float green, float blue) { setEmissive(Vec3f({ red, green, blue })); }
  void setEmissive(const Vec3f& val) { m_emissive = val; }
  void setTransparency(float transparency) { m_transparency = transparency; }

  void setAmbientMap(const TexturePtr& ambientMap) { m_ambientMap = ambientMap; }
  void setDiffuseMap(const TexturePtr& diffuseMap) { m_diffuseMap = diffuseMap; }
  void setSpecularMap(const TexturePtr& specularMap) { m_specularMap = specularMap; }
  void setEmissiveMap(const TexturePtr& emissiveMap) { m_emissiveMap = emissiveMap; }
  void setTransparencyMap(const TexturePtr& transparencyMap) { m_transparencyMap = transparencyMap; }
  void setBumpMap(const TexturePtr& bumpMap) { m_bumpMap = bumpMap; }

  template <typename... Args>
  static MaterialBlinnPhongPtr create(Args&&... args) { return std::make_unique<MaterialBlinnPhong>(std::forward<Args>(args)...); }

  void loadAmbientMap(const std::string& fileName) { m_ambientMap = Texture::create(fileName); }
  void loadDiffuseMap(const std::string& fileName) { m_diffuseMap = Texture::create(fileName); }
  void loadSpecularMap(const std::string& fileName) { m_specularMap = Texture::create(fileName); }
  void loadEmissiveMap(const std::string& fileName) { m_emissiveMap = Texture::create(fileName); }
  void loadTransparencyMap(const std::string& fileName) { m_transparencyMap = Texture::create(fileName); }
  void loadBumpMap(const std::string& fileName) { m_bumpMap = Texture::create(fileName); }

  MaterialPtr clone() const override { return MaterialBlinnPhong::create(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_ambient      = Vec3f(1.f);
  Vec3f m_diffuse      = Vec3f(1.f);
  Vec3f m_specular     = Vec3f(1.f);
  Vec3f m_emissive     = Vec3f(0.f);
  float m_transparency = 1.f;

  TexturePtr m_ambientMap      = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_diffuseMap      = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_specularMap     = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_emissiveMap     = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_transparencyMap = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_bumpMap         = Texture::recoverTexture(TexturePreset::WHITE);
};

class MaterialCookTorrance : public Material {
public:
  MaterialCookTorrance() = default;
  explicit MaterialCookTorrance(TexturePtr albedoMap) : m_albedoMap{ std::move(albedoMap) } {}
  explicit MaterialCookTorrance(const std::string& fileName) : MaterialCookTorrance(Texture::create(fileName)) {}
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor)
    : m_baseColor{ baseColor }, m_metallicFactor{ metallicFactor }, m_roughnessFactor{ roughnessFactor } {}

  MaterialType getType() const override { return MaterialType::COOK_TORRANCE; }
  const Vec3f& getBaseColor() const { return m_baseColor; }
  float getMetallicFactor() const { return m_metallicFactor; }
  float getRoughnessFactor() const { return m_roughnessFactor; }

  const TexturePtr& getAlbedoMap() const { return m_albedoMap; }
  const TexturePtr& getMetallicMap() const { return m_metallicMap; }
  const TexturePtr& getNormalMap() const { return m_normalMap; }
  const TexturePtr& getRoughnessMap() const { return m_roughnessMap; }
  const TexturePtr& getAmbientOcclusionMap() const { return m_ambientOcclusionMap; }

  void setBaseColor(float red, float green, float blue) { setBaseColor(Vec3f({ red, green, blue })); }
  void setBaseColor(const Vec3f& color) { m_baseColor = color; }
  void setMetallicFactor(float metallicFactor) { m_metallicFactor = metallicFactor; }
  void setRoughnessFactor(float roughnessFactor) { m_roughnessFactor = roughnessFactor; }

  void setAlbedoMap(const TexturePtr& albedoMap) { m_albedoMap = albedoMap; }
  void setNormalMap(const TexturePtr& normalMap) { m_normalMap = normalMap; }
  void setMetallicMap(const TexturePtr& metallicMap) { m_metallicMap = metallicMap; }
  void setRoughnessMap(const TexturePtr& roughnessMap) { m_roughnessMap = roughnessMap; }
  void setAmbientOcclusionMap(const TexturePtr& ambientOcclusionMap) { m_ambientOcclusionMap = ambientOcclusionMap; }

  template <typename... Args>
  static MaterialCookTorrancePtr create(Args&&... args) { return std::make_unique<MaterialCookTorrance>(std::forward<Args>(args)...); }

  void loadAlbedoMap(const std::string& fileName) { m_albedoMap = Texture::create(fileName); }
  void loadNormalMap(const std::string& fileName) { m_normalMap = Texture::create(fileName); }
  void loadMetallicMap(const std::string& fileName) { m_metallicMap = Texture::create(fileName); }
  void loadRoughnessMap(const std::string& fileName) { m_roughnessMap = Texture::create(fileName); }
  void loadAmbientOcclusionMap(const std::string& fileName) { m_ambientOcclusionMap = Texture::create(fileName); }

  MaterialPtr clone() const override { return MaterialCookTorrance::create(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_baseColor       = Vec3f(1.f);
  float m_metallicFactor  = 1.f;
  float m_roughnessFactor = 1.f;

  TexturePtr m_albedoMap           = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_normalMap           = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_metallicMap         = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_roughnessMap        = Texture::recoverTexture(TexturePreset::WHITE);
  TexturePtr m_ambientOcclusionMap = Texture::recoverTexture(TexturePreset::WHITE);
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
