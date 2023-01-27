#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Ray.hpp"
#include "RaZ/Utils/Shape.hpp"
#include "RaZ/Utils/StrUtils.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerUtilsTypes() {
  sol::state& state = getState();

  {
    sol::usertype<FilePath> filePath = state.new_usertype<FilePath>("FilePath",
                                                                    sol::constructors<FilePath(),
                                                                                      FilePath(const char*),
                                                                                      FilePath(const std::string&)>());
    filePath["isEmpty"]           = &FilePath::isEmpty;
    filePath["recoverPathToFile"] = PickOverload<>(&FilePath::recoverPathToFile);
    filePath["recoverFileName"]   = sol::overload([] (const FilePath& f) { return f.recoverFileName(); },
                                                  PickOverload<bool>(&FilePath::recoverFileName));
    filePath["recoverExtension"]  = PickOverload<>(&FilePath::recoverExtension);
    filePath["toUtf8"]            = &FilePath::toUtf8;
    filePath.set_function(sol::meta_function::concatenation, sol::overload(PickOverload<const char*>(&FilePath::operator+),
                                                                           PickOverload<const std::string&>(&FilePath::operator+),
                                                                           PickOverload<const FilePath&>(&FilePath::operator+),
                                                                           [] (const char* s, const FilePath& p) { return s + p; },
                                                                           [] (const std::string& s, const FilePath& p) { return s + p; }));
  }

  {
    sol::table fileUtils  = state["FileUtils"].get_or_create<sol::table>();
    fileUtils["readFile"] = &FileUtils::readFile;
  }

  {
    sol::table logger              = state["Logger"].get_or_create<sol::table>();
    logger["setLoggingLevel"]      = &Logger::setLoggingLevel;
    logger["setLoggingFunction"]   = &Logger::setLoggingFunction;
    logger["resetLoggingFunction"] = &Logger::resetLoggingFunction;
    logger["error"]                = &Logger::error;
    logger["warn"]                 = &Logger::warn;
    logger["info"]                 = &Logger::info;
    logger["debug"]                = sol::overload(PickOverload<const char*>(&Logger::debug),
                                                   PickOverload<const std::string&>(&Logger::debug));

    state.new_enum<LoggingLevel>("LoggingLevel", {
      { "NONE",    LoggingLevel::NONE },
      { "ERROR",   LoggingLevel::ERROR },
      { "WARNING", LoggingLevel::WARNING },
      { "INFO",    LoggingLevel::INFO },
      { "DEBUG",   LoggingLevel::DEBUG },
      { "ALL",     LoggingLevel::ALL }
    });
  }

  {
    {
      sol::usertype<RayHit> rayHit = state.new_usertype<RayHit>("RayHit",
                                                                sol::constructors<RayHit()>());
      rayHit["position"] = &RayHit::position;
      rayHit["normal"]   = &RayHit::normal;
      rayHit["distance"] = &RayHit::distance;
    }

    {
      sol::usertype<Ray> ray = state.new_usertype<Ray>("Ray",
                                                       sol::constructors<Ray(const Vec3f&, const Vec3f&)>());
      ray["getOrigin"]           = &Ray::getOrigin;
      ray["getDirection"]        = &Ray::getDirection;
      ray["getInverseDirection"] = &Ray::getInverseDirection;
      ray["intersects"]          = sol::overload([] (const Ray& r, const Vec3f& p) { r.intersects(p); },
                                                 PickOverload<const Vec3f&, RayHit*>(&Ray::intersects),
                                                 [] (const Ray& r, const AABB& s) { r.intersects(s); },
                                                 PickOverload<const AABB&, RayHit*>(&Ray::intersects),
                                                 //[] (const Ray& r, const Line& s) { r.intersects(s); },
                                                 //PickOverload<const Line&, RayHit*>(&Ray::intersects),
                                                 //[] (const Ray& r, const OBB& s) { r.intersects(s); },
                                                 //PickOverload<const OBB&, RayHit*>(&Ray::intersects),
                                                 [] (const Ray& r, const Plane& s) { r.intersects(s); },
                                                 PickOverload<const Plane&, RayHit*>(&Ray::intersects),
                                                 //[] (const Ray& r, const Quad& s) { r.intersects(s); },
                                                 //PickOverload<const Quad&, RayHit*>(&Ray::intersects),
                                                 [] (const Ray& r, const Sphere& s) { r.intersects(s); },
                                                 PickOverload<const Sphere&, RayHit*>(&Ray::intersects),
                                                 [] (const Ray& r, const Triangle& s) { r.intersects(s); },
                                                 PickOverload<const Triangle&, RayHit*>(&Ray::intersects));
      ray["computeProjection"]   = &Ray::computeProjection;
    }
  }

  // Shapes
  {
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
                                                                         OBB(const Vec3f&, const Vec3f&, const Mat3f),
                                                                         OBB(const AABB&),
                                                                         OBB(const AABB&, const Mat3f&)>(),
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

  {
    sol::table strUtils         = state["StrUtils"].get_or_create<sol::table>();
    strUtils["startsWith"]      = PickOverload<const std::string&, const std::string&>(&StrUtils::startsWith);
    strUtils["endsWith"]        = PickOverload<const std::string&, const std::string&>(&StrUtils::endsWith);
    strUtils["toLowercaseCopy"] = PickOverload<std::string>(&StrUtils::toLowercaseCopy);
    strUtils["toUppercaseCopy"] = PickOverload<std::string>(&StrUtils::toUppercaseCopy);
    strUtils["trimLeftCopy"]    = PickOverload<std::string>(&StrUtils::trimLeftCopy);
    strUtils["trimRightCopy"]   = PickOverload<std::string>(&StrUtils::trimRightCopy);
    strUtils["trimCopy"]        = PickOverload<std::string>(&StrUtils::trimCopy);
    strUtils["split"]           = PickOverload<std::string, char>(&StrUtils::split);
  }
}

} // namespace Raz
