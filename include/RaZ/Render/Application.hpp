#pragma once

#ifndef RAZ_APPLICATION_HPP
#define RAZ_APPLICATION_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Scene.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

class Application {
public:
  Application(WindowPtr window, ScenePtr scene, CameraPtr camera = nullptr);

  const WindowPtr& getWindow() const { return m_window; }
  const ScenePtr& getScene() const { return m_scene; }
  const CameraPtr& getCamera() const { return m_camera; }

  void setWindow(WindowPtr window) { m_window = std::move(window); }
  void setScene(ScenePtr scene) { m_scene = std::move(scene); }
  void setCamera(CameraPtr camera) { m_camera = std::move(camera); }

  void sendViewMatrix(const Mat4f& viewMat) const { m_cameraUbo.sendData(viewMat, 0); }
  void sendInverseViewMatrix(const Mat4f& invViewMat) const { m_cameraUbo.sendData(invViewMat, sizeof(Mat4f)); }
  void sendProjectionMatrix(const Mat4f& projMat) const { m_cameraUbo.sendData(projMat, sizeof(Mat4f) * 2); }
  void sendInverseProjectionMatrix(const Mat4f& invProjMat) const { m_cameraUbo.sendData(invProjMat, sizeof(Mat4f) * 3); }
  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_cameraUbo.sendData(viewProjMat, sizeof(Mat4f) * 4); }
  void sendCameraPosition(const Vec3f& cameraPos) const { m_cameraUbo.sendData(cameraPos, sizeof(Mat4f) * 5); }
  bool run() const;
  void updateShaders() const;

private:
  WindowPtr m_window {};
  ScenePtr m_scene {};
  CameraPtr m_camera = std::make_unique<Camera>(m_window->getWidth(), m_window->getHeight(), 45.f, 0.1f, 100.f);
  UniformBuffer m_cameraUbo = UniformBuffer(sizeof(Mat4f) * 5 + sizeof(Vec4f), 0);
};

} // namespace Raz

#endif // RAZ_APPLICATION_HPP
