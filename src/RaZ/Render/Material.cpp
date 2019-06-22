#include "RaZ/Render/Material.hpp"

namespace Raz {

MaterialCookTorrancePtr Material::recoverMaterial(MaterialPreset preset, float roughnessFactor) {
  static const std::array<std::pair<Vec3f, float>, static_cast<std::size_t>(MaterialPreset::PRESET_COUNT)> materialPresetParams = {
      std::pair<Vec3f, float>(Vec3f(0.02f), 0.f), // CHARCOAL
      std::pair<Vec3f, float>(Vec3f(0.21f), 0.f), // GRASS
      std::pair<Vec3f, float>(Vec3f(0.36f), 0.f), // SAND
      std::pair<Vec3f, float>(Vec3f(0.56f), 0.f), // ICE
      std::pair<Vec3f, float>(Vec3f(0.81f), 0.f), // SNOW

      std::pair<Vec3f, float>(Vec3f({ 0.560f, 0.570f, 0.580f }), 1.f), // IRON
      std::pair<Vec3f, float>(Vec3f({ 0.972f, 0.960f, 0.915f }), 1.f), // SILVER
      std::pair<Vec3f, float>(Vec3f({ 0.913f, 0.921f, 0.925f }), 1.f), // ALUMINIUM
      std::pair<Vec3f, float>(Vec3f({ 1.000f, 0.766f, 0.336f }), 1.f), // GOLD
      std::pair<Vec3f, float>(Vec3f({ 0.955f, 0.637f, 0.538f }), 1.f), // COPPER
      std::pair<Vec3f, float>(Vec3f({ 0.550f, 0.556f, 0.554f }), 1.f), // CHROMIUM
      std::pair<Vec3f, float>(Vec3f({ 0.660f, 0.609f, 0.526f }), 1.f), // NICKEL
      std::pair<Vec3f, float>(Vec3f({ 0.542f, 0.497f, 0.449f }), 1.f), // TITANIUM
      std::pair<Vec3f, float>(Vec3f({ 0.662f, 0.655f, 0.634f }), 1.f), // COBALT
      std::pair<Vec3f, float>(Vec3f({ 0.672f, 0.637f, 0.585f }), 1.f)  // PLATINUM
  };

  const std::pair<Vec3f, float>& materialParams = materialPresetParams[static_cast<std::size_t>(preset)];
  return MaterialCookTorrance::create(materialParams.first, materialParams.second, roughnessFactor);
}

void MaterialBlinnPhong::initTextures(const ShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string diffuseMapLocation      = locationBase + "diffuseMap";
  static const std::string ambientMapLocation      = locationBase + "ambientMap";
  static const std::string specularMapLocation     = locationBase + "specularMap";
  static const std::string emissiveMapLocation     = locationBase + "emissiveMap";
  static const std::string transparencyMapLocation = locationBase + "transparencyMap";
  static const std::string bumpMapLocation         = locationBase + "bumpMap";

  program.sendUniform(diffuseMapLocation,      0);
  program.sendUniform(ambientMapLocation,      1);
  program.sendUniform(specularMapLocation,     2);
  program.sendUniform(emissiveMapLocation,     3);
  program.sendUniform(transparencyMapLocation, 4);
  program.sendUniform(bumpMapLocation,         5);
}

void MaterialBlinnPhong::bindAttributes(const ShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string diffuseLocation      = locationBase + "diffuse";
  static const std::string ambientLocation      = locationBase + "ambient";
  static const std::string specularLocation     = locationBase + "specular";
  static const std::string emissiveLocation     = locationBase + "emissive";
  static const std::string transparencyLocation = locationBase + "transparency";

  program.sendUniform(diffuseLocation,      m_baseColor);
  program.sendUniform(ambientLocation,      m_ambient);
  program.sendUniform(specularLocation,     m_specular);
  program.sendUniform(emissiveLocation,     m_emissive);
  program.sendUniform(transparencyLocation, m_transparency);

  Texture::activate(0);
  m_baseColorMap->bind();

  Texture::activate(1);
  m_ambientMap->bind();

  Texture::activate(2);
  m_specularMap->bind();

  Texture::activate(3);
  m_emissiveMap->bind();

  Texture::activate(4);
  m_transparencyMap->bind();

  Texture::activate(5);
  m_bumpMap->bind();
}

void MaterialCookTorrance::initTextures(const ShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string albedoMapLocation           = locationBase + "albedoMap";
  static const std::string normalMapLocation           = locationBase + "normalMap";
  static const std::string metallicMapLocation         = locationBase + "metallicMap";
  static const std::string roughnessMapLocation        = locationBase + "roughnessMap";
  static const std::string ambientOcclusionMapLocation = locationBase + "ambientOcclusionMap";

  program.sendUniform(albedoMapLocation,           0);
  program.sendUniform(normalMapLocation,           1);
  program.sendUniform(metallicMapLocation,         2);
  program.sendUniform(roughnessMapLocation,        3);
  program.sendUniform(ambientOcclusionMapLocation, 4);
}

void MaterialCookTorrance::bindAttributes(const ShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string baseColorLocation       = locationBase + "baseColor";
  static const std::string metallicFactorLocation  = locationBase + "metallicFactor";
  static const std::string roughnessFactorLocation = locationBase + "roughnessFactor";

  program.sendUniform(baseColorLocation,       m_baseColor);
  program.sendUniform(metallicFactorLocation,  m_metallicFactor);
  program.sendUniform(roughnessFactorLocation, m_roughnessFactor);

  Texture::activate(0);
  m_baseColorMap->bind();

  Texture::activate(1);
  m_normalMap->bind();

  Texture::activate(2);
  m_metallicMap->bind();

  Texture::activate(3);
  m_roughnessMap->bind();

  Texture::activate(4);
  m_ambientOcclusionMap->bind();
}

} // namespace Raz
