#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/GraphicObjects.hpp"

namespace Raz {

class Mesh {
public:
  explicit Mesh(const std::string& fileName) { importFromFile(fileName); }
  Mesh(const Vec3f& pos1, const Vec3f& pos2, const Vec3f& pos3);
  Mesh(const Vec3f& posTopLeft, const Vec3f& posTopRight, const Vec3f& posBottomRight, const Vec3f& posBottomLeft);

  const VertexArray& getVao() const { return m_vao; }
  VertexArray& getVao() { return m_vao; }
  const VertexBuffer& getVbo() const { return m_vbo; }
  VertexBuffer& getVbo() { return m_vbo; }
  const ElementBuffer& getEbo() const { return m_vao.getEbo(); }
  ElementBuffer& getEbo() { return m_vao.getEbo(); }
  std::size_t getVertexCount() const { return m_vbo.getVertices().size(); }
  std::size_t getIndexCount() const { return getEbo().getIndices().size(); }

  void importFromFile(const std::string& fileName);
  void load();
  void draw() const;

private:
  void importOff(std::ifstream& file);
  void importObj(std::ifstream& file);

  VertexArray m_vao;
  VertexBuffer m_vbo;
};

using MeshPtr = std::shared_ptr<Mesh>;

} // namespace Raz

#endif // RAZ_MESH_HPP
