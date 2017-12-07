#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"
#include "glew/include/GL/glew.h"

namespace Raz {

class Framebuffer {
public:
  Framebuffer(unsigned int width, unsigned int height);

  GLuint getIndex() const { return m_index; }
  const TexturePtr& getTexture() const { return m_texture; }

  void bind() const;
  void unbind() const;
  void display() const;

  ~Framebuffer();

private:
  GLuint m_index;
  GLuint m_rboIndex;
  TexturePtr m_texture;
  MeshPtr m_viewport;
  ShaderProgram m_program;
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
