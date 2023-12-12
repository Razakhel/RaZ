#include "RaZ/Data/FbxFormat.hpp"
#include "RaZ/Data/GltfFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/MeshFormat.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Data/OffFormat.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

namespace Raz::MeshFormat {

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (fileExt == "gltf" || fileExt == "glb") {
    return GltfFormat::load(filePath);
  } else if (fileExt == "obj") {
    return ObjFormat::load(filePath);
  } else if (fileExt == "off") {
    Mesh mesh = OffFormat::load(filePath);
    MeshRenderer meshRenderer(mesh);
    return { std::move(mesh), std::move(meshRenderer) };
  } else if (fileExt == "fbx") {
#if defined(RAZ_USE_FBX)
    return FbxFormat::load(filePath);
#else
    throw std::invalid_argument("[MeshFormat] FBX format unsupported; check that you enabled its usage when building RaZ (if on a supported platform).");
#endif
  }

  throw std::invalid_argument("[MeshFormat] Unsupported mesh file extension '" + fileExt + "' for loading.");
}

void save(const FilePath& filePath, const Mesh& mesh, const MeshRenderer* meshRenderer) {
  const std::string fileExt = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (fileExt == "obj")
    ObjFormat::save(filePath, mesh, meshRenderer);
  else
    throw std::invalid_argument("[MeshFormat] Unsupported mesh file extension '" + fileExt + "' for saving.");
}

} // namespace Raz::MeshFormat
