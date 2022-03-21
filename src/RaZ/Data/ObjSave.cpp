#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <fstream>
#include <map>

namespace Raz::ObjFormat {

namespace {

void saveMtl(const FilePath& mtlFilePath, const std::vector<MaterialPtr>& materials) {
  std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

  mtlFile << "# MTL file created with RaZ - https://github.com/Razakhel/RaZ\n";

  const std::string mtlFileName = mtlFilePath.recoverFileName(false).toUtf8();

  for (std::size_t matIndex = 0; matIndex < materials.size(); ++matIndex) {
    const MaterialPtr& material    = materials[matIndex];
    const std::string materialName = mtlFileName + '_' + std::to_string(matIndex);

    mtlFile << "\nnewmtl " << materialName << '\n';
    mtlFile << "\tKd " << material->getBaseColor()[0] << ' ' << material->getBaseColor()[1] << ' ' << material->getBaseColor()[2] << '\n';

    if (material->getType() == MaterialType::COOK_TORRANCE) {
      const auto* matCT = static_cast<MaterialCookTorrance*>(material.get());

      mtlFile << "\tPm " << matCT->getMetallicFactor() << '\n';
      mtlFile << "\tPr " << matCT->getRoughnessFactor() << '\n';

      if (matCT->getAlbedoMap() && !matCT->getAlbedoMap()->getImage().isEmpty()) {
        const auto albedoMapPath = materialName + "_albedo.png";

        mtlFile << "\tmap_Kd " << albedoMapPath << '\n';
        ImageFormat::save(albedoMapPath, matCT->getAlbedoMap()->getImage(), true);
      }

      if (matCT->getNormalMap() && !matCT->getNormalMap()->getImage().isEmpty()) {
        const auto normalMapPath = materialName + "_normal.png";

        mtlFile << "\tnorm " << normalMapPath << '\n';
        ImageFormat::save(normalMapPath, matCT->getNormalMap()->getImage(), true);
      }

      if (matCT->getMetallicMap() && !matCT->getMetallicMap()->getImage().isEmpty()) {
        const auto metallicMapPath = materialName + "_metallic.png";

        mtlFile << "\tmap_Pm " << metallicMapPath << '\n';
        ImageFormat::save(metallicMapPath, matCT->getMetallicMap()->getImage(), true);
      }

      if (matCT->getRoughnessMap() && !matCT->getRoughnessMap()->getImage().isEmpty()) {
        const auto roughnessMapPath = materialName + "_roughness.png";

        mtlFile << "\tmap_Pr " << roughnessMapPath << '\n';
        ImageFormat::save(roughnessMapPath, matCT->getRoughnessMap()->getImage(), true);
      }

      if (matCT->getAmbientOcclusionMap() && !matCT->getAmbientOcclusionMap()->getImage().isEmpty()) {
        const auto ambOccMapPath = materialName + "_ambient_occlusion.png";

        mtlFile << "\tmap_Ka " << ambOccMapPath << '\n';
        ImageFormat::save(ambOccMapPath, matCT->getAmbientOcclusionMap()->getImage(), true);
      }
    } else {
      const auto* matBP = static_cast<MaterialBlinnPhong*>(material.get());

      mtlFile << "\tKa " << matBP->getAmbient()[0] << ' ' << matBP->getAmbient()[1] << ' ' << matBP->getAmbient()[2] << '\n';
      mtlFile << "\tKs " << matBP->getSpecular()[0] << ' ' << matBP->getSpecular()[1] << ' ' << matBP->getSpecular()[2] << '\n';
      mtlFile << "\tKe " << matBP->getEmissive()[0] << ' ' << matBP->getEmissive()[1] << ' ' << matBP->getEmissive()[2] << '\n';
      mtlFile << "\td  " << matBP->getTransparency() << '\n';

      if (matBP->getDiffuseMap() && !matBP->getDiffuseMap()->getImage().isEmpty()) {
        const auto diffuseMapPath = materialName + "_diffuse.png";

        mtlFile << "\tmap_Kd " << diffuseMapPath << '\n';
        ImageFormat::save(diffuseMapPath, matBP->getDiffuseMap()->getImage(), true);
      }

      if (matBP->getAmbientMap() && !matBP->getAmbientMap()->getImage().isEmpty()) {
        const auto ambientMapPath = materialName + "_ambient.png";

        mtlFile << "\tmap_Ka " << ambientMapPath << '\n';
        ImageFormat::save(ambientMapPath, matBP->getAmbientMap()->getImage(), true);
      }

      if (matBP->getSpecularMap() && !matBP->getSpecularMap()->getImage().isEmpty()) {
        const auto specularMapPath = materialName + "_specular.png";

        mtlFile << "\tmap_Ks " << specularMapPath << '\n';
        ImageFormat::save(specularMapPath, matBP->getSpecularMap()->getImage(), true);
      }

      if (matBP->getEmissiveMap() && !matBP->getEmissiveMap()->getImage().isEmpty()) {
        const auto emissiveMapPath = materialName + "_emissive.png";

        mtlFile << "\tmap_Ke " << emissiveMapPath << '\n';
        ImageFormat::save(emissiveMapPath, matBP->getEmissiveMap()->getImage(), true);
      }

      if (matBP->getTransparencyMap() && !matBP->getTransparencyMap()->getImage().isEmpty()) {
        const auto transparencyMapPath = materialName + "_transparency.png";

        mtlFile << "\tmap_d " << transparencyMapPath << '\n';
        ImageFormat::save(transparencyMapPath, matBP->getTransparencyMap()->getImage(), true);
      }

      if (matBP->getBumpMap() && !matBP->getBumpMap()->getImage().isEmpty()) {
        const auto bumpMapPath = materialName + "_bump.png";

        mtlFile << "\tmap_bump " << bumpMapPath << '\n';
        ImageFormat::save(bumpMapPath, matBP->getBumpMap()->getImage(), true);
      }
    }
  }
}

} // namespace

void save(const FilePath& filePath, const Mesh& mesh, const MeshRenderer* meshRenderer) {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Unable to create a mesh file as '" + filePath + "'; path to file must exist");

  file << "# OBJ file created with RaZ - https://github.com/Razakhel/RaZ\n\n";

  if (meshRenderer && !meshRenderer->getMaterials().empty()) {
    const std::string mtlFileName = filePath.recoverFileName(false) + ".mtl";
    const FilePath mtlFilePath    = filePath.recoverPathToFile() + mtlFileName;

    file << "mtllib " << mtlFilePath << "\n\n";

    saveMtl(mtlFilePath, meshRenderer->getMaterials());
  }

  std::map<Vec3f, std::size_t> posCorrespIndices;
  std::map<Vec2f, std::size_t> texCorrespIndices;
  std::map<Vec3f, std::size_t> normCorrespIndices;

  for (const Submesh& submesh : mesh.getSubmeshes()) {
    for (const Vertex& vertex : submesh.getVertices()) {
      if (posCorrespIndices.find(vertex.position) == posCorrespIndices.cend()) {
        file << "v " << vertex.position.x() << ' '
                     << vertex.position.y() << ' '
                     << vertex.position.z() << '\n';
        posCorrespIndices.emplace(vertex.position, posCorrespIndices.size() + 1);
      }

      if (texCorrespIndices.find(vertex.texcoords) == texCorrespIndices.cend()) {
        file << "vt " << vertex.texcoords.x() << ' '
                      << vertex.texcoords.y() << '\n';
        texCorrespIndices.emplace(vertex.texcoords, texCorrespIndices.size() + 1);
      }

      if (normCorrespIndices.find(vertex.normal) == normCorrespIndices.cend()) {
        file << "vn " << vertex.normal.x() << ' '
                      << vertex.normal.y() << ' '
                      << vertex.normal.z() << '\n';
        normCorrespIndices.emplace(vertex.normal, normCorrespIndices.size() + 1);
      }
    }
  }

  const std::string fileName = filePath.recoverFileName(false).toUtf8();

  for (std::size_t submeshIndex = 0; submeshIndex < mesh.getSubmeshes().size(); ++submeshIndex) {
    const Submesh& submesh = mesh.getSubmeshes()[submeshIndex];

    file << "\no " << fileName << '_' << submeshIndex << '\n';

    if (meshRenderer && !meshRenderer->getMaterials().empty())
      file << "usemtl " << fileName << '_' << meshRenderer->getSubmeshRenderers()[submeshIndex].getMaterialIndex() << '\n';

    for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
      file << "f ";

      // First vertex
      Vertex vertex = submesh.getVertices()[submesh.getTriangleIndices()[i]];

      auto posIndex  = posCorrespIndices.find(vertex.position)->second;
      auto texIndex  = texCorrespIndices.find(vertex.texcoords)->second;
      auto normIndex = normCorrespIndices.find(vertex.normal)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Second vertex
      vertex = submesh.getVertices()[submesh.getTriangleIndices()[i + 1]];

      posIndex  = posCorrespIndices.find(vertex.position)->second;
      texIndex  = texCorrespIndices.find(vertex.texcoords)->second;
      normIndex = normCorrespIndices.find(vertex.normal)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << ' ';

      // Third vertex
      vertex = submesh.getVertices()[submesh.getTriangleIndices()[i + 2]];

      posIndex  = posCorrespIndices.find(vertex.position)->second;
      texIndex  = texCorrespIndices.find(vertex.texcoords)->second;
      normIndex = normCorrespIndices.find(vertex.normal)->second;

      file << posIndex  << '/' << texIndex  << '/' << normIndex << '\n';
    }
  }
}

} // namespace Raz::ObjFormat
