#pragma once

#ifndef RAZ_SUBMESH_HPP
#define RAZ_SUBMESH_HPP

#include "RaZ/Render/GraphicObjects.hpp"

#include <functional>
#include <memory>

namespace Raz {

enum class RenderMode : unsigned int {
  POINT    = 0, // GL_POINTS
  //LINE     = 1, // GL_LINES
  TRIANGLE = 4  // GL_TRIANGLES
};

class Submesh {
public:
  explicit Submesh(RenderMode renderMode = RenderMode::TRIANGLE) { setRenderMode(renderMode); }
  Submesh(const Submesh&) = delete;
  Submesh(Submesh&&) = default;

  const std::vector<Vertex>& getVertices() const { return m_vbo.getVertices(); }
  std::vector<Vertex>& getVertices() { return m_vbo.getVertices(); }
  std::size_t getVertexCount() const { return getVertices().size(); }
  const std::vector<unsigned int>& getLineIndices() const { return m_ibo.getLineIndices(); }
  std::vector<unsigned int>& getLineIndices() { return m_ibo.getLineIndices(); }
  std::size_t getLineIndexCount() const { return getLineIndices().size(); }
  const std::vector<unsigned int>& getTriangleIndices() const { return m_ibo.getTriangleIndices(); }
  std::vector<unsigned int>& getTriangleIndices() { return m_ibo.getTriangleIndices(); }
  std::size_t getTriangleIndexCount() const { return getTriangleIndices().size(); }
  RenderMode getRenderMode() const { return m_renderMode; }
  std::size_t getMaterialIndex() const { return m_materialIndex; }

  void setRenderMode(RenderMode renderMode);
  void setMaterialIndex(std::size_t materialIndex) { m_materialIndex = materialIndex; }

  /// Loads the submesh's data (vertices & indices) onto the graphics card.
  void load() const;
  /// Draws the submesh in the scene.
  void draw() const;

  Submesh& operator=(const Submesh&) = delete;
  Submesh& operator=(Submesh&&) noexcept = default;

private:
  void loadVertices() const;
  void loadIndices() const;

  VertexArray m_vao {};
  VertexBuffer m_vbo {};
  IndexBuffer m_ibo {};
  RenderMode m_renderMode = RenderMode::TRIANGLE;
  std::function<void(const Submesh&)> m_renderFunc {};

  std::size_t m_materialIndex = std::numeric_limits<std::size_t>::max();
};

} // namespace Raz

#endif // RAZ_SUBMESH_HPP
