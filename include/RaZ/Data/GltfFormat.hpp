#pragma once

#ifndef RAZ_GLTFFORMAT_HPP
#define RAZ_GLTFFORMAT_HPP

#include <utility>

namespace Raz {

class FilePath;
class Mesh;
class MeshRenderer;

namespace GltfFormat {

/// Loads a mesh from a glTF or GLB file.
/// \param filePath File from which to load the mesh.
/// \return Pair containing respectively the mesh's data (vertices & indices) and rendering information (materials, textures, ...).
std::pair<Mesh, MeshRenderer> load(const FilePath& filePath);

} // namespace GltfFormat

} // namespace Raz

#endif // RAZ_GLTFFORMAT_HPP
