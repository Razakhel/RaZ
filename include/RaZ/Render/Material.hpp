#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Material {
public:
  Material() = default;
  explicit Material(TexturePtr diffuseMap) : m_diffuseMap{ std::move(diffuseMap) } {}
  explicit Material(const std::string& fileName) { m_diffuseMap = std::make_shared<Texture>(fileName); }

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

private:
  Vec3f m_ambient {};
  Vec3f m_diffuse {};
  Vec3f m_specular {};
  Vec3f m_emissive {};
  float m_transparency {};

  TexturePtr m_ambientMap {};
  TexturePtr m_diffuseMap = std::make_shared<Texture>();
  TexturePtr m_specularMap {};
  TexturePtr m_transparencyMap {};
  TexturePtr m_bumpMap {};
};

using MaterialPtr = std::unique_ptr<Material>;

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
