#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <vector>
#include <string>

#include "GL/glew.h"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class VertexArray {
public:
  VertexArray() { glGenVertexArrays(1, &m_index); }

  GLuint getIndex() const { return m_index; }

  void bind() const { glBindVertexArray(m_index); }

  ~VertexArray() { glDeleteVertexArrays(1, &m_index); }

private:
  GLuint m_index;
};

class VertexBuffer {
public:
  VertexBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<float>& getVertices() const { return m_vertices; }
  std::vector<float>& getVertices() { return m_vertices; }
  const std::vector<float>& getNormals() const { return m_normals; }
  std::vector<float>& getNormals() { return m_normals; }
  const std::vector<float>& getTexcoords() const { return m_texcoords; }
  std::vector<float>& getTexcoords() { return m_texcoords; }

  void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_index); }

  ~VertexBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index;
  std::vector<float> m_vertices;
  std::vector<float> m_normals;
  std::vector<float> m_texcoords;
};

class ElementBuffer {
public:
  ElementBuffer() { glGenBuffers(1, &m_index); }

  GLuint getIndex() const { return m_index; }
  const std::vector<unsigned int>& getVerticesIndices() const { return m_verticesIndices; }
  std::vector<unsigned int>& getVerticesIndices() { return m_verticesIndices; }
  const std::vector<unsigned int>& getTexcoordsIndices() const { return m_texcoordsIndices; }
  std::vector<unsigned int>& getTexcoordsIndices() { return m_texcoordsIndices; }
  const std::vector<unsigned int>& getNormalsIndices() const { return m_normalsIndices; }
  std::vector<unsigned int>& getNormalsIndices() { return m_normalsIndices; }

  void setVerticesIndices(const std::vector<unsigned int>& indices);

  void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index); }

  ~ElementBuffer() { glDeleteBuffers(1, &m_index); }

private:
  GLuint m_index;
  std::vector<unsigned int> m_verticesIndices;
  std::vector<unsigned int> m_texcoordsIndices;
  std::vector<unsigned int> m_normalsIndices;
};

class Mesh {
public:
  Mesh(const std::vector<float>& data, const std::vector<unsigned int>& indices) { load(data, indices); }
  Mesh(const std::string& fileName) { load(fileName); }

  const VertexArray& getVao() const { return m_vao; }
  const VertexBuffer& getVbo() const { return m_vbo; }
  const ElementBuffer& getEbo() const { return m_ebo; }
  const Texture& getTexture() const { return m_texture; }
  std::size_t getVertexCount() const { return m_vbo.getVertices().size(); }
  std::size_t getFaceCount() const { return m_ebo.getVerticesIndices().size(); }

  void load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
  void load(const std::string& fileName);
  void draw() const;

private:
  VertexArray m_vao;
  VertexBuffer m_vbo;
  ElementBuffer m_ebo;
  Texture m_texture;
};

} // namespace Raz

#endif // RAZ_MESH_HPP
