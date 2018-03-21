#pragma once

#ifndef RAZ_SUBMESH_HPP
#define RAZ_SUBMESH_HPP

#include <memory>

#include "RaZ/Render/GraphicObjects.hpp"

namespace Raz {

class Submesh {
public:
  Submesh() = default;

  const VertexArray& getVao() const { return m_vao; }
  VertexArray& getVao() { return m_vao; }
  const VertexBuffer& getVbo() const { return m_vbo; }
  VertexBuffer& getVbo() { return m_vbo; }
  const ElementBuffer& getEbo() const { return m_vao.getEbo(); }
  ElementBuffer& getEbo() { return m_vao.getEbo(); }
  const std::vector<Vertex>& getVertices() const { return m_vbo.getVertices(); }
  std::vector<Vertex>& getVertices() { return m_vbo.getVertices(); }
  const std::vector<unsigned int>& getIndices() const { return m_vao.getEbo().getIndices(); }
  std::vector<unsigned int>& getIndices() { return m_vao.getEbo().getIndices(); }
  std::size_t getMaterialIndex() const { return m_materialIndex; }
  std::size_t getVertexCount() const { return m_vbo.getVertices().size(); }
  std::size_t getIndexCount() const { return getEbo().getIndices().size(); }

  void setMaterialIndex(std::size_t materialIndex) { m_materialIndex = materialIndex; }

  void load() const;
  void draw() const;

private:
  VertexArray m_vao {};
  VertexBuffer m_vbo {};

  std::size_t m_materialIndex {};
};

using SubmeshPtr = std::unique_ptr<Submesh>;

} // namespace Raz

#endif // RAZ_SUBMESH_HPP
