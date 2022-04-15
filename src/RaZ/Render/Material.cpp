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

void Material::loadType(MaterialType type) {
  switch (type) {
    case MaterialType::COOK_TORRANCE:
      if (!hasAttribute("uniMaterial.baseColor"))
        setAttribute(Vec3f(1.f), "uniMaterial.baseColor");
      if (!hasAttribute("uniMaterial.emissive"))
        setAttribute(Vec3f(0.f), "uniMaterial.emissive");
      if (!hasAttribute("uniMaterial.metallicFactor"))
        setAttribute(1.f, "uniMaterial.metallicFactor");
      if (!hasAttribute("uniMaterial.roughnessFactor"))
        setAttribute(1.f, "uniMaterial.roughnessFactor");

      if (!hasTexture("uniMaterial.baseColorMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.baseColorMap");
      if (!hasTexture("uniMaterial.emissiveMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.emissiveMap");
      if (!hasTexture("uniMaterial.normalMap"))
        setTexture(Texture::create(ColorPreset::MEDIUM_BLUE), "uniMaterial.normalMap"); // Representing a [ 0; 0; 1 ] vector
      if (!hasTexture("uniMaterial.metallicMap"))
        setTexture(Texture::create(ColorPreset::RED), "uniMaterial.metallicMap");
      if (!hasTexture("uniMaterial.roughnessMap"))
        setTexture(Texture::create(ColorPreset::RED), "uniMaterial.roughnessMap");
      if (!hasTexture("uniMaterial.ambientMap"))
        setTexture(Texture::create(ColorPreset::RED), "uniMaterial.ambientMap");

      break;

    case MaterialType::BLINN_PHONG:
      if (!hasAttribute("uniMaterial.baseColor"))
        setAttribute(Vec3f(1.f), "uniMaterial.baseColor");
      if (!hasAttribute("uniMaterial.emissive"))
        setAttribute(Vec3f(0.f), "uniMaterial.emissive");
      if (!hasAttribute("uniMaterial.ambient"))
        setAttribute(Vec3f(1.f), "uniMaterial.ambient");
      if (!hasAttribute("uniMaterial.specular"))
        setAttribute(Vec3f(1.f), "uniMaterial.specular");
      if (!hasAttribute("uniMaterial.transparency"))
        setAttribute(1.f, "uniMaterial.transparency");

      if (!hasTexture("uniMaterial.baseColorMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.baseColorMap");
      if (!hasTexture("uniMaterial.emissiveMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.emissiveMap");
      if (!hasTexture("uniMaterial.ambientMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.ambientMap");
      if (!hasTexture("uniMaterial.specularMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.specularMap");
      if (!hasTexture("uniMaterial.transparencyMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.transparencyMap");
      if (!hasTexture("uniMaterial.bumpMap"))
        setTexture(Texture::create(ColorPreset::WHITE), "uniMaterial.bumpMap");

      break;

    default:
      throw std::invalid_argument("Error: Unsupported material type");
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

} // namespace Raz
