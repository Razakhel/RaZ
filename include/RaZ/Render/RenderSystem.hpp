#pragma once

#ifndef RAZ_RENDERSYSTEM_HPP
#define RAZ_RENDERSYSTEM_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/System.hpp"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

class RenderSystem : public System {
public:
  RenderSystem(unsigned int windowWidth, unsigned int windowHeight, const std::string& windowTitle = "");

  const Window& getWindow() const { return m_window; }
  Window& getWindow() { return m_window; }
  const Entity& getCameraEntity() const { return m_camera; }
  Entity& getCameraEntity() { return m_camera; }
  const ShaderProgram& getProgram() const { return m_program; }

  void setProgram(ShaderProgram&& program) { m_program = std::move(program); }

  void linkEntity(const EntityPtr& entity) override;
  void update(float deltaTime) override;
  void sendViewMatrix(const Mat4f& viewMat) const { m_cameraUbo.sendData(viewMat, 0); }
  void sendInverseViewMatrix(const Mat4f& invViewMat) const { m_cameraUbo.sendData(invViewMat, sizeof(Mat4f)); }
  void sendProjectionMatrix(const Mat4f& projMat) const { m_cameraUbo.sendData(projMat, sizeof(Mat4f) * 2); }
  void sendInverseProjectionMatrix(const Mat4f& invProjMat) const { m_cameraUbo.sendData(invProjMat, sizeof(Mat4f) * 3); }
  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_cameraUbo.sendData(viewProjMat, sizeof(Mat4f) * 4); }
  void sendCameraPosition(const Vec3f& cameraPos) const { m_cameraUbo.sendData(cameraPos, sizeof(Mat4f) * 5); }
  void updateLight(const Entity* entity, std::size_t lightIndex) const;
  void updateLights() const;
  void updateShaders() const;
  void destroy() override { m_window.setShouldClose(); }

private:
  Window m_window;
  Entity m_camera = Entity(0);
  ShaderProgram m_program {};
  UniformBuffer m_cameraUbo = UniformBuffer(sizeof(Mat4f) * 5 + sizeof(Vec4f), 0);
};

} // namespace Raz

#endif // RAZ_RENDERSYSTEM_HPP

