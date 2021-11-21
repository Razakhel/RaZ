#pragma once

#ifndef RAZ_OFFFORMAT_HPP
#define RAZ_OFFFORMAT_HPP

namespace Raz {

class FilePath;
class Mesh;

namespace OffFormat {

/// Loads a mesh from an OFF file.
/// \param filePath File from which to load the mesh.
/// \return Loaded mesh's data (vertices & indices).
Mesh load(const FilePath& filePath);

} // namespace OffFormat

} // namespace Raz

#endif // RAZ_OFFFORMAT_HPP
