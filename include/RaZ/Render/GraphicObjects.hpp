#pragma once

#ifndef RAZ_GRAPHICOBJECTS_HPP
#define RAZ_GRAPHICOBJECTS_HPP

#include "RaZ/Math/Vector.hpp"

#include <vector>

namespace Raz {

struct Vertex {
  Vec3f position {};
  Vec2f texcoords {};
  Vec3f normal {};
  Vec3f tangent {};

  std::size_t operator()() const { return tangent.hash(normal.hash(texcoords.hash(position.hash(0)))); }
  bool operator==(const Vertex& vert) const { return (position == vert.position)
                                                  && (texcoords == vert.texcoords)
                                                  && (normal == vert.normal)
                                                  && (tangent == vert.tangent); }
};

class VertexArray {
public:
  VertexArray();
  VertexArray(const VertexArray&) = delete;
  VertexArray(VertexArray&& vertexArray) noexcept;

  unsigned int getIndex() const { return m_index; }

  void bind() const;
  void unbind() const;

  VertexArray& operator=(const VertexArray&) = delete;
  VertexArray& operator=(VertexArray&& vertexArray) noexcept;

  ~VertexArray();

private:
  unsigned int m_index {};
};

class VertexBuffer {
public:
  VertexBuffer();
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;

  unsigned int getIndex() const { return m_index; }
  const std::vector<Vertex>& getVertices() const { return m_vertices; }
  std::vector<Vertex>& getVertices() { return m_vertices; }

  void bind() const;
  void unbind() const;

  VertexBuffer& operator=(const VertexBuffer&) = delete;
  VertexBuffer& operator=(VertexBuffer&& vertexBuffer) noexcept;

  ~VertexBuffer();

  unsigned int vertexCount {};

private:
  unsigned int m_index {};
  std::vector<Vertex> m_vertices {};
};

class IndexBuffer {
public:
  IndexBuffer();
  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer(IndexBuffer&& indexBuffer) noexcept;

  unsigned int getIndex() const { return m_index; }
  const std::vector<unsigned int>& getLineIndices() const { return m_lineIndices; }
  std::vector<unsigned int>& getLineIndices() { return m_lineIndices; }
  const std::vector<unsigned int>& getTriangleIndices() const { return m_triangleIndices; }
  std::vector<unsigned int>& getTriangleIndices() noexcept { return m_triangleIndices; }

  void bind() const;
  void unbind() const;

  IndexBuffer& operator=(const IndexBuffer&) = delete;
  IndexBuffer& operator=(IndexBuffer&& indexBuffer) noexcept;

  ~IndexBuffer();

  unsigned int lineIndexCount {};
  unsigned int triangleIndexCount {};

private:
  unsigned int m_index {};
  std::vector<unsigned int> m_lineIndices {};
  std::vector<unsigned int> m_triangleIndices {};
};

} // namespace Raz

#endif // RAZ_GRAPHICOBJECTS_HPP
