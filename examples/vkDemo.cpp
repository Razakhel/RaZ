#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  ////////////////////
  // Initialization //
  ////////////////////

  Raz::Application app;
  Raz::World& world = app.addWorld(3);

  ///////////////
  // Rendering //
  ///////////////

  auto& renderSystem = world.addSystem<Raz::RenderSystem>(800, 600, "RaZ");

  Raz::Window& window = renderSystem.getWindow();
  window.setIcon(RAZ_ROOT + "assets/icons/RaZ_logo_128.png"s);

  // Allowing to quit the application with the Escape key
  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&window] (float /* deltaTime */) { window.setShouldClose(); });

  ///////////////////
  // Camera entity //
  ///////////////////

  Raz::Entity& camera = world.addEntity();
  auto& cameraComp    = camera.addComponent<Raz::Camera>(window.getWidth(), window.getHeight());
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));

  //////////////////////////
  // Starting application //
  //////////////////////////

  app.run([] () { Raz::Renderer::drawFrame(); });

  return EXIT_SUCCESS;
}
