#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Utils/FileUtils.hpp"

#include <fstream>
#include <map>

namespace Raz {

namespace {

void saveMtl(const std::string& mtlFilePath, const std::vector<MaterialPtr>& materials) {
  std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

  mtlFile << "# MTL file created with RaZ - https://github.com/Razakhel/RaZ\n";

  const std::string mtlFileName   = FileUtils::extractFileNameFromPath(mtlFilePath, false);
  const TexturePtr defaultTexture = Texture::create(ColorPreset::WHITE);

  for (std::size_t matIndex = 0; matIndex < materials.size(); ++matIndex) {
    const MaterialPtr& material    = materials[matIndex];
    const std::string materialName = mtlFileName + '_' + std::to_string(matIndex);

    mtlFile << "\nnewmtl " << materialName << '\n';
    mtlFile << "\tKd " << material->getBaseColor()[0] << ' ' << material->getBaseColor()[1] << ' ' << material->getBaseColor()[2] << '\n';

    if (material->getType() == MaterialType::COOK_TORRANCE) {
      const auto* matCT = static_cast<MaterialCookTorrance*>(material.get());

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
      const auto* matBP = static_cast<MaterialBlinnPhong*>(material.get());

      mtlFile << "\tKa " << matBP->getAmbient()[0] << ' ' << matBP->getAmbient()[1] << ' ' << matBP->getAmbient()[2] << '\n';
      mtlFile << "\tKs " << matBP->getSpecular()[0] << ' ' << matBP->getSpecular()[1] << ' ' << matBP->getSpecular()[2] << '\n';
      mtlFile << "\tKe " << matBP->getEmissive()[0] << ' ' << matBP->getEmissive()[1] << ' ' << matBP->getEmissive()[2] << '\n';
      mtlFile << "\td  " << matBP->getTransparency() << '\n';

      if (matBP->getDiffuseMap() && matBP->getDiffuseMap() != defaultTexture) {
        const auto diffuseMapPath = materialName + "_diffuse.png";

        mtlFile << "\tmap_Kd " << diffuseMapPath << '\n';
        matBP->getDiffuseMap()->save(diffuseMapPath, true);
      }

      if (matBP->getAmbientMap() && matBP->getAmbientMap() != defaultTexture) {
        const auto ambientMapPath = materialName + "_ambient.png";

        mtlFile << "\tmap_Ka " << ambientMapPath << '\n';
        matBP->getAmbientMap()->save(ambientMapPath, true);
      }

      if (matBP->getSpecularMap() && matBP->getSpecularMap() != defaultTexture) {
        const auto specularMapPath = materialName + "_specular.png";

        mtlFile << "\tmap_Ks " << specularMapPath << '\n';
        matBP->getSpecularMap()->save(specularMapPath, true);
      }

      if (matBP->getEmissiveMap() && matBP->getEmissiveMap() != defaultTexture) {
        const auto emissiveMapPath = materialName + "_emissive.png";

        mtlFile << "\tmap_Ke " << emissiveMapPath << '\n';
        matBP->getEmissiveMap()->save(emissiveMapPath, true);
      }

      if (matBP->getTransparencyMap() && matBP->getTransparencyMap() != defaultTexture) {
        const auto transparencyMapPath = materialName + "_transparency.png";

        mtlFile << "\tmap_d " << transparencyMapPath << '\n';
        matBP->getTransparencyMap()->save(transparencyMapPath, true);
      }

      if (matBP->getBumpMap() && matBP->getBumpMap() != defaultTexture) {
        const auto ambOccMapPath = materialName + "_bump.png";

        mtlFile << "\tmap_bump " << ambOccMapPath << '\n';
        matBP->getBumpMap()->save(ambOccMapPath, true);
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

  for (const Submesh& submesh : m_submeshes) {
    for (const Vertex& vertex : submesh.getVertices()) {
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

  const std::string fileName = FileUtils::extractFileNameFromPath(filePath, false);

  for (std::size_t submeshIndex = 0; submeshIndex < m_submeshes.size(); ++submeshIndex) {
    const Submesh& submesh = m_submeshes[submeshIndex];

    file << "\no " << fileName << '_' << submeshIndex << '\n';

    if (!m_materials.empty())
      file << "usemtl " << fileName << '_' << submesh.getMaterialIndex() << '\n';

    for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
      file << "f ";

      // First vertex
      Vertex vertex = submesh.getVertices()[submesh.getTriangleIndices()[i + 1]];

      std::array<float, 3> pos  = { vertex.position[0], vertex.position[1], vertex.position[2] };
      std::array<float, 2> tex  = { vertex.texcoords[0], vertex.texcoords[1] };
      std::array<float, 3> norm = { vertex.normal[0], vertex.normal[1], vertex.normal[2] };

      auto posIndex  = posCorrespIndices.find(pos)->second;
      auto texIndex  = texCorrespIndices.find(tex)->second;
      auto normIndex = normCorrespIndices.find(norm)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Second vertex
      vertex = submesh.getVertices()[submesh.getTriangleIndices()[i]];

      pos  = { vertex.position[0], vertex.position[1], vertex.position[2] };
      tex  = { vertex.texcoords[0], vertex.texcoords[1] };
      norm = { vertex.normal[0], vertex.normal[1], vertex.normal[2] };

      posIndex  = posCorrespIndices.find(pos)->second;
      texIndex  = texCorrespIndices.find(tex)->second;
      normIndex = normCorrespIndices.find(norm)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Third vertex
      vertex = submesh.getVertices()[submesh.getTriangleIndices()[i + 2]];

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
