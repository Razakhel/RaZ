#pragma once

#ifndef RAZ_MESHRENDERER_HPP
#define RAZ_MESHRENDERER_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Render/Material.hpp"
#include "RaZ/Render/SubmeshRenderer.hpp"

#include <memory>

namespace Raz {

class Mesh;

class MeshRenderer final : public Component {
public:
  MeshRenderer() = default;
  explicit MeshRenderer(const Mesh& mesh, RenderMode renderMode = RenderMode::TRIANGLE) { load(mesh, renderMode); }
  MeshRenderer(const MeshRenderer&) = delete;
  MeshRenderer(MeshRenderer&&) noexcept = default;

  const std::vector<SubmeshRenderer>& getSubmeshRenderers() const { return m_submeshRenderers; }
  std::vector<SubmeshRenderer>& getSubmeshRenderers() { return m_submeshRenderers; }
  const std::vector<MaterialPtr>& getMaterials() const { return m_materials; }
  std::vector<MaterialPtr>& getMaterials() { return m_materials; }

  static void drawUnitPlane();
  static void drawUnitSphere();
  static void drawUnitQuad();
  static void drawUnitCube();

  void setRenderMode(RenderMode renderMode, const Mesh& mesh);
  void setMaterial(MaterialPtr material);
  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  Material& addMaterial(MaterialPtr material) { return *m_materials.emplace_back(std::move(material)); }
  template <typename... Args> SubmeshRenderer& addSubmeshRenderer(Args&&... args) { return m_submeshRenderers.emplace_back(std::forward<Args>(args)...); }
  /// Clones the mesh renderer.
  /// \warning This doesn't load anything onto the graphics card; the load() function must be called afterwards with a Mesh for this.
  /// \return Cloned mesh renderer.
  MeshRenderer clone() const;
  void load(const Mesh& mesh, RenderMode renderMode = RenderMode::TRIANGLE);
  void load(const ShaderProgram& program) const;
  void load(const Mesh& mesh, const ShaderProgram& program, RenderMode renderMode = RenderMode::TRIANGLE);
  void draw() const;
  void draw(const ShaderProgram& program) const;

  MeshRenderer& operator=(const MeshRenderer&) = delete;
  MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

private:
  std::vector<SubmeshRenderer> m_submeshRenderers {};
  std::vector<MaterialPtr> m_materials {};
};

} // namespace Raz

#endif // RAZ_MESHRENDERER_HPP
