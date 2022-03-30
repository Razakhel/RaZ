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
  const Vec3f& getEmissive() const { return m_emissive; }
  std::size_t getTextureCount() const noexcept { return m_textures.size(); }
  const Texture& getTexture(std::size_t index) const noexcept { return *m_textures[index].first; }

  void setBaseColor(float red, float green, float blue) { setBaseColor(Vec3f(red, green, blue)); }
  void setBaseColor(const Vec3f& color) { m_baseColor = color; }
  void setEmissive(float red, float green, float blue) { setEmissive(Vec3f(red, green, blue)); }
  void setEmissive(const Vec3f& val) { m_emissive = val; }

  void addTexture(TexturePtr texture, std::string uniformName);
  void loadTexture(const FilePath& filePath, int bindingIndex, std::string uniformName, bool flipVertically = true);

  static MaterialCookTorrancePtr recoverMaterial(MaterialPreset preset, float roughnessFactor);
  virtual MaterialPtr clone() const = 0;
  void initTextures(const RenderShaderProgram& program) const;
  void bindTextures(const RenderShaderProgram& program) const;
  virtual void bindAttributes(const RenderShaderProgram& program) const = 0;

  Material& operator=(Material&&) noexcept = default;

  virtual ~Material() = default;

protected:
  Material() = default;
  Material(const Material&) = default;

  Vec3f m_baseColor = Vec3f(1.f);
  Vec3f m_emissive  = Vec3f(0.f);

  std::vector<std::pair<TexturePtr, std::string>> m_textures {};
};

class MaterialBlinnPhong final : public Material {
public:
  MaterialBlinnPhong();
  explicit MaterialBlinnPhong(TexturePtr diffuseMap) : MaterialBlinnPhong() { setDiffuseMap(std::move(diffuseMap)); }
  explicit MaterialBlinnPhong(const Vec3f& baseColor,
                              const Vec3f& ambient  = Vec3f(1.f),
                              const Vec3f& specular = Vec3f(1.f),
                              const Vec3f& emissive = Vec3f(0.f),
                              float transparency    = 1.f);

  MaterialType getType() const override { return MaterialType::BLINN_PHONG; }
  const Vec3f& getAmbient() const { return m_ambient; }
  const Vec3f& getSpecular() const { return m_specular; }
  float getTransparency() const { return m_transparency; }

  const TexturePtr& getDiffuseMap() const { return m_textures[0].first; }
  const TexturePtr& getEmissiveMap() const { return m_textures[1].first; }
  const TexturePtr& getAmbientMap() const { return m_textures[2].first; }
  const TexturePtr& getSpecularMap() const { return m_textures[3].first; }
  const TexturePtr& getTransparencyMap() const { return m_textures[4].first; }
  const TexturePtr& getBumpMap() const { return m_textures[5].first; }

  void setDiffuse(float red, float green, float blue) { setBaseColor(red, green, blue); }
  void setDiffuse(const Vec3f& color) { setBaseColor(color); }
  void setAmbient(float red, float green, float blue) { setAmbient(Vec3f(red, green, blue)); }
  void setAmbient(const Vec3f& val) { m_ambient = val; }
  void setSpecular(float red, float green, float blue) { setSpecular(Vec3f(red, green, blue)); }
  void setSpecular(const Vec3f& val) { m_specular = val; }
  void setTransparency(float transparency) { m_transparency = transparency; }

  void setDiffuseMap(TexturePtr diffuseMap) { m_textures[0].first = std::move(diffuseMap); }
  void setEmissiveMap(TexturePtr emissiveMap) { m_textures[1].first = std::move(emissiveMap); }
  void setAmbientMap(TexturePtr ambientMap) { m_textures[2].first = std::move(ambientMap); }
  void setSpecularMap(TexturePtr specularMap) { m_textures[3].first = std::move(specularMap); }
  void setTransparencyMap(TexturePtr transparencyMap) { m_textures[4].first = std::move(transparencyMap); }
  void setBumpMap(TexturePtr bumpMap) { m_textures[5].first = std::move(bumpMap); }

  template <typename... Args>
  static MaterialBlinnPhongPtr create(Args&&... args) { return std::make_unique<MaterialBlinnPhong>(std::forward<Args>(args)...); }

  void loadDiffuseMap(const FilePath& filePath, bool flipVertically = true);
  void loadEmissiveMap(const FilePath& filePath, bool flipVertically = true);
  void loadAmbientMap(const FilePath& filePath, bool flipVertically = true);
  void loadSpecularMap(const FilePath& filePath, bool flipVertically = true);
  void loadTransparencyMap(const FilePath& filePath, bool flipVertically = true);
  void loadBumpMap(const FilePath& filePath, bool flipVertically = true);

  MaterialPtr clone() const override { return MaterialBlinnPhong::create(*this); }
  void bindAttributes(const RenderShaderProgram& program) const override;

private:
  Vec3f m_ambient      = Vec3f(1.f);
  Vec3f m_specular     = Vec3f(1.f);
  float m_transparency = 1.f;
};

class MaterialCookTorrance final : public Material {
public:
  MaterialCookTorrance();
  explicit MaterialCookTorrance(TexturePtr albedoMap) : MaterialCookTorrance() { setAlbedoMap(std::move(albedoMap)); }
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor);

  MaterialType getType() const override { return MaterialType::COOK_TORRANCE; }
  float getMetallicFactor() const { return m_metallicFactor; }
  float getRoughnessFactor() const { return m_roughnessFactor; }

  const TexturePtr& getAlbedoMap() const { return m_textures[0].first; }
  const TexturePtr& getEmissiveMap() const { return m_textures[1].first; }
  const TexturePtr& getNormalMap() const { return m_textures[2].first; }
  const TexturePtr& getMetallicMap() const { return m_textures[3].first; }
  const TexturePtr& getRoughnessMap() const { return m_textures[4].first; }
  const TexturePtr& getAmbientOcclusionMap() const { return m_textures[5].first; }

  void setMetallicFactor(float metallicFactor) { m_metallicFactor = metallicFactor; }
  void setRoughnessFactor(float roughnessFactor) { m_roughnessFactor = roughnessFactor; }

  void setAlbedoMap(TexturePtr albedoMap) { m_textures[0].first = std::move(albedoMap); }
  void setEmissiveMap(TexturePtr emissiveMap) { m_textures[1].first = std::move(emissiveMap); }
  void setNormalMap(TexturePtr normalMap) { m_textures[2].first = std::move(normalMap); }
  void setMetallicMap(TexturePtr metallicMap) { m_textures[3].first = std::move(metallicMap); }
  void setRoughnessMap(TexturePtr roughnessMap) { m_textures[4].first = std::move(roughnessMap); }
  void setAmbientOcclusionMap(TexturePtr ambientOcclusionMap) { m_textures[5].first = std::move(ambientOcclusionMap); }

  template <typename... Args>
  static MaterialCookTorrancePtr create(Args&&... args) { return std::make_unique<MaterialCookTorrance>(std::forward<Args>(args)...); }

  void loadAlbedoMap(const FilePath& filePath, bool flipVertically = true);
  void loadEmissiveMap(const FilePath& filePath, bool flipVertically = true);
  void loadNormalMap(const FilePath& filePath, bool flipVertically = true);
  void loadMetallicMap(const FilePath& filePath, bool flipVertically = true);
  void loadRoughnessMap(const FilePath& filePath, bool flipVertically = true);
  void loadAmbientOcclusionMap(const FilePath& filePath, bool flipVertically = true);

  MaterialPtr clone() const override { return MaterialCookTorrance::create(*this); }
  void bindAttributes(const RenderShaderProgram& program) const override;

private:
  float m_metallicFactor  = 1.f;
  float m_roughnessFactor = 1.f;
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
