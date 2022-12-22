#include "RaZ/Entity.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"

#include "DemoUtils.hpp"

using namespace Raz::Literals;

namespace DemoUtils {

void setupCameraControls(Raz::Entity& cameraEntity, Raz::Window& window) {
  auto& camera          = cameraEntity.getComponent<Raz::Camera>();
  auto& cameraTransform = cameraEntity.getComponent<Raz::Transform>();

  /////////////////////
  // Camera movement //
  /////////////////////

  static float cameraSpeed = 1.f;

  // Camera speedup
  window.addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                        [] (float /* deltaTime */) noexcept { cameraSpeed = 2.f; },
                        Raz::Input::ONCE,
                        [] () noexcept { cameraSpeed = 1.f; });

  window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraTransform] (float deltaTime) {
    cameraTransform.move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
  });

  window.addKeyCallback(Raz::Keyboard::V, [&cameraTransform] (float deltaTime) {
    cameraTransform.move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
  });

  window.addKeyCallback(Raz::Keyboard::W, [&cameraTransform, &camera] (float deltaTime) {
    const float moveVal = (-10.f * deltaTime) * cameraSpeed;

    cameraTransform.move(0.f, 0.f, moveVal);
    camera.setOrthographicBound(camera.getOrthographicBound() + moveVal);
  });

  window.addKeyCallback(Raz::Keyboard::S, [&cameraTransform, &camera] (float deltaTime) {
    const float moveVal = (10.f * deltaTime) * cameraSpeed;

    cameraTransform.move(0.f, 0.f, moveVal);
    camera.setOrthographicBound(camera.getOrthographicBound() + moveVal);
  });

  window.addKeyCallback(Raz::Keyboard::A, [&cameraTransform] (float deltaTime) {
    cameraTransform.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  window.addKeyCallback(Raz::Keyboard::D, [&cameraTransform] (float deltaTime) {
    cameraTransform.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  window.setMouseScrollCallback([&camera] (double /* xOffset */, double yOffset) {
    const float newFovDeg = std::clamp(Raz::Degreesf(camera.getFieldOfView()).value + static_cast<float>(-yOffset) * 2.f, 15.f, 90.f);
    camera.setFieldOfView(Raz::Degreesf(newFovDeg));
  });

  /////////////////////
  // Camera rotation //
  /////////////////////

  // The camera can be rotated while holding the mouse right click
  static bool isRightClicking = false;

  window.addMouseButtonCallback(Raz::Mouse::RIGHT_CLICK, [&window] (float /* deltaTime */) {
    isRightClicking = true;
    window.disableCursor();
  }, Raz::Input::ONCE, [&window] () {
    isRightClicking = false;
    window.showCursor();
  });

  window.setMouseMoveCallback([&cameraTransform, &window] (double xMove, double yMove) {
    if (!isRightClicking)
      return;

    // Dividing movement by the window's size to scale between -1 and 1
    cameraTransform.rotate(-90_deg * static_cast<float>(yMove) / window.getHeight(),
                           -90_deg * static_cast<float>(xMove) / window.getWidth());
  });
}

void setupMeshControls(Raz::Entity& meshEntity, Raz::Window& window) {
  auto& meshTransform = meshEntity.getComponent<Raz::Transform>();

  ///////////////////
  // Mesh movement //
  ///////////////////

  window.addKeyCallback(Raz::Keyboard::T, [&meshTransform] (float deltaTime) { meshTransform.move(0.f, 0.f,  10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::G, [&meshTransform] (float deltaTime) { meshTransform.move(0.f, 0.f, -10.f * deltaTime); });
  window.addKeyCallback(Raz::Keyboard::F, [&meshTransform] (float deltaTime) { meshTransform.move(-10.f * deltaTime, 0.f, 0.f); });
  window.addKeyCallback(Raz::Keyboard::H, [&meshTransform] (float deltaTime) { meshTransform.move( 10.f * deltaTime, 0.f, 0.f); });

  //////////////////
  // Mesh scaling //
  //////////////////

  window.addKeyCallback(Raz::Keyboard::X, [&meshTransform] (float /* deltaTime */) { meshTransform.scale(0.5f); }, Raz::Input::ONCE);
  window.addKeyCallback(Raz::Keyboard::C, [&meshTransform] (float /* deltaTime */) { meshTransform.scale(2.f); }, Raz::Input::ONCE);

  ///////////////////
  // Mesh rotation //
  ///////////////////

  window.addKeyCallback(Raz::Keyboard::UP,    [&meshTransform] (float deltaTime) { meshTransform.rotate(90_deg * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::DOWN,  [&meshTransform] (float deltaTime) { meshTransform.rotate(-90_deg * deltaTime, Raz::Axis::X); });
  window.addKeyCallback(Raz::Keyboard::LEFT,  [&meshTransform] (float deltaTime) { meshTransform.rotate(90_deg * deltaTime, Raz::Axis::Y); });
  window.addKeyCallback(Raz::Keyboard::RIGHT, [&meshTransform] (float deltaTime) { meshTransform.rotate(-90_deg * deltaTime, Raz::Axis::Y); });
}

void setupLightControls(Raz::Entity& lightEntity, const Raz::RenderSystem& renderSystem, Raz::Window& window) {
  auto& light          = lightEntity.getComponent<Raz::Light>();
  auto& lightTransform = lightEntity.getComponent<Raz::Transform>();

  ////////////////////
  // Light movement //
  ////////////////////

  window.addKeyCallback(Raz::Keyboard::I, [&lightTransform, &renderSystem] (float deltaTime) {
    lightTransform.translate(0.f, 0.f, -10.f * deltaTime);
    renderSystem.updateLights();
  });

  window.addKeyCallback(Raz::Keyboard::K, [&lightTransform, &renderSystem] (float deltaTime) {
    lightTransform.translate(0.f, 0.f, 10.f * deltaTime);
    renderSystem.updateLights();
  });

  window.addKeyCallback(Raz::Keyboard::J, [&lightTransform, &renderSystem] (float deltaTime) {
    lightTransform.translate(-10.f * deltaTime, 0.f, 0.f);
    renderSystem.updateLights();
  });

  window.addKeyCallback(Raz::Keyboard::L, [&lightTransform, &renderSystem] (float deltaTime) {
    lightTransform.translate(10.f * deltaTime, 0.f, 0.f);
    renderSystem.updateLights();
  });

  //////////////////
  // Light energy //
  //////////////////

  window.addKeyCallback(Raz::Keyboard::PAGEUP, [&light, &renderSystem] (float deltaTime) {
    light.setEnergy(light.getEnergy() + 1.f * deltaTime);
    renderSystem.updateLights();
  });

  window.addKeyCallback(Raz::Keyboard::PAGEDOWN, [&light, &renderSystem] (float deltaTime) {
    light.setEnergy(std::max(0.f, light.getEnergy() - 1.f * deltaTime));
    renderSystem.updateLights();
  });
}

void setupAddLight(const Raz::Transform& transform, Raz::World& world, Raz::Window& window) {
  window.addMouseButtonCallback(Raz::Mouse::MIDDLE_CLICK, [&world, &transform] (float /* deltaTime */) {
    auto& newLight = world.addEntityWithComponent<Raz::Light>(Raz::LightType::POINT, // Type (point light)
                                                              10.f);                 // Energy
    newLight.addComponent<Raz::Transform>(transform.getPosition());
  }, Raz::Input::ONCE);
}

void setupSoundControls(const Raz::Sound& sound, Raz::Window& window) {
  // Toggling play/pause
  window.addKeyCallback(Raz::Keyboard::NUM0, [&sound] (float /* deltaTime */) noexcept {
    if (sound.isPlaying())
      sound.pause();
    else
      sound.play();
  }, Raz::Input::ONCE);

  // Stopping the sound
  window.addKeyCallback(Raz::Keyboard::DECIMAL, [&sound] (float /* deltaTime */) noexcept { sound.stop(); }, Raz::Input::ONCE);
}

void setupAddSound(const Raz::Transform& transform, const Raz::FilePath& soundPath, Raz::World& world, Raz::Window& window) {
  // Adding a new sound on the given position
  window.addKeyCallback(Raz::Keyboard::ADD, [&world, &soundPath, &transform] (float /* deltaTime */) {
    Raz::Entity& newSound = world.addEntityWithComponent<Raz::Sound>(Raz::WavFormat::load(soundPath));
    newSound.addComponent<Raz::Transform>(transform.getPosition());
  }, Raz::Input::ONCE);
}

void insertOverlayCameraControlsHelp(Raz::OverlayWindow& overlayWindow) {
  overlayWindow.addLabel("Press WASD to fly the camera around,");
  overlayWindow.addLabel("Space/V to go up/down,");
  overlayWindow.addLabel("& Shift to move faster.");
  overlayWindow.addLabel("Hold the right mouse button to rotate the camera.");
}

void insertOverlayCullingOption(const Raz::Window& window, Raz::OverlayWindow& overlayWindow) {
  overlayWindow.addCheckbox("Enable face culling",
                            [&window] () { window.enableFaceCulling(); },
                            [&window] () { window.disableFaceCulling(); },
                            true);
}

void insertOverlayVerticalSyncOption(const Raz::Window& window, Raz::OverlayWindow& overlayWindow) {
#if !defined(USE_OPENGL_ES)
  overlayWindow.addCheckbox("Enable vertical sync",
                            [&window] () { window.enableVerticalSync(); },
                            [&window] () { window.disableVerticalSync(); },
                            window.recoverVerticalSyncState());
#endif
}

void insertOverlayFrameSpeed(Raz::OverlayWindow& overlayWindow) {
  overlayWindow.addFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  overlayWindow.addFpsCounter("FPS: %.1f");
}

} // namespace DemoUtils
