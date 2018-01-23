#pragma once

#ifndef RAZ_SCENE_HPP
#define RAZ_SCENE_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Model.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

class Scene {
public:
  Scene() = default;
  Scene(const VertexShader& vertShader, const FragmentShader& fragShader) : m_program(vertShader, fragShader) {}
  Scene(const VertexShader& vertShader, const FragmentShader& fragShader, std::vector<ModelPtr>&& models)
    : m_program(vertShader, fragShader), m_models(std::move(models)) {}

  const ShaderProgram& getProgram() const { return m_program; }
  const std::vector<ModelPtr>& getModels() const { return m_models; }

  //void setCamera(CameraPtr camera) { m_camera = std::move(camera); }
  void addModel(ModelPtr model) { m_models.emplace_back(std::move(model)); }
  void addLight(LightPtr light) { m_lights.emplace_back(std::move(light)); }
  void render(const Mat4f& viewProjMat) const;
  void updateLights() const;

private:
  ShaderProgram m_program;
  //CameraPtr m_camera;
  std::vector<ModelPtr> m_models;
  std::vector<LightPtr> m_lights;
};

} // namespace Raz

#endif // RAZ_SCENE_HPP
