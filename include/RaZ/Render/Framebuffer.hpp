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
  const TexturePtr& getColorBuffer() const { return m_colorBuffer; }
  const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }

  void bind() const;
  void unbind() const;
  void display() const;

  ~Framebuffer() { glDeleteFramebuffers(1, &m_index); }

private:
  GLuint m_index;
  TexturePtr m_colorBuffer;
  TexturePtr m_depthBuffer;
  MeshPtr m_viewport;
  ShaderProgram m_program;
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
