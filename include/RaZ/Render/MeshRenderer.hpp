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

  /// Sets a specific mode to render the mesh into.
  /// \param renderMode Render mode to apply.
  /// \param mesh Mesh to load the render mode's indices from.
  void setRenderMode(RenderMode renderMode, const Mesh& mesh);
  /// Sets one unique material for the whole mesh.
  /// \warning This clears all previously existing materials.
  /// \param material Material to be set.
  void setMaterial(MaterialPtr material);
  /// Replaces attributes of all existing materials according to a given preset.
  /// \param materialPreset Preset material to apply.
  /// \param roughnessFactor Material's roughness factor.
  void setMaterial(MaterialPreset materialPreset, float roughnessFactor);
  /// Adds a given material into the mesh renderer.
  /// \note This doesn't apply the material to any submesh; to do so, manually set the corresponding material index to any submesh renderer.
  /// \param material Material to be added.
  /// \return Reference to the newly added material.
  Material& addMaterial(MaterialPtr material) { return *m_materials.emplace_back(std::move(material)); }
  /// Adds a new submesh renderer to render a submesh with.
  /// \tparam Args Types of the arguments to be forwarded to the submesh renderer.
  /// \param args Arguments to be forwarded to the submesh renderer.
  /// \return Reference to the newly added submesh renderer.
  template <typename... Args> SubmeshRenderer& addSubmeshRenderer(Args&&... args) { return m_submeshRenderers.emplace_back(std::forward<Args>(args)...); }
  /// Clones the mesh renderer.
  /// \warning This doesn't load anything onto the GPU; to do so, call the load() function taking a Mesh afterwards.
  /// \return Cloned mesh renderer.
  MeshRenderer clone() const;
  /// Loads a mesh onto the GPU.
  /// \param mesh Mesh to be loaded.
  /// \param renderMode Render mode to apply.
  void load(const Mesh& mesh, RenderMode renderMode = RenderMode::TRIANGLE);
  /// Binds the materials' textures to the given shader program.
  /// \param program Shader program to bind the textures to.
  void load(const ShaderProgram& program) const;
  /// Loads a mesh onto the GPU and binds the materials' textures to the given shader program.
  /// \param mesh Mesh to be loaded.
  /// \param program Shader program to bind the textures to.
  /// \param renderMode Render mode to apply.
  void load(const Mesh& mesh, const ShaderProgram& program, RenderMode renderMode = RenderMode::TRIANGLE);
  /// Renders the mesh.
  /// \warning This requires to manually handle materials' uniforms updating and textures activation & binding; if not, use the overload with a ShaderProgram.
  void draw() const;
  /// Renders the mesh with the given shader program.
  /// \param program Program to render the mesh with.
  void draw(const ShaderProgram& program) const;

  MeshRenderer& operator=(const MeshRenderer&) = delete;
  MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

private:
  std::vector<SubmeshRenderer> m_submeshRenderers {};
  std::vector<MaterialPtr> m_materials {};
};

} // namespace Raz

#endif // RAZ_MESHRENDERER_HPP
