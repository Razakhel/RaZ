#pragma once

#ifndef RAZ_OFFLINERENDERSYSTEM_HPP
#define RAZ_OFFLINERENDERSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Window.hpp"

namespace Raz {

class BvhSystem;
class Entity;
class MeshRenderer;

/// OfflineRenderSystem class, handling the offline (raytraced) rendering part.
class OfflineRenderSystem final : public System {
public:
  /// Creates an offline render system, initializing its inner data.
  OfflineRenderSystem() { initialize(); }
  /// Creates an offline render system with a given scene size.
  /// \param sceneWidth Width of the scene.
  /// \param sceneHeight Height of the scene.
  OfflineRenderSystem(unsigned int sceneWidth, unsigned int sceneHeight) : OfflineRenderSystem() { resizeViewport(sceneWidth, sceneHeight); }
#if !defined(RAZ_NO_WINDOW)
  /// Creates an offline render system along with a window.
  /// \param sceneWidth Width of the scene.
  /// \param sceneHeight Height of the scene.
  /// \param windowTitle Title of the window.
  /// \param settings Settings to create the window with.
  OfflineRenderSystem(unsigned int sceneWidth, unsigned int sceneHeight, const std::string& windowTitle, WindowSetting settings = WindowSetting::DEFAULT)
    : m_window{ Window::create(sceneWidth, sceneHeight, windowTitle, settings) } { initialize(sceneWidth, sceneHeight); }
#endif

#if !defined(RAZ_NO_WINDOW)
  bool hasWindow() const { return (m_window != nullptr); }
  const Window& getWindow() const { assert("Error: The window must be set before being accessed." && hasWindow()); return *m_window; }
  Window& getWindow() { return const_cast<Window&>(static_cast<const OfflineRenderSystem*>(this)->getWindow()); }
#endif
  bool hasCubemap() const { return m_cubemap.has_value(); }
  const Cubemap& getCubemap() const { assert("Error: The cubemap must be set before being accessed." && hasCubemap()); return *m_cubemap; }

  void setBvh(const BvhSystem& bvh) { m_bvh = &bvh; }
  void setCubemap(Cubemap&& cubemap) { m_cubemap = std::move(cubemap); }

#if !defined(RAZ_NO_WINDOW)
  void createWindow(unsigned int width, unsigned int height, const std::string& title = "") { m_window = Window::create(width, height, title); }
#endif
  void resizeViewport(unsigned int width, unsigned int height);
  bool update(float deltaTime) override;
  void saveToImage(const FilePath& filePath) const;
  void removeBvh() { m_bvh = nullptr; }
  void removeCubemap() { m_cubemap.reset(); }
  void destroy() override;

protected:
  void linkEntity(const EntityPtr& entity) override;

private:
  void initialize();
  void initialize(unsigned int sceneWidth, unsigned int sceneHeight);
  const Entity* recoverHitEntity(const Ray& ray, RayHit& closestHit);
  Vec3f computeLighting(const Entity& entity, const RayHit& hitInfo);

#if !defined(RAZ_NO_WINDOW)
  WindowPtr m_window {};

  static inline Texture2DPtr s_texture {};
  RenderPass m_displayPass {};
#endif

  Entity* m_cameraEntity {};
  const BvhSystem* m_bvh {};
  std::optional<Cubemap> m_cubemap {};

  Image m_renderedScene {};
};

} // namespace Raz

#endif // RAZ_OFFLINERENDERSYSTEM_HPP
