#include "TestUtils.hpp"

#include "RaZ/Render/Window.hpp"

namespace TestUtils {

Raz::Window& getWindow() {
  static Raz::Window window(320, 180, "RaZ - Unit tests", Raz::WindowSetting::INVISIBLE);
  return window;
}

} //namespace TestUtils
