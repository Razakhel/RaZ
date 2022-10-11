#pragma once

#ifndef RAZ_DEMOUTILS_HPP
#define RAZ_DEMOUTILS_HPP

namespace Raz {

class Entity;
class FilePath;
class OverlayWindow;
class RenderSystem;
class Sound;
class Transform;
class Window;
class World;

} // namespace Raz

namespace DemoUtils {

/// Adds callbacks onto a window to allow moving & rotating a camera.
/// \param cameraEntity Camera for which to add controls.
/// \param window Window on which to add the callbacks.
void setupCameraControls(Raz::Entity& cameraEntity, Raz::Window& window);

/// Adds callbacks onto a window to allow moving, scaling & rotating a mesh.
/// \param meshEntity Mesh for which to add controls.
/// \param window Window on which to add the callbacks.
void setupMeshControls(Raz::Entity& meshEntity, Raz::Window& window);

/// Adds callbacks onto a window to allow moving a light & varying its energy.
/// \param lightEntity Light for which to add controls.
/// \param window Window on which to add the callbacks.
void setupLightControls(Raz::Entity& lightEntity, const Raz::RenderSystem& renderSystem, Raz::Window& window);

/// Adds a callback onto a window to allow adding a light on a transform's position.
/// \param transform Transform from which to apply the position to the new light.
/// \param world World in which to add the new light.
/// \param window Window on which to add the callback.
void setupAddLight(const Raz::Transform& transform, Raz::World& world, Raz::Window& window);

/// Adds callbacks onto a window to allow playing, pausing & stopping a sound.
/// \param sound Sound for which to add controls.
/// \param window Window on which to add the callbacks.
void setupSoundControls(const Raz::Sound& sound, Raz::Window& window);

/// Adds a callback onto a window to allow adding a sound on a transform's position.
/// \param transform Transform from which to apply the position to the new sound.
/// \param soundPath File path to the sound to be loaded.
/// \param world World in which to add the new sound.
/// \param window Window on which to add the callback.
void setupAddSound(const Raz::Transform& transform, const Raz::FilePath& soundPath, Raz::World& world, Raz::Window& window);

/// Inserts labels onto the overlay to display an help message about how to move around the scene.
/// \param overlayWindow Overlay window on which to insert the labels.
void insertOverlayCameraControlsHelp(Raz::OverlayWindow& overlayWindow);

/// Inserts a checkbox onto the overlay to allow toggling face culling's state.
/// \param window Window on which to toggle face culling.
/// \param overlayWindow Overlay window on which to insert the checkbox.
void insertOverlayCullingOption(const Raz::Window& window, Raz::OverlayWindow& overlayWindow);

/// Inserts a checkbox onto the overlay to allow toggling V-sync's state. Has no effect when using OpenGL ES (vertical sync is always enabled).
/// \param window Window on which to toggle V-sync.
/// \param overlayWindow Overlay window on which to insert the checkbox.
void insertOverlayVerticalSyncOption(const Raz::Window& window, Raz::OverlayWindow& overlayWindow);

/// Inserts dynamic labels onto the overlay to display frame time & FPS.
/// \param overlayWindow Overlay window on which to insert the labels.
void insertOverlayFrameSpeed(Raz::OverlayWindow& overlayWindow);

} // namespace DemoUtils

#endif // RAZ_DEMOUTILS_HPP
