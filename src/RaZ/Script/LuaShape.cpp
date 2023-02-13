#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/Shape.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerShapeTypes() {
  sol::state& state = getState();

  {
    sol::usertype<AABB> aabb = state.new_usertype<AABB>("AABB",
                                                        sol::constructors<AABB(const Vec3f&, const Vec3f&)>(),
                                                        sol::base_classes, sol::bases<Shape>());
    aabb["getMinPosition"]     = &AABB::getMinPosition;
    aabb["getMaxPosition"]     = &AABB::getMaxPosition;
    aabb["computeHalfExtents"] = &AABB::computeHalfExtents;
  }

  {
    sol::usertype<Line> line = state.new_usertype<Line>("Line",
                                                        sol::constructors<Line(const Vec3f&, const Vec3f&)>(),
                                                        sol::base_classes, sol::bases<Shape>());
    line["getBeginPos"]          = &Line::getBeginPos;
    line["getEndPos"]            = &Line::getEndPos;
    line["computeLength"]        = &Line::computeLength;
    line["computeSquaredLength"] = &Line::computeSquaredLength;
  }

  {
    sol::usertype<OBB> obb = state.new_usertype<OBB>("OBB",
                                                     sol::constructors<OBB(const Vec3f&, const Vec3f&),
                                                                       OBB(const Vec3f&, const Vec3f&, const Quaternionf&),
                                                                       OBB(const AABB&),
                                                                       OBB(const AABB&, const Quaternionf&)>(),
                                                     sol::base_classes, sol::bases<Shape>());
    obb["getMinPosition"]     = &OBB::getMinPosition;
    obb["getMaxPosition"]     = &OBB::getMaxPosition;
    obb["getRotation"]        = &OBB::getRotation;
    obb["computeHalfExtents"] = &OBB::computeHalfExtents;
  }

  {
    sol::usertype<Plane> plane = state.new_usertype<Plane>("Plane",
                                                           sol::constructors<Plane(float),
                                                                             Plane(float, const Vec3f&),
                                                                             Plane(const Vec3f&),
                                                                             Plane(const Vec3f&, const Vec3f&),
                                                                             Plane(const Vec3f&, const Vec3f&, const Vec3f&)>(),
                                                           sol::base_classes, sol::bases<Shape>());
    plane["getDistance"] = &Plane::getDistance;
    plane["getNormal"]   = &Plane::getNormal;
  }

  {
    sol::usertype<Quad> quad = state.new_usertype<Quad>("Quad",
                                                        sol::constructors<Quad(const Vec3f&, const Vec3f&, const Vec3f&, const Vec3f&)>(),
                                                        sol::base_classes, sol::bases<Shape>());
    quad["getLeftTopPos"]     = &Quad::getLeftTopPos;
    quad["getRightTopPos"]    = &Quad::getRightTopPos;
    quad["getRightBottomPos"] = &Quad::getRightBottomPos;
    quad["getLeftBottomPos"]  = &Quad::getLeftBottomPos;
  }

  {
    sol::usertype<Shape> shape = state.new_usertype<Shape>("Shape", sol::no_constructor);
    shape["getType"]            = &Shape::getType;
    shape["contains"]           = &Shape::contains;
    shape["intersects"]         = sol::overload(PickOverload<const AABB&>(&Shape::intersects),
                                                PickOverload<const Line&>(&Shape::intersects),
                                                PickOverload<const OBB&>(&Shape::intersects),
                                                PickOverload<const Plane&>(&Shape::intersects),
                                                PickOverload<const Quad&>(&Shape::intersects),
                                                PickOverload<const Sphere&>(&Shape::intersects),
                                                PickOverload<const Triangle&>(&Shape::intersects),
                                                PickOverload<const Ray&, RayHit*>(&Shape::intersects));
    shape["translate"]          = &Shape::translate;
    shape["computeProjection"]  = &Shape::computeProjection;
    shape["computeCentroid"]    = &Shape::computeCentroid;
    shape["computeBoundingBox"] = &Shape::computeBoundingBox;
  }

  {
    sol::usertype<Sphere> sphere = state.new_usertype<Sphere>("Sphere",
                                                              sol::constructors<Sphere(const Vec3f&, float)>(),
                                                              sol::base_classes, sol::bases<Shape>());
    sphere["getCenter"] = &Sphere::getCenter;
    sphere["getRadius"] = &Sphere::getRadius;
  }

  {
    sol::usertype<Triangle> triangle = state.new_usertype<Triangle>("Triangle",
                                                                    sol::constructors<Triangle(const Vec3f&, const Vec3f&, const Vec3f&)>(),
                                                                    sol::base_classes, sol::bases<Shape>());
    triangle["getFirstPos"]          = &Triangle::getFirstPos;
    triangle["getSecondPos"]         = &Triangle::getSecondPos;
    triangle["getThirdPos"]          = &Triangle::getThirdPos;
    triangle["computeNormal"]        = &Triangle::computeNormal;
    triangle["isCounterClockwise"]   = &Triangle::isCounterClockwise;
    triangle["makeCounterClockwise"] = &Triangle::makeCounterClockwise;
  }

  state.new_enum<ShapeType>("ShapeType", {
    { "AABB",     ShapeType::AABB },
    { "LINE",     ShapeType::LINE },
    { "OBB",      ShapeType::OBB },
    { "PLANE",    ShapeType::PLANE },
    { "QUAD",     ShapeType::QUAD },
    { "SPHERE",   ShapeType::SPHERE },
    { "TRIANGLE", ShapeType::TRIANGLE }
  });
}

} // namespace Raz
