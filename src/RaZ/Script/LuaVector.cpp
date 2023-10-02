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
    sol::usertype<Vec2b> vec2b = state.new_usertype<Vec2b>("Vec2b",
                                                           sol::constructors<Vec2b(),
                                                                             Vec2b(uint8_t),
                                                                             Vec2b(uint8_t, uint8_t),
                                                                             Vec2b(const Vec3b&)>());
    vec2b["x"]                    = PickConstOverload<>(&Vec2b::x);
    vec2b["y"]                    = PickConstOverload<>(&Vec2b::y);
    vec2b["dot"]                  = &Vec2b::dot<>;
    vec2b["computeSquaredLength"] = &Vec2b::computeSquaredLength<>;
    vec2b["computeLength"]        = &Vec2b::computeLength<>;
    vec2b["normalize"]            = &Vec2b::normalize<>;
    vec2b["lerp"]                 = &Vec2b::lerp<>;
    vec2b["lerpf"]                = &Vec2b::lerp<float>;
    vec2b["nlerp"]                = &Vec2b::nlerp<>;
    vec2b["strictlyEquals"]       = &Vec2b::strictlyEquals;
    vec2b.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec2b::operator-));
    vec2b.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec2b&>(&Vec2b::operator+),
                                                                   PickOverload<uint8_t>(&Vec2b::operator+)));
    vec2b.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec2b&>(&Vec2b::operator-),
                                                                      PickOverload<uint8_t>(&Vec2b::operator-)));
    vec2b.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec2b&>(&Vec2b::operator*),
                                                                         PickOverload<uint8_t>(&Vec2b::operator*)));
    vec2b.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec2b&>(&Vec2b::operator/),
                                                                   PickOverload<uint8_t>(&Vec2b::operator/)));
    vec2b.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec2b::operator[]));
  }

  {
    sol::usertype<Vec3b> vec3b = state.new_usertype<Vec3b>("Vec3b",
                                                           sol::constructors<Vec3b(),
                                                                             Vec3b(uint8_t),
                                                                             Vec3b(uint8_t, uint8_t, uint8_t),
                                                                             Vec3b(const Vec2b&, uint8_t),
                                                                             Vec3b(const Vec4b&)>());
    vec3b["x"]                    = PickConstOverload<>(&Vec3b::x);
    vec3b["y"]                    = PickConstOverload<>(&Vec3b::y);
    vec3b["z"]                    = PickConstOverload<>(&Vec3b::z);
    vec3b["dot"]                  = &Vec3b::dot<>;
    vec3b["computeSquaredLength"] = &Vec3b::computeSquaredLength<>;
    vec3b["computeLength"]        = &Vec3b::computeLength<>;
    vec3b["normalize"]            = &Vec3b::normalize<>;
    vec3b["lerp"]                 = &Vec3b::lerp<>;
    vec3b["lerpf"]                = &Vec3b::lerp<float>;
    vec3b["nlerp"]                = &Vec3b::nlerp<>;
    vec3b["strictlyEquals"]       = &Vec3b::strictlyEquals;
    vec3b.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec3b::operator-));
    vec3b.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec3b&>(&Vec3b::operator+),
                                                                   PickOverload<uint8_t>(&Vec3b::operator+)));
    vec3b.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec3b&>(&Vec3b::operator-),
                                                                      PickOverload<uint8_t>(&Vec3b::operator-)));
    vec3b.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec3b&>(&Vec3b::operator*),
                                                                         PickOverload<uint8_t>(&Vec3b::operator*)));
    vec3b.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec3b&>(&Vec3b::operator/),
                                                                   PickOverload<uint8_t>(&Vec3b::operator/)));
    vec3b.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec3b::operator[]));
  }

  {
    sol::usertype<Vec4b> vec4b = state.new_usertype<Vec4b>("Vec4b",
                                                           sol::constructors<Vec4b(),
                                                                             Vec4b(uint8_t),
                                                                             Vec4b(uint8_t, uint8_t, uint8_t, uint8_t),
                                                                             Vec4b(const Vec3b&, uint8_t)>());
    vec4b["x"]                    = PickConstOverload<>(&Vec4b::x);
    vec4b["y"]                    = PickConstOverload<>(&Vec4b::y);
    vec4b["z"]                    = PickConstOverload<>(&Vec4b::z);
    vec4b["w"]                    = PickConstOverload<>(&Vec4b::w);
    vec4b["dot"]                  = &Vec4b::dot<>;
    vec4b["computeSquaredLength"] = &Vec4b::computeSquaredLength<>;
    vec4b["computeLength"]        = &Vec4b::computeLength<>;
    vec4b["normalize"]            = &Vec4b::normalize<>;
    vec4b["lerp"]                 = &Vec4b::lerp<>;
    vec4b["lerpf"]                = &Vec4b::lerp<float>;
    vec4b["nlerp"]                = &Vec4b::nlerp<>;
    vec4b["strictlyEquals"]       = &Vec4b::strictlyEquals;
    vec4b.set_function(sol::meta_function::unary_minus, PickOverload<>(&Vec4b::operator-));
    vec4b.set_function(sol::meta_function::addition, sol::overload(PickOverload<const Vec4b&>(&Vec4b::operator+),
                                                                   PickOverload<uint8_t>(&Vec4b::operator+)));
    vec4b.set_function(sol::meta_function::subtraction, sol::overload(PickOverload<const Vec4b&>(&Vec4b::operator-),
                                                                      PickOverload<uint8_t>(&Vec4b::operator-)));
    vec4b.set_function(sol::meta_function::multiplication, sol::overload(PickOverload<const Vec4b&>(&Vec4b::operator*),
                                                                         PickOverload<uint8_t>(&Vec4b::operator*)));
    vec4b.set_function(sol::meta_function::division, sol::overload(PickOverload<const Vec4b&>(&Vec4b::operator/),
                                                                   PickOverload<uint8_t>(&Vec4b::operator/)));
    vec4b.set_function(sol::meta_function::index, PickConstOverload<std::size_t>(&Vec4b::operator[]));
  }

  {
    sol::usertype<Vec2f> vec2f = state.new_usertype<Vec2f>("Vec2f",
                                                           sol::constructors<Vec2f(),
                                                                             Vec2f(float),
                                                                             Vec2f(float, float),
                                                                             Vec2f(const Vec3f&)>());
    vec2f["x"]                    = PickConstOverload<>(&Vec2f::x);
    vec2f["y"]                    = PickConstOverload<>(&Vec2f::y);
    vec2f["dot"]                  = &Vec2f::dot<>;
    vec2f["reflect"]              = &Vec2f::reflect;
    vec2f["computeSquaredLength"] = &Vec2f::computeSquaredLength<>;
    vec2f["computeLength"]        = &Vec2f::computeLength<>;
    vec2f["normalize"]            = &Vec2f::normalize<>;
    vec2f["lerp"]                 = &Vec2f::lerp<>;
    vec2f["nlerp"]                = &Vec2f::nlerp<>;
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
    vec3f["dot"]                  = &Vec3f::dot<>;
    vec3f["cross"]                = &Vec3f::cross;
    vec3f["reflect"]              = &Vec3f::reflect;
    vec3f["computeSquaredLength"] = &Vec3f::computeSquaredLength<>;
    vec3f["computeLength"]        = &Vec3f::computeLength<>;
    vec3f["normalize"]            = &Vec3f::normalize<>;
    vec3f["lerp"]                 = &Vec3f::lerp<>;
    vec3f["nlerp"]                = &Vec3f::nlerp<>;
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
    vec4f["dot"]                  = &Vec4f::dot<>;
    vec4f["reflect"]              = &Vec4f::reflect;
    vec4f["computeSquaredLength"] = &Vec4f::computeSquaredLength<>;
    vec4f["computeLength"]        = &Vec4f::computeLength<>;
    vec4f["normalize"]            = &Vec4f::normalize<>;
    vec4f["lerp"]                 = &Vec4f::lerp<>;
    vec4f["nlerp"]                = &Vec4f::nlerp<>;
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
