#pragma once

#ifndef RAZ_SCENE_HPP
#define RAZ_SCENE_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/Model.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

class Scene {
public:
  Scene(VertexShaderPtr vertShader, FragmentShaderPtr fragShader, GeometryShaderPtr geomShader = nullptr)
    : m_program(std::move(vertShader), std::move(fragShader), std::move(geomShader)) {}
  Scene(VertexShaderPtr vertShader, FragmentShaderPtr fragShader, std::vector<ModelPtr> models, GeometryShaderPtr geomShader = nullptr)
    : m_program(std::move(vertShader), std::move(fragShader), std::move(geomShader)), m_models{ std::move(models) } {}

  const ShaderProgram& getProgram() const { return m_program; }
  const CubemapPtr& getCubemap() const { return m_cubemap; }
  const std::vector<ModelPtr>& getModels() const { return m_models; }
  std::size_t recoverVertexCount() const;
  std::size_t recoverTriangleCount() const;

  void setCubemap(CubemapPtr cubemap) { m_cubemap = std::move(cubemap); }

  void addModel(ModelPtr model) { m_models.emplace_back(std::move(model)); }
  void addLight(LightPtr light) { m_lights.emplace_back(std::move(light)); }
  void load() const;
  void render(const CameraPtr& camera) const;
  void updateLights() const;

private:
  ShaderProgram m_program {};
  CubemapPtr m_cubemap {};
  std::vector<ModelPtr> m_models {};
  std::vector<LightPtr> m_lights {};
};

using ScenePtr = std::unique_ptr<Scene>;

} // namespace Raz

#endif // RAZ_SCENE_HPP
