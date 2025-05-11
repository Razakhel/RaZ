#pragma once

#ifndef RAZ_MARCHINGSQUARES_HPP
#define RAZ_MARCHINGSQUARES_HPP

namespace Raz {

template <typename T>
class Grid2;
using Grid2b = Grid2<bool>;
class Mesh;

namespace MarchingSquares {

/// Computes a mesh using the [marching squares](https://en.wikipedia.org/wiki/Marching_squares) algorithm.
/// \note Currently, contour lines are represented as triangles, each having the first two points duplicated.
/// \param grid 2D grid to create the mesh from.
/// \return Mesh representing the contour corresponding to the input grid.
Mesh compute(const Grid2b& grid);

} // namespace MarchingSquares

} // namespace Raz

#endif // RAZ_MARCHINGSQUARES_HPP
