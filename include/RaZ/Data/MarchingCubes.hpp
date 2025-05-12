#pragma once

#ifndef RAZ_MARCHINGCUBES_HPP
#define RAZ_MARCHINGCUBES_HPP

namespace Raz {

template <typename T>
class Grid3;
using Grid3b = Grid3<bool>;
class Mesh;

namespace MarchingCubes {

/// Computes a mesh using the [marching cubes](https://en.wikipedia.org/wiki/Marching_cubes) algorithm.
/// \param grid 3D grid to create the mesh from.
/// \return Mesh representing the contour corresponding to the input grid.
Mesh compute(const Grid3b& grid);

} // namespace MarchingCubes

} // namespace Raz

#endif // RAZ_MARCHINGCUBES_HPP
