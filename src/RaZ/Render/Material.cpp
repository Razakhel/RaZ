#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

bool Material::hasAttribute(const std::string& uniformName) const noexcept {
  return (m_attributes.find(uniformName) != m_attributes.cend());
}

void Material::addTexture(TexturePtr texture, std::string uniformName) {
  m_textures.emplace_back(std::move(texture), std::move(uniformName));
}

void Material::loadTexture(const FilePath& filePath, int bindingIndex, std::string uniformName, bool flipVertically) {
  addTexture(Texture::create(ImageFormat::load(filePath, flipVertically), bindingIndex), std::move(uniformName));
}

void Material::removeAttribute(const std::string& uniformName) {
  const auto attribIt = m_attributes.find(uniformName);

  if (attribIt == m_attributes.end())
    throw std::invalid_argument("Error: The given attribute uniform name does not exist");

  m_attributes.erase(attribIt);
}

void Material::sendAttributes(const RenderShaderProgram& program) const {
  program.use();

  for (const auto& [location, attrib] : m_attributes)
    std::visit([&program, &uniformName = location] (const auto& value) { program.sendUniform(uniformName, value); }, attrib);
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

  m_attributes.reserve(5);
  setDiffuse(Vec3f(1.f));
  setEmissive(Vec3f(0.f));
  setAmbient(Vec3f(1.f));
  setSpecular(Vec3f(1.f));
  setTransparency(1.f);
}

MaterialBlinnPhong::MaterialBlinnPhong(const Vec3f& diffuse,
                                       const Vec3f& ambient,
                                       const Vec3f& specular,
                                       const Vec3f& emissive,
                                       float transparency) : MaterialBlinnPhong() {
  setDiffuse(diffuse);
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

MaterialCookTorrance::MaterialCookTorrance() {
  m_textures.reserve(6);
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 0), "uniMaterial.albedoMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE, 1), "uniMaterial.emissiveMap");
  m_textures.emplace_back(Texture::create(ColorPreset::MEDIUM_BLUE, 2), "uniMaterial.normalMap"); // Representing a [ 0; 0; 1 ] vector
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 3), "uniMaterial.metallicMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 4), "uniMaterial.roughnessMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED, 5), "uniMaterial.ambientOcclusionMap");

  m_attributes.reserve(4);
  setBaseColor(Vec3f(1.f));
  setEmissive(Vec3f(0.f));
  setMetallicFactor(1.f);
  setRoughnessFactor(1.f);
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

} // namespace Raz
