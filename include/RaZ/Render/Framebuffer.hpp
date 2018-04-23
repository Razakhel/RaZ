#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "glew/include/GL/glew.h"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Framebuffer {
public:
  Framebuffer(unsigned int width, unsigned int height, const std::string& vertShaderPath, const std::string& fragShaderPath);

  const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }
  const TexturePtr& getColorBuffer() const { return m_colorBuffer; }
  const TexturePtr& getNormalBuffer() const { return m_normalBuffer; }
  const ShaderProgram& getProgram() const { return m_program; }

  void bind() const;
  void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  void display() const;

  ~Framebuffer() { glDeleteFramebuffers(1, &m_index); }

private:
  GLuint m_index;
  TexturePtr m_depthBuffer;
  TexturePtr m_colorBuffer;
  TexturePtr m_normalBuffer;
  ShaderProgram m_program;
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
