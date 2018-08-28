#pragma once

#ifndef RAZ_SHAPE_HPP
#define RAZ_SHAPE_HPP

#include "RaZ/Math/Vector.hpp"

namespace Raz {

struct Triangle {
  Triangle() = default;
  Triangle(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos)
    : firstPos{ firstPos }, secondPos{ secondPos }, thirdPos{ thirdPos } {}

  Vec3f firstPos {};
  Vec3f secondPos {};
  Vec3f thirdPos {};
};

struct Quad {
  Quad() = default;
  Quad(const Vec3f& leftTopPos, const Vec3f& rightTopPos, const Vec3f& rightBottomPos, const Vec3f& leftBottomPos)
    : leftTopPos{ leftTopPos }, rightTopPos{ rightTopPos }, rightBottomPos{ rightBottomPos }, leftBottomPos{ leftBottomPos } {}

  Vec3f leftTopPos {};
  Vec3f rightTopPos {};
  Vec3f rightBottomPos {};
  Vec3f leftBottomPos {};
};

struct AABB {
  AABB() = default;
  AABB(const Vec3f& rightTopFrontPos, const Vec3f& leftBottomBackPos)
    : rightTopFrontPos{ rightTopFrontPos }, leftBottomBackPos{ leftBottomBackPos } {}

  Vec3f rightTopFrontPos {};
  Vec3f leftBottomBackPos {};
};

} // namespace Raz

#endif // RAZ_SHAPE_HPP
