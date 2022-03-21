#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

void Material::loadBaseColorMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_baseColorMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

MaterialCookTorrancePtr Material::recoverMaterial(MaterialPreset preset, float roughnessFactor) {
  static constexpr std::array<std::pair<Vec3f, float>, static_cast<std::size_t>(MaterialPreset::PRESET_COUNT)> materialPresetParams = {
      std::pair<Vec3f, float>(Vec3f(0.02f), 0.f), // CHARCOAL
      std::pair<Vec3f, float>(Vec3f(0.21f), 0.f), // GRASS
      std::pair<Vec3f, float>(Vec3f(0.36f), 0.f), // SAND
      std::pair<Vec3f, float>(Vec3f(0.56f), 0.f), // ICE
      std::pair<Vec3f, float>(Vec3f(0.81f), 0.f), // SNOW

      std::pair<Vec3f, float>(Vec3f(0.560f, 0.570f, 0.580f), 1.f), // IRON
      std::pair<Vec3f, float>(Vec3f(0.972f, 0.960f, 0.915f), 1.f), // SILVER
      std::pair<Vec3f, float>(Vec3f(0.913f, 0.921f, 0.925f), 1.f), // ALUMINIUM
      std::pair<Vec3f, float>(Vec3f(1.000f, 0.766f, 0.336f), 1.f), // GOLD
      std::pair<Vec3f, float>(Vec3f(0.955f, 0.637f, 0.538f), 1.f), // COPPER
      std::pair<Vec3f, float>(Vec3f(0.550f, 0.556f, 0.554f), 1.f), // CHROMIUM
      std::pair<Vec3f, float>(Vec3f(0.660f, 0.609f, 0.526f), 1.f), // NICKEL
      std::pair<Vec3f, float>(Vec3f(0.542f, 0.497f, 0.449f), 1.f), // TITANIUM
      std::pair<Vec3f, float>(Vec3f(0.662f, 0.655f, 0.634f), 1.f), // COBALT
      std::pair<Vec3f, float>(Vec3f(0.672f, 0.637f, 0.585f), 1.f)  // PLATINUM
  };

  const std::pair<Vec3f, float>& materialParams = materialPresetParams[static_cast<std::size_t>(preset)];
  return MaterialCookTorrance::create(materialParams.first, materialParams.second, roughnessFactor);
}

void MaterialBlinnPhong::loadDiffuseMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  loadBaseColorMap(filePath, bindingIndex, flipVertically);
}

void MaterialBlinnPhong::loadAmbientMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_ambientMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialBlinnPhong::loadSpecularMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_specularMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialBlinnPhong::loadEmissiveMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_emissiveMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialBlinnPhong::loadTransparencyMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_transparencyMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialBlinnPhong::loadBumpMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_bumpMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialBlinnPhong::initTextures(const RenderShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string diffuseMapLocation      = locationBase + "diffuseMap";
  static const std::string ambientMapLocation      = locationBase + "ambientMap";
  static const std::string specularMapLocation     = locationBase + "specularMap";
  static const std::string emissiveMapLocation     = locationBase + "emissiveMap";
  static const std::string transparencyMapLocation = locationBase + "transparencyMap";
  static const std::string bumpMapLocation         = locationBase + "bumpMap";

  program.use();
  program.sendUniform(diffuseMapLocation,      m_baseColorMap->getBindingIndex());
  program.sendUniform(ambientMapLocation,      m_ambientMap->getBindingIndex());
  program.sendUniform(specularMapLocation,     m_specularMap->getBindingIndex());
  program.sendUniform(emissiveMapLocation,     m_emissiveMap->getBindingIndex());
  program.sendUniform(transparencyMapLocation, m_transparencyMap->getBindingIndex());
  program.sendUniform(bumpMapLocation,         m_bumpMap->getBindingIndex());
}

void MaterialBlinnPhong::bindAttributes(const RenderShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string diffuseLocation      = locationBase + "diffuse";
  static const std::string ambientLocation      = locationBase + "ambient";
  static const std::string specularLocation     = locationBase + "specular";
  static const std::string emissiveLocation     = locationBase + "emissive";
  static const std::string transparencyLocation = locationBase + "transparency";

  program.use();
  program.sendUniform(diffuseLocation,      m_baseColor);
  program.sendUniform(ambientLocation,      m_ambient);
  program.sendUniform(specularLocation,     m_specular);
  program.sendUniform(emissiveLocation,     m_emissive);
  program.sendUniform(transparencyLocation, m_transparency);

  m_baseColorMap->activate();
  m_baseColorMap->bind();

  m_ambientMap->activate();
  m_ambientMap->bind();

  m_specularMap->activate();
  m_specularMap->bind();

  m_emissiveMap->activate();
  m_emissiveMap->bind();

  m_transparencyMap->activate();
  m_transparencyMap->bind();

  m_bumpMap->activate();
  m_bumpMap->bind();
}

void MaterialCookTorrance::loadAlbedoMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  loadBaseColorMap(filePath, bindingIndex, flipVertically);
}

void MaterialCookTorrance::loadNormalMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_normalMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialCookTorrance::loadMetallicMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_metallicMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialCookTorrance::loadRoughnessMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_roughnessMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialCookTorrance::loadAmbientOcclusionMap(const FilePath& filePath, int bindingIndex, bool flipVertically) {
  m_ambientOcclusionMap = Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex);
}

void MaterialCookTorrance::initTextures(const RenderShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string albedoMapLocation           = locationBase + "albedoMap";
  static const std::string normalMapLocation           = locationBase + "normalMap";
  static const std::string metallicMapLocation         = locationBase + "metallicMap";
  static const std::string roughnessMapLocation        = locationBase + "roughnessMap";
  static const std::string ambientOcclusionMapLocation = locationBase + "ambientOcclusionMap";

  program.use();
  program.sendUniform(albedoMapLocation,           m_baseColorMap->getBindingIndex());
  program.sendUniform(normalMapLocation,           m_normalMap->getBindingIndex());
  program.sendUniform(metallicMapLocation,         m_metallicMap->getBindingIndex());
  program.sendUniform(roughnessMapLocation,        m_roughnessMap->getBindingIndex());
  program.sendUniform(ambientOcclusionMapLocation, m_ambientOcclusionMap->getBindingIndex());
}

void MaterialCookTorrance::bindAttributes(const RenderShaderProgram& program) const {
  static const std::string locationBase = "uniMaterial.";

  static const std::string baseColorLocation       = locationBase + "baseColor";
  static const std::string metallicFactorLocation  = locationBase + "metallicFactor";
  static const std::string roughnessFactorLocation = locationBase + "roughnessFactor";

  program.use();
  program.sendUniform(baseColorLocation,       m_baseColor);
  program.sendUniform(metallicFactorLocation,  m_metallicFactor);
  program.sendUniform(roughnessFactorLocation, m_roughnessFactor);

  m_baseColorMap->activate();
  m_baseColorMap->bind();

  m_normalMap->activate();
  m_normalMap->bind();

  m_metallicMap->activate();
  m_metallicMap->bind();

  m_roughnessMap->activate();
  m_roughnessMap->bind();

  m_ambientOcclusionMap->activate();
  m_ambientOcclusionMap->bind();
}

} // namespace Raz
