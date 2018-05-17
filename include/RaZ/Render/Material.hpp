#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include <unordered_map>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

enum class MaterialPreset {
  CHARCOAL, GRASS, SAND, ICE, SNOW,                                                   // Dielectric presets
  IRON, SILVER, ALUMINIUM, GOLD, COPPER, CHROMIUM, NICKEL, TITANIUM, COBALT, PLATINUM // Metallic presets
};

class MaterialCookTorrance;

class Material {
public:
  static std::unique_ptr<MaterialCookTorrance> recoverMaterial(MaterialPreset preset, float roughnessFactor);
  virtual std::unique_ptr<Material> clone() const = 0;
  virtual void initTextures(const ShaderProgram& program) const = 0;
  virtual void bindAttributes(const ShaderProgram& program) const = 0;
};

using MaterialPtr = std::unique_ptr<Material>;

class MaterialStandard : public Material {
public:
  MaterialStandard() = default;
  explicit MaterialStandard(TexturePtr diffuseMap) : m_diffuseMap{ std::move(diffuseMap) } {}
  explicit MaterialStandard(const std::string& fileName) { m_diffuseMap = std::make_shared<Texture>(fileName); }

  const TexturePtr getAmbientMap() const { return m_ambientMap; }
  const TexturePtr getDiffuseMap() const { return m_diffuseMap; }
  const TexturePtr getSpecularMap() const { return m_specularMap; }
  const TexturePtr getEmissiveMap() const { return m_emissiveMap; }
  const TexturePtr getTransparencyMap() const { return m_transparencyMap; }
  const TexturePtr getBumpMap() const { return m_bumpMap; }

  void setDiffuse(float red, float green, float blue) { setDiffuse(Vec3f({ red, green, blue })); }
  void setDiffuse(const Vec3f& val) { m_diffuse = val; }
  void setAmbient(float red, float green, float blue) { setAmbient(Vec3f({ red, green, blue })); }
  void setAmbient(const Vec3f& val) { m_ambient = val; }
  void setSpecular(float red, float green, float blue) { setSpecular(Vec3f({ red, green, blue })); }
  void setSpecular(const Vec3f& val) { m_specular = val; }
  void setEmissive(float red, float green, float blue) { setEmissive(Vec3f({ red, green, blue })); }
  void setEmissive(const Vec3f& val) { m_emissive = val; }
  void setTransparency(float transparency) { m_transparency = transparency; }

  void setAmbientMap(TexturePtr ambientMap) { m_ambientMap = std::move(ambientMap); }
  void setDiffuseMap(TexturePtr diffuseMap) { m_diffuseMap = std::move(diffuseMap); }
  void setSpecularMap(TexturePtr specularMap) { m_specularMap = std::move(specularMap); }
  void setEmissiveMap(TexturePtr emissiveMap) { m_emissiveMap = std::move(emissiveMap); }
  void setTransparencyMap(TexturePtr transparencyMap) { m_transparencyMap = std::move(transparencyMap); }
  void setBumpMap(TexturePtr bumpMap) { m_bumpMap = std::move(bumpMap); }

  void loadAmbientMap(const std::string& fileName) { m_ambientMap = std::make_shared<Texture>(fileName); }
  void loadDiffuseMap(const std::string& fileName) { m_diffuseMap = std::make_shared<Texture>(fileName); }
  void loadSpecularMap(const std::string& fileName) { m_specularMap = std::make_shared<Texture>(fileName); }
  void loadEmissiveMap(const std::string& fileName) { m_emissiveMap = std::make_shared<Texture>(fileName); }
  void loadTransparencyMap(const std::string& fileName) { m_transparencyMap = std::make_shared<Texture>(fileName); }
  void loadBumpMap(const std::string& fileName) { m_bumpMap = std::make_shared<Texture>(fileName); }

  std::unique_ptr<Material> clone() const override { return std::make_unique<MaterialStandard>(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_ambient      = Vec3f(1.f);
  Vec3f m_diffuse      = Vec3f(1.f);
  Vec3f m_specular     = Vec3f(1.f);
  Vec3f m_emissive     = Vec3f(1.f);
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
  explicit MaterialCookTorrance(const std::string& fileName) { m_albedoMap = std::make_shared<Texture>(fileName); }
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor)
    : m_baseColor{ baseColor }, m_metallicFactor{ metallicFactor }, m_roughnessFactor{ roughnessFactor } {}

  const Vec3f& getBaseColor() const { return m_baseColor; }
  float getMetallicFactor() const { return m_metallicFactor; }
  float getRoughnessFactor() const { return m_roughnessFactor; }

  const TexturePtr getAlbedoMap() const { return m_albedoMap; }
  const TexturePtr getMetallicMap() const { return m_metallicMap; }
  const TexturePtr getNormalMap() const { return m_normalMap; }
  const TexturePtr getRoughnessMap() const { return m_roughnessMap; }
  const TexturePtr getAmbientOcclusionMap() const { return m_ambientOcclusionMap; }

  void setBaseColor(float red, float green, float blue) { setBaseColor(Vec3f({ red, green, blue })); }
  void setBaseColor(const Vec3f& color) { m_baseColor = color; }
  void setMetallicFactor(float metallicFactor) { m_metallicFactor = metallicFactor; }
  void setRoughnessFactor(float roughnessFactor) { m_roughnessFactor = roughnessFactor; }

  void setAlbedoMap(TexturePtr albedoMap) { m_albedoMap = std::move(albedoMap); }
  void setNormalMap(TexturePtr normalMap) { m_normalMap = std::move(normalMap); }
  void setMetallicMap(TexturePtr metallicMap) { m_metallicMap = std::move(metallicMap); }
  void setRoughnessMap(TexturePtr roughnessMap) { m_roughnessMap = std::move(roughnessMap); }
  void setAmbientOcclusionMap(TexturePtr ambientOcclusionMap) { m_ambientOcclusionMap = std::move(ambientOcclusionMap); }

  void loadAlbedoMap(const std::string& fileName) { m_albedoMap = std::make_shared<Texture>(fileName); }
  void loadNormalMap(const std::string& fileName) { m_normalMap = std::make_shared<Texture>(fileName); }
  void loadMetallicMap(const std::string& fileName) { m_metallicMap = std::make_shared<Texture>(fileName); }
  void loadRoughnessMap(const std::string& fileName) { m_roughnessMap = std::make_shared<Texture>(fileName); }
  void loadAmbientOcclusionMap(const std::string& fileName) { m_ambientOcclusionMap = std::make_shared<Texture>(fileName); }

  std::unique_ptr<Material> clone() const override { return std::make_unique<MaterialCookTorrance>(*this); }
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
