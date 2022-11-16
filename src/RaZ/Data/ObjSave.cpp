#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <fstream>
#include <map>

namespace Raz::ObjFormat {

namespace {

template <typename T, std::size_t Size = 1>
inline void writeAttribute(std::ofstream& file, std::string_view tag, const RenderShaderProgram& program, std::string_view uniformName) {
  if (!program.hasAttribute(uniformName.data()))
    return;

  file << '\t' << tag;

  if constexpr (Size == 1) {
    file << ' ' << program.getAttribute<T>(uniformName.data());
  } else {
    for (const T& value : program.getAttribute<Vector<T, Size>>(uniformName.data()).getData())
      file << ' ' << value;
  }

  file << '\n';
}

#if !defined(USE_OPENGL_ES) // Texture::recoverImage() is unavailable with OpenGL ES
inline void writeTexture(std::ofstream& file, std::string_view tag, const std::string& materialName, std::string_view suffix,
                         const RenderShaderProgram& program, std::string_view uniformName) {
  if (!program.hasTexture(uniformName.data()))
    return;

  const auto* texture = dynamic_cast<const Texture2D*>(&program.getTexture(uniformName.data()));

  if (texture == nullptr || texture->getWidth() == 0 || texture->getHeight() == 0 || texture->getColorspace() == TextureColorspace::INVALID)
    return;

  const std::string texturePath = materialName + '_' + suffix + ".png";

  file << '\t' << tag << ' ' << texturePath << '\n';
  ImageFormat::save(texturePath, texture->recoverImage(), true);
}
#endif

void saveMtl(const FilePath& mtlFilePath, const std::vector<Material>& materials) {
  std::ofstream mtlFile(mtlFilePath, std::ios_base::out | std::ios_base::binary);

  mtlFile << "# MTL file created with RaZ - https://github.com/Razakhel/RaZ\n";

  const std::string mtlFileName = mtlFilePath.recoverFileName(false).toUtf8();

  for (std::size_t matIndex = 0; matIndex < materials.size(); ++matIndex) {
    const RenderShaderProgram& matProgram = materials[matIndex].getProgram();
    const std::string materialName        = mtlFileName + '_' + std::to_string(matIndex);

    mtlFile << "\nnewmtl " << materialName << '\n';

    writeAttribute<float, 3>(mtlFile, "Kd", matProgram, MaterialAttribute::BaseColor);
    writeAttribute<float, 3>(mtlFile, "Ke", matProgram, MaterialAttribute::Emissive);
    writeAttribute<float, 3>(mtlFile, "Ka", matProgram, MaterialAttribute::Ambient);
    writeAttribute<float, 3>(mtlFile, "Ks", matProgram, MaterialAttribute::Specular);
    writeAttribute<float, 1>(mtlFile, "d",  matProgram, MaterialAttribute::Transparency);
    writeAttribute<float, 1>(mtlFile, "Pm", matProgram, MaterialAttribute::Metallic);
    writeAttribute<float, 1>(mtlFile, "Pr", matProgram, MaterialAttribute::Roughness);

#if !defined(USE_OPENGL_ES)
    writeTexture(mtlFile, "map_Kd",   materialName, "baseColor",    matProgram, MaterialTexture::BaseColor);
    writeTexture(mtlFile, "map_Ke",   materialName, "emissive",     matProgram, MaterialTexture::Emissive);
    writeTexture(mtlFile, "map_Ka",   materialName, "ambient",      matProgram, MaterialTexture::Ambient);
    writeTexture(mtlFile, "map_Ks",   materialName, "specular",     matProgram, MaterialTexture::Specular);
    writeTexture(mtlFile, "map_d",    materialName, "transparency", matProgram, MaterialTexture::Transparency);
    writeTexture(mtlFile, "map_bump", materialName, "bump",         matProgram, MaterialTexture::Bump);
    writeTexture(mtlFile, "norm",     materialName, "normal",       matProgram, MaterialTexture::Normal);
    writeTexture(mtlFile, "map_Pm",   materialName, "metallic",     matProgram, MaterialTexture::Metallic);
    writeTexture(mtlFile, "map_Pr",   materialName, "roughness",    matProgram, MaterialTexture::Roughness);
#endif
  }
}

} // namespace

void save(const FilePath& filePath, const Mesh& mesh, const MeshRenderer* meshRenderer) {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Unable to create an OBJ file as '" + filePath + "'; path to file must exist");

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
