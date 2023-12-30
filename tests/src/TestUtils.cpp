#include "TestUtils.hpp"

#if !defined(RAZ_NO_WINDOW)
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"
#endif // RAZ_NO_WINDOW

namespace TestUtils {

#if !defined(RAZ_NO_WINDOW)
Raz::Window& getWindow() {
  // Technically only a Window is needed, but it needs a parent RenderSystem containing it
  static Raz::RenderSystem renderSystem(320, 180, "RaZ - Unit tests", Raz::WindowSetting::INVISIBLE);
  return renderSystem.getWindow();
}
#endif // RAZ_NO_WINDOW

} //namespace TestUtils
