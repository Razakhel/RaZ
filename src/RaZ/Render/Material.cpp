#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/Renderer.hpp"

namespace Raz {

namespace {

constexpr std::string_view vertShaderSource = {
#include "common.vert.embed"
};

constexpr std::string_view cookTorranceShaderSource = {
#include "cook-torrance.frag.embed"
};

constexpr std::string_view blinnPhongShaderSource = {
#include "blinn-phong.frag.embed"
};

} // namespace

bool Material::hasAttribute(const std::string& uniformName) const noexcept {
  return (m_attributes.find(uniformName) != m_attributes.cend());
}

bool Material::hasTexture(const Texture2D& texture) const noexcept {
  return std::any_of(m_textures.cbegin(), m_textures.cend(), [&texture] (const std::pair<Texture2DPtr, std::string>& element) {
    return (element.first->getIndex() == texture.getIndex());
  });
}

bool Material::hasTexture(const std::string& uniformName) const noexcept {
  return std::any_of(m_textures.cbegin(), m_textures.cend(), [&uniformName] (const std::pair<Texture2DPtr, std::string>& element) {
    return (element.second == uniformName);
  });
}

const Texture2D& Material::getTexture(const std::string& uniformName) const {
  const auto textureIt = std::find_if(m_textures.begin(), m_textures.end(), [&uniformName] (const std::pair<Texture2DPtr, std::string>& element) {
    return (element.second == uniformName);
  });

  if (textureIt == m_textures.cend())
    throw std::invalid_argument("Error: The given attribute uniform name does not exist");

  return *textureIt->first;
}

void Material::setTexture(Texture2DPtr texture, std::string uniformName) {
  const auto textureIt = std::find_if(m_textures.begin(), m_textures.end(), [&uniformName] (const std::pair<Texture2DPtr, std::string>& element) {
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

void Material::loadType(MaterialType type) {
  switch (type) {
    case MaterialType::COOK_TORRANCE:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(cookTorranceShaderSource));

      if (!hasAttribute(MaterialAttribute::BaseColor))
        setAttribute(Vec3f(1.f), MaterialAttribute::BaseColor);
      if (!hasAttribute(MaterialAttribute::Emissive))
        setAttribute(Vec3f(0.f), MaterialAttribute::Emissive);
      if (!hasAttribute(MaterialAttribute::Metallic))
        setAttribute(0.f, MaterialAttribute::Metallic);
      if (!hasAttribute(MaterialAttribute::Roughness))
        setAttribute(1.f, MaterialAttribute::Roughness);

      if (!hasTexture(MaterialTexture::BaseColor))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::BaseColor);
      if (!hasTexture(MaterialTexture::Emissive))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Emissive);
      if (!hasTexture(MaterialTexture::Normal))
        setTexture(Texture2D::create(ColorPreset::MediumBlue), MaterialTexture::Normal); // Representing a [ 0; 0; 1 ] vector
      if (!hasTexture(MaterialTexture::Metallic))
        setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Metallic);
      if (!hasTexture(MaterialTexture::Roughness))
        setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Roughness);
      if (!hasTexture(MaterialTexture::Ambient))
        setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Ambient);

      break;

    case MaterialType::BLINN_PHONG:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(blinnPhongShaderSource));

      if (!hasAttribute(MaterialAttribute::BaseColor))
        setAttribute(Vec3f(1.f), MaterialAttribute::BaseColor);
      if (!hasAttribute(MaterialAttribute::Emissive))
        setAttribute(Vec3f(0.f), MaterialAttribute::Emissive);
      if (!hasAttribute(MaterialAttribute::Ambient))
        setAttribute(Vec3f(1.f), MaterialAttribute::Ambient);
      if (!hasAttribute(MaterialAttribute::Specular))
        setAttribute(Vec3f(1.f), MaterialAttribute::Specular);
      if (!hasAttribute(MaterialAttribute::Transparency))
        setAttribute(1.f, MaterialAttribute::Transparency);

      if (!hasTexture(MaterialTexture::BaseColor))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::BaseColor);
      if (!hasTexture(MaterialTexture::Emissive))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Emissive);
      if (!hasTexture(MaterialTexture::Ambient))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Ambient);
      if (!hasTexture(MaterialTexture::Specular))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Specular);
      if (!hasTexture(MaterialTexture::Transparency))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Transparency);
      if (!hasTexture(MaterialTexture::Bump))
        setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Bump);

      break;

    default:
      throw std::invalid_argument("Error: Unsupported material type");
  }
}

void Material::sendAttributes() const {
  if (m_attributes.empty())
    return;

  m_program.use();

  for (const auto& [name, attrib] : m_attributes)
    std::visit([this, &uniformName = name] (const auto& value) { m_program.sendUniform(uniformName, value); }, attrib);
}

void Material::initTextures() const {
  if (m_textures.empty())
    return;

  m_program.use();

  // TODO: binding indices should be user-definable to allow the same texture to be bound to multiple uniforms
  int bindingIndex = 0;

  for (const auto& [texture, name] : m_textures)
    m_program.sendUniform(name, bindingIndex++);
}

void Material::bindTextures() const {
  m_program.use();

  unsigned int textureIndex = 0;

  for (const auto& [texture, _] : m_textures) {
    Renderer::activateTexture(textureIndex++);
    texture->bind();
  }
}

void Material::removeTexture(const Texture2D& texture) {
  m_textures.erase(std::remove_if(m_textures.begin(), m_textures.end(), [&texture] (const std::pair<Texture2DPtr, std::string>& element) {
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

} // namespace Raz
