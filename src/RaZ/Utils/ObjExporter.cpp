#include <fstream>
#include <map>

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Utils/FileUtils.hpp"

namespace Raz {

namespace {

void saveMtl(const std::string& mtlFilePath, const std::vector<MaterialPtr>& materials) {
  std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

  mtlFile << "# MTL file created by RaZ - https://github.com/Razakhel/RaZ\n";

  const std::string mtlFileName = FileUtils::extractFileNameFromPath(mtlFilePath, false);
  const auto defaultTexture = Texture::recoverTexture(TexturePreset::WHITE);

  for (std::size_t matIndex = 0; matIndex < materials.size(); ++matIndex) {
    const MaterialPtr& material = materials[matIndex];
    const std::string materialName = mtlFileName + '_' + std::to_string(matIndex);

    mtlFile << "\nnewmtl " << materialName << '\n';

    if (material->getType() == MaterialType::COOK_TORRANCE) {
      const auto matCT = dynamic_cast<MaterialCookTorrance*>(material.get());

      mtlFile << "\tKd " << matCT->getBaseColor()[0] << ' ' << matCT->getBaseColor()[1] << ' ' << matCT->getBaseColor()[2] << '\n';
      mtlFile << "\tPm " << matCT->getMetallicFactor() << '\n';
      mtlFile << "\tPr " << matCT->getRoughnessFactor() << '\n';

      if (matCT->getAlbedoMap() && matCT->getAlbedoMap() != defaultTexture) {
        const auto albedoMapPath = materialName + "_albedo.png";

        mtlFile << "\tmap_Kd " << albedoMapPath << '\n';
        matCT->getAlbedoMap()->save(albedoMapPath, true);
      }

      if (matCT->getNormalMap() && matCT->getNormalMap() != defaultTexture) {
        const auto normalMapPath = materialName + "_normal.png";

        mtlFile << "\tnorm " << normalMapPath << '\n';
        matCT->getNormalMap()->save(normalMapPath, true);
      }

      if (matCT->getMetallicMap() && matCT->getMetallicMap() != defaultTexture) {
        const auto metallicMapPath = materialName + "_metallic.png";

        mtlFile << "\tmap_Pm " << metallicMapPath << '\n';
        matCT->getMetallicMap()->save(metallicMapPath, true);
      }

      if (matCT->getRoughnessMap() && matCT->getRoughnessMap() != defaultTexture) {
        const auto roughnessMapPath = materialName + "_roughness.png";

        mtlFile << "\tmap_Pr " << roughnessMapPath << '\n';
        matCT->getRoughnessMap()->save(roughnessMapPath, true);
      }

      if (matCT->getAmbientOcclusionMap() && matCT->getAmbientOcclusionMap() != defaultTexture) {
        const auto ambOccMapPath = materialName + "_ambient_occlusion.png";

        mtlFile << "\tmap_Ka " << ambOccMapPath << '\n';
        matCT->getAmbientOcclusionMap()->save(ambOccMapPath, true);
      }
    } else {
      const auto matSTD = dynamic_cast<MaterialStandard*>(material.get());

      mtlFile << "\tKa " << matSTD->getAmbient()[0] << ' ' << matSTD->getAmbient()[1] << ' ' << matSTD->getAmbient()[2] << '\n';
      mtlFile << "\tKd " << matSTD->getDiffuse()[0] << ' ' << matSTD->getDiffuse()[1] << ' ' << matSTD->getDiffuse()[2] << '\n';
      mtlFile << "\tKs " << matSTD->getSpecular()[0] << ' ' << matSTD->getSpecular()[1] << ' ' << matSTD->getSpecular()[2] << '\n';
      mtlFile << "\tKe " << matSTD->getEmissive()[0] << ' ' << matSTD->getEmissive()[1] << ' ' << matSTD->getEmissive()[2] << '\n';
      mtlFile << "\td  " << matSTD->getTransparency() << '\n';

      if (matSTD->getAmbientMap() && matSTD->getAmbientMap() != defaultTexture) {
        const auto ambientMapPath = materialName + "_ambient.png";

        mtlFile << "\tmap_Ka " << ambientMapPath << '\n';
        matSTD->getAmbientMap()->save(ambientMapPath);
      }

      if (matSTD->getDiffuseMap() && matSTD->getDiffuseMap() != defaultTexture) {
        const auto diffuseMapPath = materialName + "_diffuse.png";

        mtlFile << "\tmap_Kd " << diffuseMapPath << '\n';
        matSTD->getDiffuseMap()->save(diffuseMapPath);
      }

      if (matSTD->getSpecularMap() && matSTD->getSpecularMap() != defaultTexture) {
        const auto specularMapPath = materialName + "_specular.png";

        mtlFile << "\tmap_Ks " << specularMapPath << '\n';
        matSTD->getSpecularMap()->save(specularMapPath);
      }

      if (matSTD->getEmissiveMap() && matSTD->getEmissiveMap() != defaultTexture) {
        const auto emissiveMapPath = materialName + "_emissive.png";

        mtlFile << "\tmap_Ke " << emissiveMapPath << '\n';
        matSTD->getEmissiveMap()->save(emissiveMapPath);
      }

      if (matSTD->getTransparencyMap() && matSTD->getTransparencyMap() != defaultTexture) {
        const auto transparencyMapPath = materialName + "_transparency.png";

        mtlFile << "\tmap_d " << transparencyMapPath << '\n';
        matSTD->getTransparencyMap()->save(transparencyMapPath);
      }

      if (matSTD->getBumpMap() && matSTD->getBumpMap() != defaultTexture) {
        const auto ambOccMapPath = materialName + "_bump.png";

        mtlFile << "\tmap_bump " << ambOccMapPath << '\n';
        matSTD->getBumpMap()->save(ambOccMapPath);
      }
    }
  }
}

} // namespace

void Mesh::saveObj(std::ofstream& file, const std::string& filePath) const {
  file << "# OBJ file created with RaZ - https://github.com/Razakhel/RaZ\n\n";

  if (!m_materials.empty()) {
    const auto mtlFileName = FileUtils::extractFileNameFromPath(filePath, false) + ".mtl";
    const auto mtlFilePath = FileUtils::extractPathToFile(filePath) + mtlFileName;

    file << "mtllib " << mtlFilePath << "\n\n";

    std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

    saveMtl(mtlFilePath, m_materials);
  }

  std::map<std::array<float, 3>, std::size_t> posCorrespIndices;
  std::map<std::array<float, 2>, std::size_t> texCorrespIndices;
  std::map<std::array<float, 3>, std::size_t> normCorrespIndices;

  for (const auto& submesh : m_submeshes) {
    for (const auto& vertex : submesh->getVertices()) {
      const std::array<float, 3> pos = { vertex.position[0], vertex.position[1], vertex.position[2] };

      if (posCorrespIndices.find(pos) == posCorrespIndices.cend()) {
        file << "v " << vertex.position[0] << ' '
                     << vertex.position[1] << ' '
                     << vertex.position[2] << '\n';
        posCorrespIndices.emplace(pos, posCorrespIndices.size() + 1);
      }

      const std::array<float, 2> tex = { vertex.texcoords[0], vertex.texcoords[1] };

      if (texCorrespIndices.find(tex) == texCorrespIndices.cend()) {
        file << "vt " << vertex.texcoords[0] << ' '
                      << vertex.texcoords[1] << '\n';
        texCorrespIndices.emplace(tex, texCorrespIndices.size() + 1);
      }

      const std::array<float, 3> norm = { vertex.normal[0], vertex.normal[1], vertex.normal[2] };

      if (normCorrespIndices.find(norm) == normCorrespIndices.cend()) {
        file << "vn " << vertex.normal[0] << ' '
                      << vertex.normal[1] << ' '
                      << vertex.normal[2] << '\n';
        normCorrespIndices.emplace(norm, normCorrespIndices.size() + 1);
      }
    }
  }

  const auto fileName = FileUtils::extractFileNameFromPath(filePath, false);

  for (std::size_t submeshIndex = 0; submeshIndex < m_submeshes.size(); ++submeshIndex) {
    const auto& submesh = m_submeshes[submeshIndex];

    file << "\no " << fileName << '_' << submeshIndex << '\n';

    if (!m_materials.empty())
      file << "usemtl " << fileName << '_' << submesh->getMaterialIndex() << '\n';

    for (std::size_t i = 0; i < submesh->getIndexCount(); i += 3) {
      file << "f ";

      // First vertex
      auto vertex = submesh->getVertices()[submesh->getIndices()[i + 1]];

      std::array<float, 3> pos  = { vertex.position[0], vertex.position[1], vertex.position[2] };
      std::array<float, 2> tex  = { vertex.texcoords[0], vertex.texcoords[1] };
      std::array<float, 3> norm = { vertex.normal[0], vertex.normal[1], vertex.normal[2] };

      auto posIndex  = posCorrespIndices.find(pos)->second;
      auto texIndex  = texCorrespIndices.find(tex)->second;
      auto normIndex = normCorrespIndices.find(norm)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Second vertex
      vertex = submesh->getVertices()[submesh->getIndices()[i]];

      pos  = { vertex.position[0], vertex.position[1], vertex.position[2] };
      tex  = { vertex.texcoords[0], vertex.texcoords[1] };
      norm = { vertex.normal[0], vertex.normal[1], vertex.normal[2] };

      posIndex  = posCorrespIndices.find(pos)->second;
      texIndex  = texCorrespIndices.find(tex)->second;
      normIndex = normCorrespIndices.find(norm)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Third vertex
      vertex = submesh->getVertices()[submesh->getIndices()[i + 2]];

      pos  = std::array<float, 3>({ vertex.position[0], vertex.position[1], vertex.position[2] });
      tex  = std::array<float, 2>({ vertex.texcoords[0], vertex.texcoords[1] });
      norm = std::array<float, 3>({ vertex.normal[0], vertex.normal[1], vertex.normal[2] });

      posIndex  = posCorrespIndices.find(pos)->second;
      texIndex  = texCorrespIndices.find(tex)->second;
      normIndex = normCorrespIndices.find(norm)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << '\n';
    }
  }
}

} // namespace Raz
