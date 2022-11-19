#include "TestUtils.hpp"

#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"

namespace TestUtils {

Raz::Window& getWindow() {
  // Technically only a Window is needed, but it needs a parent RenderSystem containing it
  static Raz::RenderSystem renderSystem(320, 180, "RaZ - Unit tests", Raz::WindowSetting::INVISIBLE);
  return renderSystem.getWindow();
}

} //namespace TestUtils
