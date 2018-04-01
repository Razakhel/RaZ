#include "RaZ/Render/Material.hpp"

namespace Raz {

void MaterialStandard::initTextures(const ShaderProgram& program) const {
  const std::string locationBase = "uniMaterial.";

  const std::string ambientMapLocation = locationBase + "ambientMap";
  const std::string diffuseMapLocation = locationBase + "diffuseMap";
  const std::string specularMapLocation = locationBase + "specularMap";
  const std::string transparencyMapLocation = locationBase + "transparencyMap";
  const std::string bumpMapLocation = locationBase + "bumpMap";

  program.sendUniform(ambientMapLocation, 0);
  program.sendUniform(diffuseMapLocation, 1);
  program.sendUniform(specularMapLocation, 2);
  program.sendUniform(transparencyMapLocation, 3);
  program.sendUniform(bumpMapLocation, 4);
}

void MaterialStandard::bindAttributes(const ShaderProgram& program) const {
  const std::string locationBase = "uniMaterial.";

  const std::string ambientLocation = locationBase + "ambient";
  const std::string diffuseLocation = locationBase + "diffuse";
  const std::string specularLocation = locationBase + "specular";
  const std::string emissiveLocation = locationBase + "emissive";
  const std::string transparencyLocation = locationBase + "transparency";

  program.sendUniform(ambientLocation, m_ambient);
  program.sendUniform(diffuseLocation, m_diffuse);
  program.sendUniform(specularLocation, m_specular);
  program.sendUniform(emissiveLocation, m_emissive);
  program.sendUniform(transparencyLocation, m_transparency);

  glActiveTexture(GL_TEXTURE0);
  m_ambientMap->bind();

  glActiveTexture(GL_TEXTURE1);
  m_diffuseMap->bind();

   glActiveTexture(GL_TEXTURE2);
  m_specularMap->bind();

   glActiveTexture(GL_TEXTURE3);
  m_transparencyMap->bind();

   glActiveTexture(GL_TEXTURE4);
  m_bumpMap->bind();
}

void MaterialCookTorrance::initTextures(const ShaderProgram& program) const {
  const std::string locationBase = "uniMaterial.";

  const std::string albedoMapLocation = locationBase + "albedoMap";
  const std::string normalMapLocation = locationBase + "normalMap";
  const std::string metallicMapLocation = locationBase + "metallicMap";
  const std::string roughnessMapLocation = locationBase + "roughnessMap";
  const std::string ambientOcclusionMapLocation = locationBase + "ambientOcclusionMap";

  program.sendUniform(albedoMapLocation, 0);
  program.sendUniform(normalMapLocation, 1);
  program.sendUniform(metallicMapLocation, 2);
  program.sendUniform(roughnessMapLocation, 3);
  program.sendUniform(ambientOcclusionMapLocation, 4);
}

void MaterialCookTorrance::bindAttributes(const ShaderProgram& program) const {
  const std::string locationBase = "uniMaterial.";

  const std::string metallicFactorLocation = locationBase + "metallicFactor";
  const std::string roughnessFactorLocation = locationBase + "roughnessFactor";

  program.sendUniform(metallicFactorLocation, m_metallicFactor);
  program.sendUniform(roughnessFactorLocation, m_roughnessFactor);

  glActiveTexture(GL_TEXTURE0);
  m_albedoMap->bind();

  glActiveTexture(GL_TEXTURE1);
  m_normalMap->bind();

  glActiveTexture(GL_TEXTURE2);
  m_metallicMap->bind();

  glActiveTexture(GL_TEXTURE3);
  m_roughnessMap->bind();

  glActiveTexture(GL_TEXTURE4);
  m_ambientOcclusionMap->bind();
}

} // namespace Raz
