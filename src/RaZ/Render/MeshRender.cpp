#include "RaZ/Render/Mesh.hpp"

namespace Raz {

std::size_t Mesh::recoverVertexCount() const {
  std::size_t vertexCount = 0;

  for (const SubmeshPtr& submesh : m_submeshes)
    vertexCount += submesh->getVertexCount();

  return vertexCount;
}

std::size_t Mesh::recoverTriangleCount() const {
  std::size_t indexCount = 0;

  for (const SubmeshPtr& submesh : m_submeshes)
    indexCount += submesh->getIndexCount();

  return indexCount / 3;
}

void Mesh::drawUnitQuad() {
  static const MeshPtr quadMesh = Mesh::create(Quad(Vec3f({ -1.f,  1.f, 0.f }),
                                                    Vec3f({  1.f,  1.f, 0.f }),
                                                    Vec3f({  1.f, -1.f, 0.f }),
                                                    Vec3f({ -1.f, -1.f, 0.f })));

  quadMesh->draw();
}

void Mesh::drawUnitCube() {
  static const MeshPtr cubeMesh = Mesh::create(AABB(Vec3f({  1.f,  1.f,  1.f }),
                                                    Vec3f({ -1.f, -1.f, -1.f })));

  cubeMesh->draw();
}

void Mesh::setMaterial(MaterialPreset materialPreset, float roughnessFactor) {
  const MaterialCookTorrancePtr& newMaterial = Material::recoverMaterial(materialPreset, roughnessFactor);

  for (MaterialPtr& material : m_materials) {
    if (material->getType() == MaterialType::COOK_TORRANCE) {
      auto materialCT = dynamic_cast<MaterialCookTorrance*>(material.get());

      materialCT->setBaseColor(newMaterial->getBaseColor());
      materialCT->setMetallicFactor(newMaterial->getMetallicFactor());
      materialCT->setRoughnessFactor(roughnessFactor);
      materialCT->setAlbedoMap(Texture::recoverTexture(TexturePreset::WHITE));
    } else {
      auto materialStd = dynamic_cast<MaterialStandard*>(material.get());
      const float specular = newMaterial->getMetallicFactor() * (1.f - roughnessFactor);

      materialStd->setDiffuse(newMaterial->getBaseColor());
      materialStd->setSpecular(Vec3f(specular));
      materialStd->setDiffuseMap(Texture::recoverTexture(TexturePreset::WHITE));
    }
  }
}

void Mesh::load() const {
  for (const SubmeshPtr& submesh : m_submeshes)
    submesh->load();
}

void Mesh::load(const ShaderProgram& program) const {
  load();

  for (const MaterialPtr& material : m_materials)
    material->initTextures(program);
}

void Mesh::draw() const {
  for (const SubmeshPtr& submesh : m_submeshes)
    submesh->draw();
}

void Mesh::draw(const ShaderProgram& program) const {
  for (const SubmeshPtr& submesh : m_submeshes) {
    if (!m_materials.empty()) {
      const MaterialPtr& material = m_materials[submesh->getMaterialIndex()];

      if (material)
        material->bindAttributes(program);
    }

    submesh->draw();
  }
}

} // namespace Raz
