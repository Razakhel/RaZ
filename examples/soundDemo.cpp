#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(2);

  const std::vector<std::string> devices = Raz::AudioSystem::recoverDevices();

  std::cout << "Available audio devices:\n";
  for (const std::string& device : devices)
    std::cout << "- " << device << '\n';
  std::cout << std::flush;

  auto& audio = world.addSystem<Raz::AudioSystem>();
  std::cout << "Current audio device: " << audio.recoverCurrentDevice() << std::endl;

  Raz::Entity& sound = world.addEntity();
  auto& soundComp    = sound.addComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT + "assets/sounds/wave_seagulls.wav"s));
  soundComp.play();

  float elapsedTime = 0.f;

  app.run([&] () {
    elapsedTime = std::max(elapsedTime, soundComp.recoverElapsedTime());

    if (!soundComp.isPlaying()) {
      std::cout << "Sound's duration: " << elapsedTime <<  " minutes" << std::endl;
      app.quit();
    }
  });

  return EXIT_SUCCESS;
}
