#include "TestUtils.hpp"

#include "RaZ/Utils/Window.hpp"

namespace TestUtils {

Raz::Window& getWindow() {
  static Raz::Window window(320, 180, "RaZ - Unit tests", Raz::WindowSetting::INVISIBLE);
  return window;
}

} //namespace TestUtils
