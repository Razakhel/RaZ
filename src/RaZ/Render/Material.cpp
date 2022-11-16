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

constexpr std::string_view singleTexture2DShaderSource = {
#include "single_texture_2d.frag.embed"
};

constexpr std::string_view singleTexture3DShaderSource = {
#include "single_texture_3d.frag.embed"
};

} // namespace

void Material::loadType(MaterialType type) {
  switch (type) {
    case MaterialType::COOK_TORRANCE:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(cookTorranceShaderSource));

      if (!m_program.hasAttribute(MaterialAttribute::BaseColor))
        m_program.setAttribute(Vec3f(1.f), MaterialAttribute::BaseColor);
      if (!m_program.hasAttribute(MaterialAttribute::Emissive))
        m_program.setAttribute(Vec3f(0.f), MaterialAttribute::Emissive);
      if (!m_program.hasAttribute(MaterialAttribute::Metallic))
        m_program.setAttribute(0.f, MaterialAttribute::Metallic);
      if (!m_program.hasAttribute(MaterialAttribute::Roughness))
        m_program.setAttribute(1.f, MaterialAttribute::Roughness);

      if (!m_program.hasTexture(MaterialTexture::BaseColor))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::BaseColor);
      if (!m_program.hasTexture(MaterialTexture::Emissive))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Emissive);
      if (!m_program.hasTexture(MaterialTexture::Normal))
        m_program.setTexture(Texture2D::create(ColorPreset::MediumBlue), MaterialTexture::Normal); // Representing a [ 0; 0; 1 ] vector
      if (!m_program.hasTexture(MaterialTexture::Metallic))
        m_program.setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Metallic);
      if (!m_program.hasTexture(MaterialTexture::Roughness))
        m_program.setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Roughness);
      if (!m_program.hasTexture(MaterialTexture::Ambient))
        m_program.setTexture(Texture2D::create(ColorPreset::Red), MaterialTexture::Ambient);

      break;

    case MaterialType::BLINN_PHONG:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(blinnPhongShaderSource));

      if (!m_program.hasAttribute(MaterialAttribute::BaseColor))
        m_program.setAttribute(Vec3f(1.f), MaterialAttribute::BaseColor);
      if (!m_program.hasAttribute(MaterialAttribute::Emissive))
        m_program.setAttribute(Vec3f(0.f), MaterialAttribute::Emissive);
      if (!m_program.hasAttribute(MaterialAttribute::Ambient))
        m_program.setAttribute(Vec3f(1.f), MaterialAttribute::Ambient);
      if (!m_program.hasAttribute(MaterialAttribute::Specular))
        m_program.setAttribute(Vec3f(1.f), MaterialAttribute::Specular);
      if (!m_program.hasAttribute(MaterialAttribute::Transparency))
        m_program.setAttribute(1.f, MaterialAttribute::Transparency);

      if (!m_program.hasTexture(MaterialTexture::BaseColor))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::BaseColor);
      if (!m_program.hasTexture(MaterialTexture::Emissive))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Emissive);
      if (!m_program.hasTexture(MaterialTexture::Ambient))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Ambient);
      if (!m_program.hasTexture(MaterialTexture::Specular))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Specular);
      if (!m_program.hasTexture(MaterialTexture::Transparency))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Transparency);
      if (!m_program.hasTexture(MaterialTexture::Bump))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::Bump);

      break;

    case MaterialType::SINGLE_TEXTURE_2D:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(singleTexture2DShaderSource));

      if (!m_program.hasTexture(MaterialTexture::BaseColor))
        m_program.setTexture(Texture2D::create(ColorPreset::White), MaterialTexture::BaseColor);

      break;

    case MaterialType::SINGLE_TEXTURE_3D:
      m_program.setShaders(VertexShader::loadFromSource(vertShaderSource), FragmentShader::loadFromSource(singleTexture3DShaderSource));

      if (!m_program.hasTexture(MaterialTexture::BaseColor))
        m_program.setTexture(Texture3D::create(ColorPreset::White), MaterialTexture::BaseColor);

      break;

    default:
      throw std::invalid_argument("Error: Unsupported material type");
  }
}

} // namespace Raz
