#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <memory>
#include <string>
#include <vector>

#include "GL/glew.h"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class ElementBuffer {
public:
  ElementBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<std::size_t>& getIndices() const { return m_indices; }
  std::vector<std::size_t>& getIndices() { return m_indices; }

  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index); }
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

  ~ElementBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index;
  std::vector<std::size_t> m_indices;
};

class VertexBuffer {
public:
  VertexBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<float>& getVertices() const { return m_vertices; }
  std::vector<float>& getVertices() { return m_vertices; }

  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_index); }
  void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

  ~VertexBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index;
  std::vector<float> m_vertices;
};

class VertexArray {
public:
  VertexArray() { glGenVertexArrays(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const ElementBuffer& getEbo() const { return m_ebo; }
  ElementBuffer& getEbo() { return m_ebo; }

  void bind() const { glBindVertexArray(m_index); }
  void unbind() const { glBindVertexArray(0); }

  ~VertexArray() { glDeleteVertexArrays(1, &m_index); }

private:
  GLuint m_index;
  ElementBuffer m_ebo;
};

class Mesh {
public:
  Mesh() = default;
  Mesh(const std::string& fileName) { import(fileName); }

  const VertexArray& getVao() const { return m_vao; }
  VertexArray& getVao() { return m_vao; }
  const VertexBuffer& getVbo() const { return m_vbo; }
  VertexBuffer& getVbo() { return m_vbo; }
  const ElementBuffer& getEbo() const { return m_vao.getEbo(); }
  ElementBuffer& getEbo() { return m_vao.getEbo(); }
  std::size_t getVertexCount() const { return m_vbo.getVertices().size(); }
  std::size_t getIndexCount() const { return m_vao.getEbo().getIndices().size(); }

  void import(const std::string& fileName);
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
