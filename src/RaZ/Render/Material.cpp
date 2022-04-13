#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

bool Material::hasAttribute(const std::string& uniformName) const noexcept {
  return (m_attributes.find(uniformName) != m_attributes.cend());
}

bool Material::hasTexture(const Texture& texture) const noexcept {
  return std::any_of(m_textures.cbegin(), m_textures.cend(), [&texture] (const std::pair<TexturePtr, std::string>& element) {
    return (element.first->getIndex() == texture.getIndex());
  });
}

bool Material::hasTexture(const std::string& uniformName) const noexcept {
  return std::any_of(m_textures.cbegin(), m_textures.cend(), [&uniformName] (const std::pair<TexturePtr, std::string>& element) {
    return (element.second == uniformName);
  });
}

const Texture& Material::getTexture(const std::string& uniformName) const {
  const auto textureIt = std::find_if(m_textures.begin(), m_textures.end(), [&uniformName] (const std::pair<TexturePtr, std::string>& element) {
    return (element.second == uniformName);
  });

  if (textureIt == m_textures.cend())
    throw std::invalid_argument("Error: The given attribute uniform name does not exist");

  return *textureIt->first;
}

void Material::setTexture(TexturePtr texture, std::string uniformName) {
  const auto textureIt = std::find_if(m_textures.begin(), m_textures.end(), [&uniformName] (const std::pair<TexturePtr, std::string>& element) {
    return (element.second == uniformName);
  });

  if (textureIt != m_textures.end())
    textureIt->first = std::move(texture);
  else
    m_textures.emplace_back(std::move(texture), std::move(uniformName));
}

void Material::removeAttribute(const std::string& uniformName) {
  const auto attribIt = m_attributes.find(uniformName);

  if (attribIt == m_attributes.end())
    throw std::invalid_argument("Error: The given attribute uniform name does not exist");

  m_attributes.erase(attribIt);
}

void Material::removeTexture(const Texture& texture) {
  m_textures.erase(std::remove_if(m_textures.begin(), m_textures.end(), [&texture] (const std::pair<TexturePtr, std::string>& element) {
    return (element.first->getIndex() == texture.getIndex());
  }), m_textures.end());
}

void Material::removeTexture(const std::string& uniformName) {
  for (auto textureIt = m_textures.begin(); textureIt != m_textures.end(); ++textureIt) {
    if (textureIt->second != uniformName)
      continue;

    m_textures.erase(textureIt);
    return;
  }
}

void Material::sendAttributes(const RenderShaderProgram& program) const {
  program.use();

  for (const auto& [name, attrib] : m_attributes)
    std::visit([&program, &uniformName = name] (const auto& value) { program.sendUniform(uniformName, value); }, attrib);
}

void Material::initTextures(const RenderShaderProgram& program) const {
  program.use();

  // TODO: binding indices should be user-definable to allow the same texture to be bound to multiple uniforms
  int bindingIndex = 0;

  for (const auto& [texture, name] : m_textures)
    program.sendUniform(name, bindingIndex++);
}

void Material::bindTextures(const RenderShaderProgram& program) const {
  program.use();

  unsigned int textureIndex = 0;

  for (const auto& [texture, _] : m_textures) {
    Renderer::activateTexture(textureIndex++);
    texture->bind();
  }
}

MaterialBlinnPhong::MaterialBlinnPhong() {
  m_textures.reserve(6);
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.diffuseMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.emissiveMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.ambientMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.specularMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.transparencyMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.bumpMap");

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
  setDiffuseMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialBlinnPhong::loadEmissiveMap(const FilePath& filePath, bool flipVertically) {
  setEmissiveMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialBlinnPhong::loadAmbientMap(const FilePath& filePath, bool flipVertically) {
  setAmbientMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialBlinnPhong::loadSpecularMap(const FilePath& filePath, bool flipVertically) {
  setSpecularMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialBlinnPhong::loadTransparencyMap(const FilePath& filePath, bool flipVertically) {
  setTransparencyMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialBlinnPhong::loadBumpMap(const FilePath& filePath, bool flipVertically) {
  setBumpMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

MaterialCookTorrance::MaterialCookTorrance() {
  m_textures.reserve(6);
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.albedoMap");
  m_textures.emplace_back(Texture::create(ColorPreset::WHITE), "uniMaterial.emissiveMap");
  m_textures.emplace_back(Texture::create(ColorPreset::MEDIUM_BLUE), "uniMaterial.normalMap"); // Representing a [ 0; 0; 1 ] vector
  m_textures.emplace_back(Texture::create(ColorPreset::RED), "uniMaterial.metallicMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED), "uniMaterial.roughnessMap");
  m_textures.emplace_back(Texture::create(ColorPreset::RED), "uniMaterial.ambientOcclusionMap");

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
  setAlbedoMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialCookTorrance::loadEmissiveMap(const FilePath& filePath, bool flipVertically) {
  setEmissiveMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialCookTorrance::loadNormalMap(const FilePath& filePath, bool flipVertically) {
  setNormalMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialCookTorrance::loadMetallicMap(const FilePath& filePath, bool flipVertically) {
  setMetallicMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialCookTorrance::loadRoughnessMap(const FilePath& filePath, bool flipVertically) {
  setRoughnessMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

void MaterialCookTorrance::loadAmbientOcclusionMap(const FilePath& filePath, bool flipVertically) {
  setAmbientOcclusionMap(Texture::create(ImageFormat::load(filePath, flipVertically)));
}

} // namespace Raz
