#pragma once

#ifndef RAZ_FBXFORMAT_HPP
#define RAZ_FBXFORMAT_HPP

#include <utility>

namespace Raz {

class FilePath;
class Mesh;
class MeshRenderer;

namespace FbxFormat {

/// Loads a mesh from an FBX file.
/// \param filePath File from which to load the mesh.
/// \return Pair containing respectively the mesh's data (vertices & indices) and rendering information (materials, textures, ...).
std::pair<Mesh, MeshRenderer> load(const FilePath& filePath);

} // namespace FbxFormat

} // namespace Raz

#endif // RAZ_FBXFORMAT_HPP
