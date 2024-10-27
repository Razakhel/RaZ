#pragma once

#ifndef RAZ_RENDERSYSTEM_HPP
#define RAZ_RENDERSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Render/Window.hpp"

#if !defined(__APPLE__) && !defined(__EMSCRIPTEN__) && !defined(RAZ_NO_WINDOW)
// XR currently isn't available with macOS or Emscripten and requires windowing capabilities
#define RAZ_USE_XR
#endif

namespace Raz {

class Entity;
class MeshRenderer;
#if defined(RAZ_USE_XR)
class XrSystem;
#endif

/// RenderSystem class, handling the rendering part.
class RenderSystem final : public System {
  friend RenderGraph;

public:
  /// Creates a render system, initializing its inner data.
  RenderSystem() { initialize(); }
  /// Creates a render system with a given scene size.
  /// \param sceneWidth Width of the scene.
  /// \param sceneHeight Height of the scene.
  RenderSystem(unsigned int sceneWidth, unsigned int sceneHeight) : RenderSystem() { resizeViewport(sceneWidth, sceneHeight); }
#if !defined(RAZ_NO_WINDOW)
  /// Creates a render system along with a window.
  /// \param windowWidth Width of the window.
  /// \param windowHeight Height of the window.
  /// \param windowTitle Title of the window.
  /// \param settings Settings to create the window with.
  /// \param antiAliasingSampleCount Number of anti-aliasing samples.
  /// \note The window's width & height are to be considered just hints; the window manager remains responsible for the actual dimensions, which may be lower.
  ///   This can notably happen when the requested window size exceeds what the screens can display. The actual window's size can be queried afterward.
  /// \see getWindow(), Window::getWidth(), Window::getHeight()
  RenderSystem(unsigned int windowWidth, unsigned int windowHeight,
               const std::string& windowTitle,
               WindowSetting windowSettings = WindowSetting::DEFAULT,
               uint8_t antiAliasingSampleCount = 1)
    : m_window{ Window::create(*this, windowWidth, windowHeight, windowTitle, windowSettings, antiAliasingSampleCount) } { initialize(m_window->getWidth(),
                                                                                                                                      m_window->getHeight()); }
#endif

#if !defined(RAZ_NO_WINDOW)
  bool hasWindow() const { return (m_window != nullptr); }
  const Window& getWindow() const { assert("Error: The window must be set before being accessed." && hasWindow()); return *m_window; }
  Window& getWindow() { return const_cast<Window&>(static_cast<const RenderSystem*>(this)->getWindow()); }
#endif
  const RenderPass& getGeometryPass() const { return m_renderGraph.getGeometryPass(); }
  RenderPass& getGeometryPass() { return m_renderGraph.getGeometryPass(); }
  const RenderGraph& getRenderGraph() const { return m_renderGraph; }
  RenderGraph& getRenderGraph() { return m_renderGraph; }
  bool hasCubemap() const { return m_cubemap.has_value(); }
  const Cubemap& getCubemap() const { assert("Error: The cubemap must be set before being accessed." && hasCubemap()); return *m_cubemap; }

  void setCubemap(Cubemap&& cubemap);
#if defined(RAZ_USE_XR)
  void enableXr(XrSystem& xrSystem);
#endif

#if !defined(RAZ_NO_WINDOW)
  void createWindow(unsigned int width, unsigned int height,
                    const std::string& title = {},
                    WindowSetting settings = WindowSetting::DEFAULT,
                    uint8_t antiAliasingSampleCount = 1) { m_window = Window::create(*this, width, height, title, settings, antiAliasingSampleCount); }
#endif
  void resizeViewport(unsigned int width, unsigned int height);
  bool update(const FrameTimeInfo& timeInfo) override;
  /// Updates all lights referenced by the RenderSystem, sending their data to the GPU.
  void updateLights() const;
  void updateShaders() const;
  void updateMaterials(const MeshRenderer& meshRenderer) const;
  void updateMaterials() const;
  /// Retrieves & saves the back buffer's data from the GPU.
  /// \warning The pixel storage pack & unpack alignments should be set to 1 in order to recover actual pixels.
  /// \see Renderer::setPixelStorage()
  /// \warning Retrieving an image from the GPU is slow; use this function with caution.
  void saveToImage(const FilePath& filePath, TextureFormat format = TextureFormat::RGB, PixelDataType dataType = PixelDataType::UBYTE) const;
  void removeCubemap() { m_cubemap.reset(); }
  void destroy() override;

protected:
  void linkEntity(const EntityPtr& entity) override;

private:
  void initialize();
  void initialize(unsigned int sceneWidth, unsigned int sceneHeight);
  void sendCameraInfo() const;
  void sendViewMatrix(const Mat4f& viewMat) const { m_cameraUbo.sendData(viewMat, 0); }
  void sendInverseViewMatrix(const Mat4f& invViewMat) const { m_cameraUbo.sendData(invViewMat, sizeof(Mat4f)); }
  void sendProjectionMatrix(const Mat4f& projMat) const { m_cameraUbo.sendData(projMat, sizeof(Mat4f) * 2); }
  void sendInverseProjectionMatrix(const Mat4f& invProjMat) const { m_cameraUbo.sendData(invProjMat, sizeof(Mat4f) * 3); }
  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_cameraUbo.sendData(viewProjMat, sizeof(Mat4f) * 4); }
  void sendCameraPosition(const Vec3f& cameraPos) const { m_cameraUbo.sendData(cameraPos, sizeof(Mat4f) * 5); }
  /// Updates a single light, sending its data to the GPU.
  /// \warning The lights UBO needs to be bound before calling this function.
  /// \note If resetting a removed light or updating one not yet known by the application, call updateLights() instead to fully take that change into account.
  /// \param entity Light entity to be updated; if not a directional light, needs to have a Transform component.
  /// \param lightIndex Index of the light to be updated.
  void updateLight(const Entity& entity, unsigned int lightIndex) const;
#if defined(RAZ_USE_XR)
  void renderXrFrame();
#endif
  void copyToWindow(const Texture2D& colorBuffer, const Texture2D& depthBuffer, unsigned int windowWidth, unsigned int windowHeight) const;

  unsigned int m_sceneWidth {};
  unsigned int m_sceneHeight {};

#if !defined(RAZ_NO_WINDOW)
  WindowPtr m_window {};
#endif

  Entity* m_cameraEntity {};
  RenderGraph m_renderGraph {};
  UniformBuffer m_cameraUbo = UniformBuffer(sizeof(Mat4f) * 5 + sizeof(Vec4f), UniformBufferUsage::DYNAMIC);
  UniformBuffer m_lightsUbo = UniformBuffer(sizeof(Vec4f) * 4 * 100 + sizeof(Vec4u), UniformBufferUsage::DYNAMIC);
  UniformBuffer m_timeUbo   = UniformBuffer(sizeof(float) * 2, UniformBufferUsage::STREAM);
  UniformBuffer m_modelUbo  = UniformBuffer(sizeof(Mat4f), UniformBufferUsage::STREAM);

  std::optional<Cubemap> m_cubemap {};

#if defined(RAZ_USE_XR)
  XrSystem* m_xrSystem {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERSYSTEM_HPP
