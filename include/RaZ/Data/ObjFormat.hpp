#pragma once

#ifndef RAZ_OBJFORMAT_HPP
#define RAZ_OBJFORMAT_HPP

#include <utility>

namespace Raz {

class FilePath;
class Mesh;
class MeshRenderer;

namespace ObjFormat {

/// Loads an OBJ mesh from a file.
/// \param filePath File from which to load the mesh.
/// \return Pair containing respectively the mesh's data (vertices & indices) and rendering information (materials, textures, ...).
std::pair<Mesh, MeshRenderer> load(const FilePath& filePath);

/// Saves an OBJ mesh to a file.
/// \param filePath File to which to save the mesh.
/// \param mesh Mesh to export data from.
/// \param meshRenderer Optional mesh renderer to export material & textures from.
void save(const FilePath& filePath, const Mesh& mesh, const MeshRenderer* meshRenderer = nullptr);

} // namespace ObjFormat

} // namespace Raz

#endif // RAZ_OBJFORMAT_HPP
