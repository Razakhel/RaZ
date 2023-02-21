#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Quaternion.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerVectorTypes() {
  sol::state& state = getState();

  // Attempting to hash vectors can give an error: 'integer value will be misrepresented in lua'
  // This could be solved by defining SOL_ALL_INTEGER_VALUES_FIT to 1 (see https://github.com/ThePhD/sol2/issues/1178#issuecomment-833923813),
  //   but this gives signed integers thus can (and does) produce negative values
  // Unsigned 64-bit integers seem to be supported in Lua 5.3 and above, so there may (will) be a better solution
  // Manually hashing a vector should not be useful on Lua's side anyway

  {
    sol::usertype<Vec2f> vec2f = state.new_usertype<Vec2f>("Vec2f",
                                                           sol::constructors<Vec2f(),
                                                                             Vec2f(float),
                                                                             Vec2f(float, float),
                                                                             Vec2f(const Vec3f&)>());
    vec2f["x"]                    = PickConstOverload<>(&Vec2f::x);
    vec2f["y"]                    = PickConstOverload<>(&Vec2f::y);
    vec2f["dot"]                  = &Vec2f::dot;
    vec2f["reflect"]              = &Vec2f::reflect;
    vec2f["computeLength"]        = &Vec2f::computeLength;
    vec2f["computeSquaredLength"] = &Vec2f::computeSquaredLength;
    vec2f["normalize"]            = &Vec2f::normalize;
    vec2f["lerp"]                 = &Vec2f::lerp;
    vec2f["nlerp"]                = &Vec2f::nlerp;
    vec2f["strictlyEquals"]       = &Vec2f::strictlyEquals;
    vec2f.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec2f::operator-));
    vec2f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec2f&>(&Vec2f::operator+),
                                                                   PickOverload<float>(&Vec2f::operator+)));
    vec2f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec2f&>(&Vec2f::operator-),
                                                                      PickOverload<float>(&Vec2f::operator-)));
    vec2f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec2f&>(&Vec2f::operator*),
                                                                         PickOverload<float>(&Vec2f::operator*),
                                                                         PickOverload<const Vec2f&, const Mat2f&>(&::Raz::operator*<float, 2, 2>)));
    vec2f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec2f&>(&Vec2f::operator/),
                                                                   PickOverload<float>(&Vec2f::operator/)));
    vec2f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec2f::operator[]));
  }

  {
    sol::usertype<Vec3f> vec3f = state.new_usertype<Vec3f>("Vec3f",
                                                           sol::constructors<Vec3f(),
                                                                             Vec3f(float),
                                                                             Vec3f(float, float, float),
                                                                             Vec3f(const Vec2f&, float),
                                                                             Vec3f(const Vec4f&)>());
    vec3f["x"]                    = PickConstOverload<>(&Vec3f::x);
    vec3f["y"]                    = PickConstOverload<>(&Vec3f::y);
    vec3f["z"]                    = PickConstOverload<>(&Vec3f::z);
    vec3f["dot"]                  = &Vec3f::dot;
    vec3f["cross"]                = &Vec3f::cross;
    vec3f["reflect"]              = &Vec3f::reflect;
    vec3f["computeLength"]        = &Vec3f::computeLength;
    vec3f["computeSquaredLength"] = &Vec3f::computeSquaredLength;
    vec3f["normalize"]            = &Vec3f::normalize;
    vec3f["lerp"]                 = &Vec3f::lerp;
    vec3f["nlerp"]                = &Vec3f::nlerp;
    vec3f["strictlyEquals"]       = &Vec3f::strictlyEquals;
    vec3f.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec3f::operator-));
    vec3f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec3f&>(&Vec3f::operator+),
                                                                   PickOverload<float>(&Vec3f::operator+)));
    vec3f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec3f&>(&Vec3f::operator-),
                                                                      PickOverload<float>(&Vec3f::operator-)));
    vec3f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec3f&>(&Vec3f::operator*),
                                                                         PickOverload<float>(&Vec3f::operator*),
                                                                         PickOverload<const Vec3f&, const Mat3f&>(&::Raz::operator*<float, 3, 3>),
                                                                         PickOverload<const Vec3f&, const Quaternionf&>(&::Raz::operator*<float>)));
    vec3f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec3f&>(&Vec3f::operator/),
                                                                   PickOverload<float>(&Vec3f::operator/)));
    vec3f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec3f::operator[]));

    sol::table axis = state["Axis"].get_or_create<sol::table>();
    axis["X"]       = Axis::X;
    axis["Y"]       = Axis::Y;
    axis["Z"]       = Axis::Z;
  }

  {
    sol::usertype<Vec4f> vec4f = state.new_usertype<Vec4f>("Vec4f",
                                                           sol::constructors<Vec4f(),
                                                                             Vec4f(float),
                                                                             Vec4f(float, float, float, float),
                                                                             Vec4f(const Vec3f&, float)>());
    vec4f["x"]                    = PickConstOverload<>(&Vec4f::x);
    vec4f["y"]                    = PickConstOverload<>(&Vec4f::y);
    vec4f["z"]                    = PickConstOverload<>(&Vec4f::z);
    vec4f["w"]                    = PickConstOverload<>(&Vec4f::w);
    vec4f["dot"]                  = &Vec4f::dot;
    vec4f["reflect"]              = &Vec4f::reflect;
    vec4f["computeLength"]        = &Vec4f::computeLength;
    vec4f["computeSquaredLength"] = &Vec4f::computeSquaredLength;
    vec4f["normalize"]            = &Vec4f::normalize;
    vec4f["lerp"]                 = &Vec4f::lerp;
    vec4f["nlerp"]                = &Vec4f::nlerp;
    vec4f["strictlyEquals"]       = &Vec4f::strictlyEquals;
    vec4f.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec4f::operator-));
    vec4f.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec4f&>(&Vec4f::operator+),
                                                                   PickOverload<float>(&Vec4f::operator+)));
    vec4f.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec4f&>(&Vec4f::operator-),
                                                                      PickOverload<float>(&Vec4f::operator-)));
    vec4f.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec4f&>(&Vec4f::operator*),
                                                                         PickOverload<float>(&Vec4f::operator*),
                                                                         PickOverload<const Vec4f&, const Mat4f&>(&::Raz::operator*<float, 4, 4>)));
    vec4f.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec4f&>(&Vec4f::operator/),
                                                                   PickOverload<float>(&Vec4f::operator/)));
    vec4f.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec4f::operator[]));
  }
}

} // namespace Raz
