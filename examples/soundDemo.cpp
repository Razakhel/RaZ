#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(Raz::World(2));

  const std::vector<std::string> devices = Raz::AudioSystem::recoverDevices();

  std::cout << "Available audio devices:\n";
  for (const std::string& device : devices)
    std::cout << "- " << device << '\n';
  std::cout << std::flush;

  auto& audio = world.addSystem<Raz::AudioSystem>();
  std::cout << "Current audio device: " << audio.recoverCurrentDevice() << std::endl;

  world.addEntityWithComponent<Raz::Sound>(RAZ_ROOT + "assets/sounds/wave_seagulls.wav"s);

  app.runOnce();

  return EXIT_SUCCESS;
}
