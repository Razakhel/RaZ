#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

#include <unordered_map>
#include <variant>

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

class Material {
public:
  Material(Material&&) noexcept = default;

  virtual MaterialType getType() const = 0;
  bool hasAttribute(const std::string& uniformName) const noexcept;
  std::size_t getAttributeCount() const noexcept { return m_attributes.size(); }
  template <typename T> const T& getAttribute(const std::string& uniformName) const noexcept {
    assert("Error: The given attribute uniform name does not exist." && hasAttribute(uniformName));
    assert("Error: The fetched attribute is not of the asked type." && std::holds_alternative<T>(m_attributes.find(uniformName)->second));
    return std::get<T>(m_attributes.find(uniformName)->second);
  }
  std::size_t getTextureCount() const noexcept { return m_textures.size(); }
  const Texture& getTexture(std::size_t index) const noexcept { return *m_textures[index].first; }

  /// Sets an attribute to be sent to the shaders. If the uniform name already exists, replaces the attribute's value.
  /// \tparam T Type of the attribute to set. Must be a type handled by ShaderProgram::sendUniform().
  /// \param attribVal Attribute to set.
  /// \param uniformName Uniform name of the attribute to set.
  template <typename T> void setAttribute(T&& attribVal, std::string uniformName) { m_attributes[std::move(uniformName)] = std::forward<T>(attribVal); }

  void addTexture(TexturePtr texture, std::string uniformName);
  void loadTexture(const FilePath& filePath, int bindingIndex, std::string uniformName, bool flipVertically = true);

  virtual MaterialPtr clone() const = 0;
  void sendAttributes(const RenderShaderProgram& program) const;
  /// Removes an attribute given its uniform name.
  /// \param uniformName Uniform name of the attribute to remove.
  void removeAttribute(const std::string& uniformName);
  /// Removes all attributes in the material.
  void clearAttributes() { m_attributes.clear(); }
  void initTextures(const RenderShaderProgram& program) const;
  void bindTextures(const RenderShaderProgram& program) const;

  Material& operator=(Material&&) noexcept = default;

  virtual ~Material() = default;

protected:
  Material() = default;
  Material(const Material&) = default;

  using Attribute = std::variant<int, unsigned int, float, Vec2i, Vec3i, Vec4i, Vec2u, Vec3u, Vec4u, Vec2f, Vec3f, Vec4f, Mat2f, Mat3f, Mat4f>;
  std::unordered_map<std::string, Attribute> m_attributes {};

  std::vector<std::pair<TexturePtr, std::string>> m_textures {};
};

class MaterialBlinnPhong final : public Material {
public:
  MaterialBlinnPhong();
  explicit MaterialBlinnPhong(TexturePtr diffuseMap) : MaterialBlinnPhong() { setDiffuseMap(std::move(diffuseMap)); }
  explicit MaterialBlinnPhong(const Vec3f& diffuse,
                              const Vec3f& ambient  = Vec3f(1.f),
                              const Vec3f& specular = Vec3f(1.f),
                              const Vec3f& emissive = Vec3f(0.f),
                              float transparency    = 1.f);

  MaterialType getType() const override { return MaterialType::BLINN_PHONG; }
  const Vec3f& getDiffuse() const { return getAttribute<Vec3f>("uniMaterial.diffuse"); }
  const Vec3f& getEmissive() const { return getAttribute<Vec3f>("uniMaterial.emissive"); }
  const Vec3f& getAmbient() const { return getAttribute<Vec3f>("uniMaterial.ambient"); }
  const Vec3f& getSpecular() const { return getAttribute<Vec3f>("uniMaterial.specular"); }
  float getTransparency() const { return getAttribute<float>("uniMaterial.transparency"); }

  const TexturePtr& getDiffuseMap() const { return m_textures[0].first; }
  const TexturePtr& getEmissiveMap() const { return m_textures[1].first; }
  const TexturePtr& getAmbientMap() const { return m_textures[2].first; }
  const TexturePtr& getSpecularMap() const { return m_textures[3].first; }
  const TexturePtr& getTransparencyMap() const { return m_textures[4].first; }
  const TexturePtr& getBumpMap() const { return m_textures[5].first; }

  void setDiffuse(const Vec3f& color) { setAttribute(color, "uniMaterial.diffuse"); }
  void setEmissive(const Vec3f& emissive) { setAttribute(emissive, "uniMaterial.emissive"); }
  void setAmbient(const Vec3f& ambient) { setAttribute(ambient, "uniMaterial.ambient"); }
  void setSpecular(const Vec3f& specular) { setAttribute(specular, "uniMaterial.specular"); }
  void setTransparency(float transparency) { setAttribute(transparency, "uniMaterial.transparency"); }

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
};

class MaterialCookTorrance final : public Material {
public:
  MaterialCookTorrance();
  explicit MaterialCookTorrance(TexturePtr albedoMap) : MaterialCookTorrance() { setAlbedoMap(std::move(albedoMap)); }
  MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor);

  MaterialType getType() const override { return MaterialType::COOK_TORRANCE; }
  const Vec3f& getBaseColor() const { return getAttribute<Vec3f>("uniMaterial.baseColor"); }
  const Vec3f& getEmissive() const { return getAttribute<Vec3f>("uniMaterial.emissive"); }
  float getMetallicFactor() const { return getAttribute<float>("uniMaterial.metallicFactor"); }
  float getRoughnessFactor() const { return getAttribute<float>("uniMaterial.roughnessFactor"); }

  const TexturePtr& getAlbedoMap() const { return m_textures[0].first; }
  const TexturePtr& getEmissiveMap() const { return m_textures[1].first; }
  const TexturePtr& getNormalMap() const { return m_textures[2].first; }
  const TexturePtr& getMetallicMap() const { return m_textures[3].first; }
  const TexturePtr& getRoughnessMap() const { return m_textures[4].first; }
  const TexturePtr& getAmbientOcclusionMap() const { return m_textures[5].first; }

  void setBaseColor(const Vec3f& baseColor) { setAttribute(baseColor, "uniMaterial.baseColor"); }
  void setEmissive(const Vec3f& emissive) { setAttribute(emissive, "uniMaterial.emissive"); }
  void setMetallicFactor(float metallicFactor) { setAttribute(metallicFactor, "uniMaterial.metallicFactor"); }
  void setRoughnessFactor(float roughnessFactor) { setAttribute(roughnessFactor, "uniMaterial.roughnessFactor"); }

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
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
