#include "TestUtils.hpp"

#if !defined(RAZ_NO_WINDOW)
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"
#endif // RAZ_NO_WINDOW

#if !defined(RAZ_NO_LUA)
#include "RaZ/Script/LuaWrapper.hpp"
#endif // RAZ_NO_LUA

namespace TestUtils {

#if !defined(RAZ_NO_WINDOW)
Raz::Window& getWindow() {
  // Technically only a Window is needed, but it needs a parent RenderSystem containing it
  static Raz::RenderSystem renderSystem(320, 180, "RaZ - Unit tests", Raz::WindowSetting::INVISIBLE);
  return renderSystem.getWindow();
}
#endif // RAZ_NO_WINDOW

#if !defined(RAZ_NO_LUA)
bool executeLuaScript(const std::string& code) {
  const bool res = Raz::LuaWrapper::execute(code);
  Raz::LuaWrapper::collectGarbage();
  return res;
}
#endif // RAZ_NO_LUA

} //namespace TestUtils
