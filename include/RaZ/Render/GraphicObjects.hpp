#pragma once

#ifndef RAZ_GRAPHICOBJECTS_HPP
#define RAZ_GRAPHICOBJECTS_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

namespace Raz {

class VertexArray {
public:
  VertexArray();
  VertexArray(const VertexArray&) = delete;
  VertexArray(VertexArray&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }

  void bind() const;
  void unbind() const;

  VertexArray& operator=(const VertexArray&) = delete;
  VertexArray& operator=(VertexArray&&) noexcept = default;

  ~VertexArray();

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

class VertexBuffer {
public:
  VertexBuffer();
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer(VertexBuffer&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }

  void bind() const;
  void unbind() const;

  VertexBuffer& operator=(const VertexBuffer&) = delete;
  VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

  ~VertexBuffer();

  unsigned int vertexCount {};

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

class IndexBuffer {
public:
  IndexBuffer();
  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer(IndexBuffer&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }

  void bind() const;
  void unbind() const;

  IndexBuffer& operator=(const IndexBuffer&) = delete;
  IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

  ~IndexBuffer();

  unsigned int lineIndexCount {};
  unsigned int triangleIndexCount {};

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

} // namespace Raz

#endif // RAZ_GRAPHICOBJECTS_HPP
