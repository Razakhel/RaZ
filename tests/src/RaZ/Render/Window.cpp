#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Window resize", "[render][!mayfail]") { // Doesn't work on the Linux CI and apparently WSL
  // A Window always needs a RenderSystem
  Raz::RenderSystem renderSystem(1, 2, "Test", Raz::WindowSetting::INVISIBLE);
  Raz::Window& window = renderSystem.getWindow();

  CHECK(window.getWidth() == 1);
  CHECK(window.getHeight() == 2);

  window.resize(3, 4);
  CHECK(window.getWidth() == 3);
  CHECK(window.getHeight() == 4);

  window.makeFullscreen();
  CHECK(window.getWidth() > 3);
  CHECK(window.getHeight() > 4);

  // Making a window exit fullscreen gives it back its previous size
  window.makeWindowed();
  CHECK(window.getWidth() == 3);
  CHECK(window.getHeight() == 4);
}
