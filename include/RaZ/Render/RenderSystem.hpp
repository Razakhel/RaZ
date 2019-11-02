#pragma once

#ifndef RAZ_RENDERSYSTEM_HPP
#define RAZ_RENDERSYSTEM_HPP

#include "RaZ/Entity.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/System.hpp"
#include "RaZ/Utils/Window.hpp"

namespace Raz {

enum class RenderPass : uint8_t {
  GEOMETRY = 0,
  //LIGHTING,
  //SSAO,
  SSR,
  //SHADOW,
  //TRANSPARENCY,

  RENDER_PASS_COUNT
};

using ShaderPrograms = std::array<ShaderProgram, static_cast<std::size_t>(RenderPass::RENDER_PASS_COUNT)>;
using Framebuffers   = std::array<Framebuffer, static_cast<std::size_t>(RenderPass::RENDER_PASS_COUNT)>;

class RenderSystem : public System {
public:
  RenderSystem(unsigned int windowWidth, unsigned int windowHeight, const std::string& windowTitle = "");

  const Window& getWindow() const { return m_window; }
  Window& getWindow() { return m_window; }
  const Entity& getCameraEntity() const { return m_camera; }
  Entity& getCameraEntity() { return m_camera; }
  const Camera& getCamera() const { return m_camera.getComponent<Camera>(); }
  Camera& getCamera() { return m_camera.getComponent<Camera>(); }
  const ShaderProgram& getProgram(RenderPass renderPass) const { return m_programs[static_cast<std::size_t>(renderPass)]; }
  const ShaderProgram& getGeometryProgram() const { return getProgram(RenderPass::GEOMETRY); }
  //const ShaderProgram& getLightingProgram() const { return getProgram(RenderPass::LIGHTING); }
  //const ShaderProgram& getSSAOProgram() const { return getProgram(RenderPass::SSAO); }
  const ShaderProgram& getSSRProgram() const { return getProgram(RenderPass::SSR); }
  //const ShaderProgram& getShadowProgram() const { return getProgram(RenderPass::SHADOW); }
  //const ShaderProgram& getTransparencyProgram() const { return getProgram(RenderPass::TRANSPARENCY); }
  const CubemapPtr& getCubemap() const { return m_cubemap; }

  void setProgram(RenderPass renderPass, ShaderProgram&& program);
  void setGeometryProgram(ShaderProgram&& program) { setProgram(RenderPass::GEOMETRY, std::move(program)); }
  //void setLightingProgram(ShaderProgram&& program) { setProgram(RenderPass::LIGHTING, std::move(program)); }
  //void setSSAOProgram(ShaderProgram&& program) { setProgram(RenderPass::SSAO, std::move(program)); }
  void setSSRProgram(ShaderProgram&& program) { setProgram(RenderPass::SSR, std::move(program)); }
  //void setShadowProgram(ShaderProgram&& program) { setProgram(RenderPass::SHADOW, std::move(program)); }
  //void setTransparencyProgram(ShaderProgram&& program) { setProgram(RenderPass::TRANSPARENCY, std::move(program)); }
  void setCubemap(CubemapPtr cubemap) { m_cubemap = std::move(cubemap); }

  void enableRenderPass(RenderPass renderPass, bool value = true) { enableRenderPass(static_cast<std::size_t>(renderPass), value); }
  void enableRenderPass(std::size_t programIndex, bool value = true) { m_enabledPasses.setBit(programIndex, value); }
  void disableRenderPass(RenderPass renderPass) { enableRenderPass(renderPass, false); }
  void disableRenderPass(std::size_t programIndex) { enableRenderPass(programIndex, false); }
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
  void destroy() override { m_window.setShouldClose(); }

private:
  Window m_window;
  Entity m_camera = Entity(0);

  ShaderPrograms m_programs {};
  Framebuffers m_framebuffers {};
  Bitset m_enabledPasses = Bitset(static_cast<std::size_t>(RenderPass::RENDER_PASS_COUNT));
  UniformBuffer m_cameraUbo = UniformBuffer(sizeof(Mat4f) * 5 + sizeof(Vec4f), 0);

  CubemapPtr m_cubemap {};
};

} // namespace Raz

#endif // RAZ_RENDERSYSTEM_HPP
