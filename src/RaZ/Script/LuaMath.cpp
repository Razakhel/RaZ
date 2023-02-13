#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/PerlinNoise.hpp"
#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FloatUtils.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerMathTypes() {
  sol::state& state = getState();

  // Angles
  {
    {
      sol::usertype<Degreesf> degreesf = state.new_usertype<Degreesf>("Degreesf",
                                                                      sol::constructors<Degreesf(float),
                                                                                        Degreesf(Radiansf)>());
      degreesf["value"] = &Degreesf::value;
      degreesf.set_function(sol::meta_function::unary_minus, [] (const Degreesf& deg) { return -deg; });
      degreesf.set_function(sol::meta_function::addition, &Degreesf::operator+<float>);
      degreesf.set_function(sol::meta_function::subtraction, &Degreesf::operator-<float>);
      degreesf.set_function(sol::meta_function::multiplication, &Degreesf::operator*<float>);
      degreesf.set_function(sol::meta_function::division, &Degreesf::operator/<float>);
    }

    {
      sol::usertype<Radiansf> radiansf = state.new_usertype<Radiansf>("Radiansf",
                                                                      sol::constructors<Radiansf(float),
                                                                                        Radiansf(Degreesf)>());
      radiansf["value"] = &Radiansf::value;
      radiansf.set_function(sol::meta_function::unary_minus, [] (const Radiansf& rad) { return -rad; });
      radiansf.set_function(sol::meta_function::addition, &Radiansf::operator+<float>);
      radiansf.set_function(sol::meta_function::subtraction, &Radiansf::operator-<float>);
      radiansf.set_function(sol::meta_function::multiplication, &Radiansf::operator*<float>);
      radiansf.set_function(sol::meta_function::division, &Radiansf::operator/<float>);
    }
  }

  {
    sol::table constants     = state["Constant"].get_or_create<sol::table>();
    constants["Pi"]          = Pi<float>;
    constants["GoldenRatio"] = GoldenRatio<float>;
  }

  {
    sol::table floatUtils        = state["FloatUtils"].get_or_create<sol::table>();
    floatUtils["areNearlyEqual"] = sol::overload([] (float v1, float v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<float, float, float>(&FloatUtils::areNearlyEqual<float, float>),
                                                 [] (double v1, double v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<double, double, double>(&FloatUtils::areNearlyEqual<double, double>),
                                                 [] (const Vec2f& v1, const Vec2f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Vec2f&, const Vec2f&, float>(&FloatUtils::areNearlyEqual<float, 2, float>),
                                                 [] (const Vec3f& v1, const Vec3f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Vec3f&, const Vec3f&, float>(&FloatUtils::areNearlyEqual<float, 3, float>),
                                                 [] (const Vec4f& v1, const Vec4f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Vec4f&, const Vec4f&, float>(&FloatUtils::areNearlyEqual<float, 4, float>),
                                                 [] (const Mat2f& v1, const Mat2f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Mat2f&, const Mat2f&, float>(&FloatUtils::areNearlyEqual<float, 2, 2, float>),
                                                 [] (const Mat3f& v1, const Mat3f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Mat3f&, const Mat3f&, float>(&FloatUtils::areNearlyEqual<float, 3, 3, float>),
                                                 [] (const Mat4f& v1, const Mat4f& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Mat4f&, const Mat4f&, float>(&FloatUtils::areNearlyEqual<float, 4, 4, float>),
                                                 [] (const Quaternionf& v1, const Quaternionf& v2) { return FloatUtils::areNearlyEqual(v1, v2); },
                                                 PickOverload<const Quaternionf&, const Quaternionf&, float>(&FloatUtils::areNearlyEqual<float>));
  }

  {
    sol::table mathUtils      = state["MathUtils"].get_or_create<sol::table>();
    mathUtils["lerp"]         = sol::overload(PickOverload<float, float, float>(&MathUtils::lerp<float, float>),
                                              PickOverload<const Vec2f&, const Vec2f&, float>(&MathUtils::lerp<float, 2, float>),
                                              PickOverload<const Vec3f&, const Vec3f&, float>(&MathUtils::lerp<float, 3, float>),
                                              PickOverload<const Vec4f&, const Vec4f&, float>(&MathUtils::lerp<float, 4, float>));
    mathUtils["smoothstep"]   = sol::overload(PickOverload<float>(&MathUtils::smoothstep<float>),
                                              PickOverload<float, float, float>(&MathUtils::smoothstep<float>));
    mathUtils["smootherstep"] = sol::overload(PickOverload<float>(&MathUtils::smootherstep<float>),
                                              PickOverload<float, float, float>(&MathUtils::smootherstep<float>));
  }

  // Attempting to hash matrices and vectors can give an error: 'integer value will be misrepresented in lua'
  // This could be solved by defining SOL_ALL_INTEGER_VALUES_FIT to 1 (see https://github.com/ThePhD/sol2/issues/1178#issuecomment-833923813),
  //   but this gives signed integers thus can (and does) produce negative values
  // Unsigned 64-bit integers seem to be supported in Lua 5.3 and above, so there may (will) be a better solution
  // Manually hashing an object should not be useful on Lua's side anyway

  // Matrices
  {
    {
      sol::usertype<Mat2f> mat2f = state.new_usertype<Mat2f>("Mat2f",
                                                             sol::constructors<Mat2f(),
                                                                               Mat2f(float, float,
                                                                                     float, float),
                                                                               Mat2f(const Mat3f&)>());
      mat2f["getElement"]         = PickConstOverload<std::size_t, std::size_t>(&Mat2f::getElement);
      mat2f["identity"]           = &Mat2f::identity;
      mat2f["fromRows"]           = &Mat2f::fromRows<const Vec2f&, const Vec2f&>;
      mat2f["fromColumns"]        = &Mat2f::fromColumns<const Vec2f&, const Vec2f&>;
      mat2f["transpose"]          = &Mat2f::transpose;
      mat2f["computeDeterminant"] = &Mat2f::computeDeterminant;
      mat2f["inverse"]            = &Mat2f::inverse;
      mat2f["recoverRow"]         = &Mat2f::recoverRow;
      mat2f["recoverColumn"]      = &Mat2f::recoverColumn;
      mat2f["strictlyEquals"]     = &Mat2f::strictlyEquals;
      mat2f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Mat2f&>(&Mat2f::operator+),
                                                                     PickOverload<float>(&Mat2f::operator+)));
      mat2f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Mat2f&>(&Mat2f::operator-),
                                                                        PickOverload<float>(&Mat2f::operator-)));
      mat2f.set_function(sol::meta_function::modulus, &Mat2f::operator%);
      mat2f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<float>(&Mat2f::operator*),
                                                                           [] (const Mat2f& mat1, const Mat2f& mat2) { return mat1 * mat2; },
                                                                           PickOverload<const Mat2f&, const Vec2f&>(&::Raz::operator*<float, 2, 2>)));
      mat2f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Mat2f&>(&Mat2f::operator/),
                                                                     PickOverload<float>(&Mat2f::operator/)));
      mat2f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Mat2f::operator[]));
    }

    {
      sol::usertype<Mat3f> mat3f = state.new_usertype<Mat3f>("Mat3f",
                                                             sol::constructors<Mat3f(),
                                                                               Mat3f(float, float, float,
                                                                                     float, float, float,
                                                                                     float, float, float),
                                                                               Mat3f(const Mat2f&),
                                                                               Mat3f(const Mat4f&)>());
      mat3f["getElement"]         = PickConstOverload<std::size_t, std::size_t>(&Mat3f::getElement);
      mat3f["identity"]           = &Mat3f::identity;
      mat3f["fromRows"]           = &Mat3f::fromRows<const Vec3f&, const Vec3f&, const Vec3f&>;
      mat3f["fromColumns"]        = &Mat3f::fromColumns<const Vec3f&, const Vec3f&, const Vec3f&>;
      mat3f["transpose"]          = &Mat3f::transpose;
      mat3f["computeDeterminant"] = &Mat3f::computeDeterminant;
      mat3f["inverse"]            = &Mat3f::inverse;
      mat3f["recoverRow"]         = &Mat3f::recoverRow;
      mat3f["recoverColumn"]      = &Mat3f::recoverColumn;
      mat3f["strictlyEquals"]     = &Mat3f::strictlyEquals;
      mat3f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Mat3f&>(&Mat3f::operator+),
                                                                     PickOverload<float>(&Mat3f::operator+)));
      mat3f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Mat3f&>(&Mat3f::operator-),
                                                                        PickOverload<float>(&Mat3f::operator-)));
      mat3f.set_function(sol::meta_function::modulus, &Mat3f::operator%);
      mat3f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<float>(&Mat3f::operator*),
                                                                           [] (const Mat3f& mat1, const Mat3f& mat2) { return mat1 * mat2; },
                                                                           PickOverload<const Mat3f&, const Vec3f&>(&::Raz::operator*<float, 3, 3>)));
      mat3f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Mat3f&>(&Mat3f::operator/),
                                                                     PickOverload<float>(&Mat3f::operator/)));
      mat3f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Mat3f::operator[]));
    }

    {
      sol::usertype<Mat4f> mat4f = state.new_usertype<Mat4f>("Mat4f",
                                                             sol::constructors<Mat4f(),
                                                                               Mat4f(float, float, float, float,
                                                                                     float, float, float, float,
                                                                                     float, float, float, float,
                                                                                     float, float, float, float),
                                                                               Mat4f(const Mat3f&)>());
      mat4f["getElement"]         = PickConstOverload<std::size_t, std::size_t>(&Mat4f::getElement);
      mat4f["identity"]           = &Mat4f::identity;
      mat4f["fromRows"]           = &Mat4f::fromRows<const Vec4f&, const Vec4f&, const Vec4f&, const Vec4f&>;
      mat4f["fromColumns"]        = &Mat4f::fromColumns<const Vec4f&, const Vec4f&, const Vec4f&, const Vec4f&>;
      mat4f["transpose"]          = &Mat4f::transpose;
      mat4f["computeDeterminant"] = &Mat4f::computeDeterminant;
      mat4f["inverse"]            = &Mat4f::inverse;
      mat4f["recoverRow"]         = &Mat4f::recoverRow;
      mat4f["recoverColumn"]      = &Mat4f::recoverColumn;
      mat4f["strictlyEquals"]     = &Mat4f::strictlyEquals;
      mat4f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Mat4f&>(&Mat4f::operator+),
                                                                     PickOverload<float>(&Mat4f::operator+)));
      mat4f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Mat4f&>(&Mat4f::operator-),
                                                                        PickOverload<float>(&Mat4f::operator-)));
      mat4f.set_function(sol::meta_function::modulus, &Mat4f::operator%);
      mat4f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<float>(&Mat4f::operator*),
                                                                           [] (const Mat4f& mat1, const Mat4f& mat2) { return mat1 * mat2; },
                                                                           PickOverload<const Mat4f&, const Vec4f&>(&::Raz::operator*<float, 4, 4>)));
      mat4f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Mat4f&>(&Mat4f::operator/),
                                                                     PickOverload<float>(&Mat4f::operator/)));
      mat4f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Mat4f::operator[]));
    }
  }

  {
    sol::table perlinNoise   = state["PerlinNoise"].get_or_create<sol::table>();
    perlinNoise["compute1D"] = sol::overload([] (float x) { return PerlinNoise::compute1D(x); },
                                             [] (float x, uint8_t o) { return PerlinNoise::compute1D(x, o); },
                                             PickOverload<float, uint8_t, bool>(&PerlinNoise::compute1D));
    perlinNoise["compute2D"] = sol::overload([] (float x, float y) { return PerlinNoise::compute2D(x, y); },
                                             [] (float x, float y, uint8_t o) { return PerlinNoise::compute2D(x, y, o); },
                                             PickOverload<float, float, uint8_t, bool>(&PerlinNoise::compute2D));
    perlinNoise["compute3D"] = sol::overload([] (float x, float y, float z) { return PerlinNoise::compute3D(x, y, z); },
                                             [] (float x, float y, float z, uint8_t o) { return PerlinNoise::compute3D(x, y, z, o); },
                                             PickOverload<float, float, float, uint8_t, bool>(&PerlinNoise::compute3D));
  }

  {
    sol::usertype<Quaternionf> quaternionf = state.new_usertype<Quaternionf>("Quaternionf",
                                                                             sol::constructors<Quaternionf(float, float, float, float),
                                                                                               Quaternionf(Radiansf, const Vec3f&),
                                                                                               Quaternionf(Radiansf, float, float, float)>());
    quaternionf["w"]                  = &Quaternionf::w;
    quaternionf["x"]                  = &Quaternionf::x;
    quaternionf["y"]                  = &Quaternionf::y;
    quaternionf["z"]                  = &Quaternionf::z;
    quaternionf["identity"]           = &Quaternionf::identity;
    quaternionf["dot"]                = &Quaternionf::dot;
    quaternionf["computeNorm"]        = &Quaternionf::computeNorm;
    quaternionf["computeSquaredNorm"] = &Quaternionf::computeSquaredNorm;
    quaternionf["normalize"]          = &Quaternionf::normalize;
    quaternionf["lerp"]               = PickOverload<const Quaternionf&, float>(&Quaternionf::lerp);
    quaternionf["nlerp"]              = &Quaternionf::nlerp;
    quaternionf["slerp"]              = &Quaternionf::slerp;
    quaternionf["conjugate"]          = &Quaternionf::conjugate;
    quaternionf["inverse"]            = &Quaternionf::inverse;
    quaternionf["computeMatrix"]      = &Quaternionf::computeMatrix;
    quaternionf.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Quaternionf&>(&Quaternionf::operator*),
                                                                               PickOverload<const Quaternionf&, const Vec3f&>(&::Raz::operator*<float>)));
  }

  {
    sol::usertype<Transform> transform = state.new_usertype<Transform>("Transform",
                                                                       sol::constructors<Transform(),
                                                                                         Transform(const Vec3f&),
                                                                                         Transform(const Vec3f&, const Quaternionf&),
                                                                                         Transform(const Vec3f&, const Quaternionf&, const Vec3f&)>(),
                                                                       sol::base_classes, sol::bases<Component>());
    transform["position"]                 = sol::property(&Transform::getPosition, PickOverload<const Vec3f&>(&Transform::setPosition));
    transform["rotation"]                 = sol::property(&Transform::getRotation, PickOverload<const Quaternionf&>(&Transform::setRotation));
    transform["scaling"]                  = sol::property(&Transform::getScale, PickOverload<const Vec3f&>(&Transform::setScale));
    transform["move"]                     = sol::overload(PickOverload<const Vec3f&>(&Transform::move),
                                                          PickOverload<float, float, float>(&Transform::move));
    transform["translate"]                = sol::overload(PickOverload<const Vec3f&>(&Transform::translate),
                                                          PickOverload<float, float, float>(&Transform::translate));
    transform["rotate"]                   = sol::overload(PickOverload<const Quaternionf&>(&Transform::rotate),
                                                          PickOverload<Radiansf, const Vec3f&>(&Transform::rotate),
                                                          PickOverload<Radiansf, Radiansf>(&Transform::rotate),
                                                          PickOverload<Radiansf, Radiansf, Radiansf>(&Transform::rotate));
    transform["scale"]                    = sol::overload(PickOverload<const Vec3f&>(&Transform::scale),
                                                          PickOverload<float, float, float>(&Transform::scale),
                                                          PickOverload<float>(&Transform::scale));
    transform["computeTranslationMatrix"] = sol::overload([] (Transform& t) { return t.computeTranslationMatrix(); },
                                                          PickOverload<bool>(&Transform::computeTranslationMatrix));
    transform["computeTransformMatrix"]   = &Transform::computeTransformMatrix;
  }
}

} // namespace Raz
