#pragma once

#ifndef RAZ_SUBMESH_HPP
#define RAZ_SUBMESH_HPP

#include <memory>

#include "RaZ/Render/GraphicObjects.hpp"

namespace Raz {

enum class RenderMode : unsigned int {
  POINT    = 0, // GL_POINTS
  LINE     = 1, // GL_LINES
  TRIANGLE = 4  // GL_TRIANGLES
};

class Submesh;
using SubmeshPtr = std::unique_ptr<Submesh>;

class Submesh {
public:
  Submesh() = default;
  explicit Submesh(RenderMode renderMode) : m_renderMode{ renderMode } {}
  Submesh(const Submesh&) = delete;
  Submesh(Submesh&&) = default;

  const VertexArray& getVao() const { return m_vao; }
  const VertexBuffer& getVbo() const { return m_vbo; }
  const ElementBuffer& getEbo() const { return m_vao.getEbo(); }
  const std::vector<Vertex>& getVertices() const { return m_vbo.getVertices(); }
  std::vector<Vertex>& getVertices() { return m_vbo.getVertices(); }
  std::size_t getVertexCount() const { return m_vbo.getVertices().size(); }
  const std::vector<unsigned int>& getIndices() const { return m_vao.getEbo().getIndices(); }
  std::vector<unsigned int>& getIndices() { return m_vao.getEbo().getIndices(); }
  std::size_t getIndexCount() const { return getEbo().getIndices().size(); }
  RenderMode getRenderMode() const { return m_renderMode; }
  std::size_t getMaterialIndex() const { return m_materialIndex; }

  void setRenderMode(RenderMode renderMode) { m_renderMode = renderMode; }
  void setMaterialIndex(std::size_t materialIndex) { m_materialIndex = materialIndex; }

  template <typename... Args>
  static SubmeshPtr create(Args&&... args) { return std::make_unique<Submesh>(std::forward<Args>(args)...); }

  /// Loads the submesh's data (vertices & indices) onto the graphics card.
  void load() const;
  /// Draws the submesh in the scene.
  void draw() const;

  Submesh& operator=(const Submesh&) = delete;
  Submesh& operator=(Submesh&&) = default;

private:
  void loadVertices() const;
  void loadIndices() const;

  VertexArray m_vao {};
  VertexBuffer m_vbo {};
  RenderMode m_renderMode = RenderMode::TRIANGLE;

  std::size_t m_materialIndex {};
};

} // namespace Raz

#endif // RAZ_SUBMESH_HPP
