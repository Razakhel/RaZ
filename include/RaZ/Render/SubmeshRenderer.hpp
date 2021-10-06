#pragma once

#ifndef RAZ_SUBMESHRENDERER_HPP
#define RAZ_SUBMESHRENDERER_HPP

#include "RaZ/Data/Submesh.hpp"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Utils/Shape.hpp"

#include <functional>
#include <memory>

namespace Raz {

enum class RenderMode : unsigned int {
  POINT    = 0, // GL_POINTS
  //LINE     = 1, // GL_LINES
  TRIANGLE = 4  // GL_TRIANGLES
};

class SubmeshRenderer {
public:
  SubmeshRenderer() = default;
  explicit SubmeshRenderer(const Submesh& submesh, RenderMode renderMode = RenderMode::TRIANGLE) { load(submesh, renderMode); }

  RenderMode getRenderMode() const { return m_renderMode; }
  std::size_t getMaterialIndex() const { return m_materialIndex; }

  /// Sets a specific mode to render the submesh into.
  /// \param renderMode Render mode to apply.
  /// \param submesh Submesh to load the render mode's indices from.
  void setRenderMode(RenderMode renderMode, const Submesh& submesh);
  void setMaterialIndex(std::size_t materialIndex) { m_materialIndex = materialIndex; }

  /// Clones the submesh renderer.
  /// \warning This doesn't load anything onto the graphics card; the load() function must be called afterwards with a Submesh for this.
  /// \return Cloned submesh renderer.
  SubmeshRenderer clone() const;
  /// Loads the submesh's data (vertices & indices) onto the graphics card.
  /// \param submesh Submesh to load the data from.
  /// \param renderMode Primitive type to render the submesh with.
  void load(const Submesh& submesh, RenderMode renderMode = RenderMode::TRIANGLE);
  /// Draws the submesh in the scene.
  void draw() const;

private:
  void loadVertices(const Submesh& submesh);
  void loadIndices(const Submesh& submesh);

  VertexArray m_vao {};
  VertexBuffer m_vbo {};
  IndexBuffer m_ibo {};

  RenderMode m_renderMode = RenderMode::TRIANGLE;
  std::function<void(const VertexBuffer&, const IndexBuffer&)> m_renderFunc {};

  std::size_t m_materialIndex = 0;
};

} // namespace Raz

#endif // RAZ_SUBMESHRENDERER_HPP
