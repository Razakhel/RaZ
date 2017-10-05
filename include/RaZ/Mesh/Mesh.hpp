#pragma once

#ifndef RAZ_MESH_HPP
#define RAZ_MESH_HPP

#include <vector>
#include <string>

#include "GL/glew.h"

namespace Raz {

class VertexArray {
public:
  VertexArray() { glGenVertexArrays(1, &index); }

  GLuint getIndex() const { return index; }

  void bind() { glBindVertexArray(index); }

  ~VertexArray() { glDeleteVertexArrays(1, &index); }

private:
  GLuint index;
};

class VertexBuffer {
public:
  VertexBuffer() { glGenBuffers(1, &index); }

  GLuint getIndex() const { return index; }
  const std::vector<float>& getVertices() const { return vertices; }
  std::vector<float>& getVertices() { return vertices; }

  void bind() { glBindBuffer(GL_ARRAY_BUFFER, index); }

  ~VertexBuffer() { glDeleteBuffers(1, &index); }

private:
  GLuint index;
  std::vector<float> vertices;
};

class ElementBuffer {
public:
  ElementBuffer() { glGenBuffers(1, &index); }

  GLuint getIndex() const { return index; }
  const std::vector<unsigned int>& getIndices() const { return indices; }
  std::vector<unsigned int>& getIndices() { return indices; }

  void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index); }

  ~ElementBuffer() { glDeleteBuffers(1, &index); }

private:
  GLuint index;
  std::vector<unsigned int> indices;
};

class Texture {
public:
  Texture() { glGenTextures(1, &index); }

  GLuint getIndex() const { return index; }

  void bind() { glBindTexture(GL_TEXTURE_2D, index); }

  ~Texture() { glDeleteTextures(1, &index); }

private:
  GLuint index;
};

class Mesh {
public:
  Mesh(const std::string& fileName) { load(fileName); }

  VertexArray& getVao() { return vao; }
  VertexBuffer& getVbo() { return vbo; }
  ElementBuffer& getEbo() { return ebo; }
  std::size_t getVertexCount() const { return vbo.getVertices().size(); }
  std::size_t getFaceCount() const { return ebo.getIndices().size(); }

  void load(const std::string& fileName);

private:
  VertexArray vao;
  VertexBuffer vbo;
  ElementBuffer ebo;
};

} // namespace Raz

#endif // RAZ_MESH_HPP
