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

template <typename T, std::size_t Size = 1>
inline void writeAttribute(std::ofstream& file, std::string_view tag, const Material& material, std::string_view uniformName) {
  if (!material.hasAttribute(uniformName.data()))
    return;

  file << '\t' << tag;

  if constexpr (Size == 1) {
    file << ' ' << material.getAttribute<T>(uniformName.data());
  } else {
    for (const T& value : material.getAttribute<Vector<T, Size>>(uniformName.data()).getData())
      file << ' ' << value;
  }

  file << '\n';
}

inline void writeTexture(std::ofstream& file, std::string_view tag, const std::string& materialName, std::string_view suffix,
                         const Material& material, std::string_view uniformName) {
  if (!material.hasTexture(uniformName.data()))
    return;

  const Texture& texture = material.getTexture(uniformName.data());

  if (texture.getImage().isEmpty())
    return;

  const std::string texturePath = materialName + '_' + suffix + ".png";

  file << '\t' << tag << ' ' << texturePath << '\n';
  ImageFormat::save(texturePath, texture.getImage(), true);
}

void saveMtl(const FilePath& mtlFilePath, const std::vector<Material>& materials) {
  std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

  mtlFile << "# MTL file created with RaZ - https://github.com/Razakhel/RaZ\n";

  const std::string mtlFileName = mtlFilePath.recoverFileName(false).toUtf8();

  for (std::size_t matIndex = 0; matIndex < materials.size(); ++matIndex) {
    const Material& material       = materials[matIndex];
    const std::string materialName = mtlFileName + '_' + std::to_string(matIndex);

    mtlFile << "\nnewmtl " << materialName << '\n';

    writeAttribute<float, 3>(mtlFile, "Kd", material, "uniMaterial.baseColor");
    writeAttribute<float, 3>(mtlFile, "Ke", material, "uniMaterial.emissive");
    writeAttribute<float, 3>(mtlFile, "Ka", material, "uniMaterial.ambient");
    writeAttribute<float, 3>(mtlFile, "Ks", material, "uniMaterial.specular");
    writeAttribute<float, 1>(mtlFile, "d",  material, "uniMaterial.transparency");
    writeAttribute<float, 1>(mtlFile, "Pm", material, "uniMaterial.metallicFactor");
    writeAttribute<float, 1>(mtlFile, "Pr", material, "uniMaterial.roughnessFactor");

    writeTexture(mtlFile, "map_Kd",   materialName, "baseColor",    material, "uniMaterial.baseColorMap");
    writeTexture(mtlFile, "map_Ke",   materialName, "emissive",     material, "uniMaterial.emissiveMap");
    writeTexture(mtlFile, "map_Ka",   materialName, "ambient",      material, "uniMaterial.ambientMap");
    writeTexture(mtlFile, "map_Ks",   materialName, "specular",     material, "uniMaterial.specularMap");
    writeTexture(mtlFile, "map_d",    materialName, "transparency", material, "uniMaterial.transparencyMap");
    writeTexture(mtlFile, "map_bump", materialName, "bump",         material, "uniMaterial.bumpMap");
    writeTexture(mtlFile, "norm",     materialName, "normal",       material, "uniMaterial.normalMap");
    writeTexture(mtlFile, "map_Pm",   materialName, "metallic",     material, "uniMaterial.metallicMap");
    writeTexture(mtlFile, "map_Pr",   materialName, "roughness",    material, "uniMaterial.roughnessMap");
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
