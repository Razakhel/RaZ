#pragma once

#ifndef RAZ_BVHFORMAT_HPP
#define RAZ_BVHFORMAT_HPP

namespace Raz {

class FilePath;
template <typename T> class Graph;
using Skeleton = Graph<class SkeletonJoint>;

namespace BvhFormat {

/// Loads a skeleton from a BVH file.
/// \param filePath File from which to load the skeleton.
/// \return Loaded skeleton's data.
Skeleton load(const FilePath& filePath);

} // namespace BvhFormat

} // namespace Raz

#endif // RAZ_BVHFORMAT_HPP
