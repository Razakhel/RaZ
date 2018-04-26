#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::drawQuad() {
  static const MeshPtr quadMesh = Mesh::createQuad(Vec3f({ -1.f,  1.f, 0.f }),
                                                   Vec3f({  1.f,  1.f, 0.f }),
                                                   Vec3f({  1.f, -1.f, 0.f }),
                                                   Vec3f({ -1.f, -1.f, 0.f }));

  quadMesh->draw();
}

void Mesh::drawCube() {
  static const MeshPtr cubeMesh = Mesh::createAABB(Vec3f({  1.f,  1.f,  1.f }),
                                                   Vec3f({ -1.f, -1.f, -1.f }));

  cubeMesh->draw();
}

void Mesh::setMaterial(MaterialPreset materialPreset, float roughnessFactor) {
  const auto& newMaterial = Material::recoverMaterial(materialPreset, roughnessFactor);

  for (auto& material : m_materials) {
    auto* cookTorranceMaterial = dynamic_cast<MaterialCookTorrance*>(material.get());

    if (cookTorranceMaterial) {
      cookTorranceMaterial->setBaseColor(newMaterial->getBaseColor());
      cookTorranceMaterial->setMetallicFactor(newMaterial->getMetallicFactor());
      cookTorranceMaterial->setRoughnessFactor(roughnessFactor);

      cookTorranceMaterial->setAlbedoMap(Texture::recoverTexture(TexturePreset::WHITE));
    }
  }
}

void Mesh::load(const ShaderProgram* program) const {
  for (const auto& submesh : m_submeshes)
    submesh->load();

  if (program) {
    for (const auto& material : m_materials)
      material->initTextures(*program);
  }
}

void Mesh::draw(const ShaderProgram* program) const {
  for (const auto& submesh : m_submeshes) {
    if (program) {
      if (!m_materials.empty()) {
        const auto& material = m_materials[submesh->getMaterialIndex()];

        if (material)
          material->bindAttributes(*program);
      }
    }

    submesh->draw();
  }
}

} // namespace Raz
