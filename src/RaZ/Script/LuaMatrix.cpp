#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerMatrixTypes() {
  sol::state& state = getState();

  // Attempting to hash matrices can give an error: 'integer value will be misrepresented in lua'
  // This could be solved by defining SOL_ALL_INTEGER_VALUES_FIT to 1 (see https://github.com/ThePhD/sol2/issues/1178#issuecomment-833923813),
  //   but this gives signed integers thus can (and does) produce negative values
  // Unsigned 64-bit integers seem to be supported in Lua 5.3 and above, so there may (will) be a better solution
  // Manually hashing a matrix should not be useful on Lua's side anyway

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

} // namespace Raz
