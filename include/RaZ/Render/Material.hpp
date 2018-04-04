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

class Material {
public:
  static std::unique_ptr<Material> recoverMaterial(MaterialPreset preset, float roughnessFactor);
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
  const TexturePtr getTransparencyMap() const { return m_transparencyMap; }
  const TexturePtr getBumpMap() const { return m_bumpMap; }

  void assignAmbient(float red, float green, float blue) { m_ambient = Vec3f({ red, green, blue }); }
  void assignDiffuse(float red, float green, float blue) { m_diffuse = Vec3f({ red, green, blue }); }
  void assignSpecular(float red, float green, float blue) { m_specular = Vec3f({ red, green, blue }); }
  void assignEmissive(float red, float green, float blue) { m_emissive = Vec3f({ red, green, blue }); }
  void setTransparency(float transparency) { m_transparency = transparency; }

  void loadAmbientMap(const std::string& fileName) { m_ambientMap = std::make_shared<Texture>(fileName); }
  void loadDiffuseMap(const std::string& fileName) { m_diffuseMap = std::make_shared<Texture>(fileName); }
  void loadSpecularMap(const std::string& fileName) { m_specularMap = std::make_shared<Texture>(fileName); }
  void loadTransparencyMap(const std::string& fileName) { m_transparencyMap = std::make_shared<Texture>(fileName); }
  void loadBumpMap(const std::string& fileName) { m_bumpMap = std::make_shared<Texture>(fileName); }

  std::unique_ptr<Material> clone() const override { return std::make_unique<MaterialStandard>(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_ambient = Vec3f(1.f);
  Vec3f m_diffuse = Vec3f(1.f);
  Vec3f m_specular = Vec3f(1.f);
  Vec3f m_emissive = Vec3f(1.f);
  float m_transparency = 1.f;

  TexturePtr m_ambientMap = std::make_shared<Texture>(255);
  TexturePtr m_diffuseMap = std::make_shared<Texture>(255);
  TexturePtr m_specularMap = std::make_shared<Texture>(255);
  TexturePtr m_transparencyMap = std::make_shared<Texture>(255);
  TexturePtr m_bumpMap = std::make_shared<Texture>(255);
};

class MaterialCookTorrance : public Material {
public:
  MaterialCookTorrance() = default;
  explicit MaterialCookTorrance(TexturePtr albedoMap) : m_albedoMap{ std::move(albedoMap) } {}
  explicit MaterialCookTorrance(const std::string& fileName) { m_albedoMap = std::make_shared<Texture>(fileName); }
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor)
    : m_baseColor{ baseColor }, m_metallicFactor{ metallicFactor }, m_roughnessFactor{ roughnessFactor } {}

  const TexturePtr getAlbedoMap() const { return m_albedoMap; }
  const TexturePtr getMetallicMap() const { return m_metallicMap; }
  const TexturePtr getNormalMap() const { return m_normalMap; }
  const TexturePtr getRoughnessMap() const { return m_roughnessMap; }
  const TexturePtr getAmbientOcclusionMap() const { return m_ambientOcclusionMap; }

  void loadAlbedoMap(const std::string& fileName) { m_albedoMap = std::make_shared<Texture>(fileName); }
  void loadNormalMap(const std::string& fileName) { m_normalMap = std::make_shared<Texture>(fileName); }
  void loadMetallicMap(const std::string& fileName) { m_metallicMap = std::make_shared<Texture>(fileName); }
  void loadRoughnessMap(const std::string& fileName) { m_roughnessMap = std::make_shared<Texture>(fileName); }
  void loadAmbientOcclusionMap(const std::string& fileName) { m_ambientOcclusionMap = std::make_shared<Texture>(fileName); }

  std::unique_ptr<Material> clone() const override { return std::make_unique<MaterialCookTorrance>(*this); }
  void initTextures(const ShaderProgram& program) const override;
  void bindAttributes(const ShaderProgram& program) const override;

private:
  Vec3f m_baseColor = Vec3f(1.f);
  float m_metallicFactor = 1.f;
  float m_roughnessFactor = 1.f;

  TexturePtr m_albedoMap = std::make_shared<Texture>(255);
  TexturePtr m_normalMap = std::make_shared<Texture>(255);
  TexturePtr m_metallicMap = std::make_shared<Texture>(255);
  TexturePtr m_roughnessMap = std::make_shared<Texture>(255);
  TexturePtr m_ambientOcclusionMap = std::make_shared<Texture>(255);
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
