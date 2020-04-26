#pragma once

#ifndef RAZ_RENDERSYSTEM_HPP
#define RAZ_RENDERSYSTEM_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/System.hpp"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

/// RenderSystem class, handling the rendering part.
class RenderSystem final : public System {
public:
  /// Creates a render system, initializing its inner data.
  RenderSystem() { initialize(); }
  /// Creates a render system with a given scene size.
  /// \param sceneWidth Width of the scene.
  /// \param sceneHeight Height of the scene.
  RenderSystem(unsigned int sceneWidth, unsigned int sceneHeight) : RenderSystem() { resizeViewport(sceneWidth, sceneHeight); }
  /// Creates a render system along with a Window.
  /// \param sceneWidth Width of the scene.
  /// \param sceneHeight Height of the scene.
  /// \param windowTitle Title of the window.
  /// \param antiAliasingSampleCount Number of anti-aliasing samples.
  RenderSystem(unsigned int sceneWidth, unsigned int sceneHeight, const std::string& windowTitle, uint8_t antiAliasingSampleCount = 1)
    : m_window{ Window::create(sceneWidth, sceneHeight, windowTitle, antiAliasingSampleCount) } { initialize(sceneWidth, sceneHeight); }

  bool hasWindow() const { return (m_window != nullptr); }
  const Window& getWindow() const { assert("Error: Window must be set before being accessed." && m_window); return *m_window; }
  Window& getWindow() { return const_cast<Window&>(static_cast<const RenderSystem*>(this)->getWindow()); }
  const Entity& getCameraEntity() const { return m_cameraEntity; }
  Entity& getCameraEntity() { return m_cameraEntity; }
  const Camera& getCamera() const { return m_cameraEntity.getComponent<Camera>(); }
  Camera& getCamera() { return m_cameraEntity.getComponent<Camera>(); }
  const GeometryPass& getGeometryPass() const;
  GeometryPass& getGeometryPass() { return const_cast<GeometryPass&>(static_cast<const RenderSystem*>(this)->getGeometryPass()); }
  const SSRPass& getSSRPass() const;
  SSRPass& getSSRPass() { return const_cast<SSRPass&>(static_cast<const RenderSystem*>(this)->getSSRPass()); }
  const Cubemap& getCubemap() const { assert("Error: Cubemap must be set before being accessed." && m_cubemap); return *m_cubemap; }

  void setCubemap(CubemapPtr cubemap) { m_cubemap = std::move(cubemap); }

  void resizeViewport(unsigned int width, unsigned int height);
  void createWindow(unsigned int width, unsigned int height, const std::string& title = "") { m_window = Window::create(width, height, title); }
  void enableGeometryPass(VertexShader vertShader, FragmentShader fragShader);
  void enableSSRPass(FragmentShader fragShader);
  void disableGeometryPass() { m_renderPasses[static_cast<std::size_t>(RenderPassType::GEOMETRY)].reset(); }
  void disableSSRPass() { m_renderPasses[static_cast<std::size_t>(RenderPassType::SSR)].reset(); }
  void linkEntity(const EntityPtr& entity) override;
  bool update(float deltaTime) override;
  void sendViewMatrix(const Mat4f& viewMat) const { m_cameraUbo.sendData(viewMat, 0); }
  void sendInverseViewMatrix(const Mat4f& invViewMat) const { m_cameraUbo.sendData(invViewMat, sizeof(Mat4f)); }
  void sendProjectionMatrix(const Mat4f& projMat) const { m_cameraUbo.sendData(projMat, sizeof(Mat4f) * 2); }
  void sendInverseProjectionMatrix(const Mat4f& invProjMat) const { m_cameraUbo.sendData(invProjMat, sizeof(Mat4f) * 3); }
  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_cameraUbo.sendData(viewProjMat, sizeof(Mat4f) * 4); }
  void sendCameraPosition(const Vec3f& cameraPos) const { m_cameraUbo.sendData(cameraPos, sizeof(Mat4f) * 5); }
  void sendCameraMatrices(const Mat4f& viewProjMat) const;
  void sendCameraMatrices() const;
  void updateLight(const Entity* entity, std::size_t lightIndex) const;
  void updateLights() const;
  void removeCubemap() { m_cubemap.reset(); }
  void updateShaders() const;
  void saveToImage(const std::string& fileName, TextureFormat format = TextureFormat::RGB) const;
  void destroy() override { if (m_window) m_window->setShouldClose(); }

private:
  void initialize();
  void initialize(unsigned int sceneWidth, unsigned int sceneHeight);

  unsigned int m_sceneWidth {};
  unsigned int m_sceneHeight {};

  WindowPtr m_window {};
  Entity m_cameraEntity = Entity(0);

  std::array<RenderPassPtr, static_cast<std::size_t>(RenderPassType::RENDER_PASS_COUNT)> m_renderPasses {};
  UniformBuffer m_cameraUbo = UniformBuffer(sizeof(Mat4f) * 5 + sizeof(Vec4f), 0);

  CubemapPtr m_cubemap {};
};

} // namespace Raz

#endif // RAZ_RENDERSYSTEM_HPP
