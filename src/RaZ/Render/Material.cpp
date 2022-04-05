#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

void Material::addTexture(TexturePtr texture, std::string uniformName) {
  m_textures.emplace_back(std::move(texture), std::move(uniformName));
}

void Material::loadTexture(const FilePath& filePath, int bindingIndex, std::string uniformName, bool flipVertically) {
  addTexture(Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex), std::move(uniformName));
}

void Material::initTextures(const RenderShaderProgram& program) const {
  program.use();

  for (const auto& [texture, location] : m_textures)
    program.sendUniform(location, texture->getBindingIndex());
}

void Material::bindTextures(const RenderShaderProgram& program) const {
  program.use();

  for (const auto& [texture, _] : m_textures) {
    texture->activate();
    texture->bind();
  }
}

MaterialBlinnPhong::MaterialBlinnPhong() {
  m_textures.reserve(6);

  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 0), "uniMaterial.diffuseMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 1), "uniMaterial.emissiveMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 2), "uniMaterial.ambientMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 3), "uniMaterial.specularMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 4), "uniMaterial.transparencyMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 5), "uniMaterial.bumpMap");
}

MaterialBlinnPhong::MaterialBlinnPhong(const Vec3f& baseColor,
                                       const Vec3f& ambient,
                                       const Vec3f& specular,
                                       const Vec3f& emissive,
                                       float transparency) : MaterialBlinnPhong() {
  setBaseColor(baseColor);
  setEmissive(emissive);
  setAmbient(ambient);
  setSpecular(specular);
  setTransparency(transparency);
}

void MaterialBlinnPhong::loadDiffuseMap(const FilePath& filePath, bool flipVertically) {
  setDiffuseMap(Texture::create(ImageFormat::load(filePath, flipVertically), getDiffuseMap()->getBindingIndex()));
}

void MaterialBlinnPhong::loadEmissiveMap(const FilePath& filePath, bool flipVertically) {
  setEmissiveMap(Texture::create(ImageFormat::load(filePath, flipVertically), getEmissiveMap()->getBindingIndex()));
}

void MaterialBlinnPhong::loadAmbientMap(const FilePath& filePath, bool flipVertically) {
  setAmbientMap(Texture::create(ImageFormat::load(filePath, flipVertically), getAmbientMap()->getBindingIndex()));
}

void MaterialBlinnPhong::loadSpecularMap(const FilePath& filePath, bool flipVertically) {
  setSpecularMap(Texture::create(ImageFormat::load(filePath, flipVertically), getSpecularMap()->getBindingIndex()));
}

void MaterialBlinnPhong::loadTransparencyMap(const FilePath& filePath, bool flipVertically) {
  setTransparencyMap(Texture::create(ImageFormat::load(filePath, flipVertically), getTransparencyMap()->getBindingIndex()));
}

void MaterialBlinnPhong::loadBumpMap(const FilePath& filePath, bool flipVertically) {
  setBumpMap(Texture::create(ImageFormat::load(filePath, flipVertically), getBumpMap()->getBindingIndex()));
}

void MaterialBlinnPhong::bindAttributes(const RenderShaderProgram& program) const {
  bindTextures(program); // Binding textures marks the program as used

  static const std::string locationBase = "uniMaterial.";

  static const std::string diffuseLocation      = locationBase + "diffuse";
  static const std::string emissiveLocation     = locationBase + "emissive";
  static const std::string ambientLocation      = locationBase + "ambient";
  static const std::string specularLocation     = locationBase + "specular";
  static const std::string transparencyLocation = locationBase + "transparency";

  program.sendUniform(diffuseLocation,      m_baseColor);
  program.sendUniform(emissiveLocation,     m_emissive);
  program.sendUniform(ambientLocation,      m_ambient);
  program.sendUniform(specularLocation,     m_specular);
  program.sendUniform(transparencyLocation, m_transparency);
}

MaterialCookTorrance::MaterialCookTorrance() {
  m_textures.reserve(6);

  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 0), "uniMaterial.albedoMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 1), "uniMaterial.emissiveMap");
  m_textures.emplace_back(Texture::create(ColorPreset::MEDIUM_BLUE, 2), "uniMaterial.normalMap"); // Representing a [ 0; 0; 1 ] vector
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 3), "uniMaterial.metallicMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 4), "uniMaterial.roughnessMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 5), "uniMaterial.ambientOcclusionMap");
}

MaterialCookTorrance::MaterialCookTorrance(const Vec3f& baseColor, float metallicFactor, float roughnessFactor) : MaterialCookTorrance() {
  setBaseColor(baseColor);
  setMetallicFactor(metallicFactor);
  setRoughnessFactor(roughnessFactor);
}

void MaterialCookTorrance::loadAlbedoMap(const FilePath& filePath, bool flipVertically) {
  setAlbedoMap(Texture::create(ImageFormat::load(filePath, flipVertically), getAlbedoMap()->getBindingIndex()));
}

void MaterialCookTorrance::loadEmissiveMap(const FilePath& filePath, bool flipVertically) {
  setEmissiveMap(Texture::create(ImageFormat::load(filePath, flipVertically), getEmissiveMap()->getBindingIndex()));
}

void MaterialCookTorrance::loadNormalMap(const FilePath& filePath, bool flipVertically) {
  setNormalMap(Texture::create(ImageFormat::load(filePath, flipVertically), getNormalMap()->getBindingIndex()));
}

void MaterialCookTorrance::loadMetallicMap(const FilePath& filePath, bool flipVertically) {
  setMetallicMap(Texture::create(ImageFormat::load(filePath, flipVertically), getMetallicMap()->getBindingIndex()));
}

void MaterialCookTorrance::loadRoughnessMap(const FilePath& filePath, bool flipVertically) {
  setRoughnessMap(Texture::create(ImageFormat::load(filePath, flipVertically), getRoughnessMap()->getBindingIndex()));
}

void MaterialCookTorrance::loadAmbientOcclusionMap(const FilePath& filePath, bool flipVertically) {
  setAmbientOcclusionMap(Texture::create(ImageFormat::load(filePath, flipVertically), getAmbientOcclusionMap()->getBindingIndex()));
}

void MaterialCookTorrance::bindAttributes(const RenderShaderProgram& program) const {
  bindTextures(program); // Binding textures marks the program as used

  static const std::string locationBase = "uniMaterial.";

  static const std::string baseColorLocation       = locationBase + "baseColor";
  static const std::string emissiveLocation        = locationBase + "emissive";
  static const std::string metallicFactorLocation  = locationBase + "metallicFactor";
  static const std::string roughnessFactorLocation = locationBase + "roughnessFactor";

  program.sendUniform(baseColorLocation,       m_baseColor);
  program.sendUniform(emissiveLocation,        m_emissive);
  program.sendUniform(metallicFactorLocation,  m_metallicFactor);
  program.sendUniform(roughnessFactorLocation, m_roughnessFactor);
}

} // namespace Raz
