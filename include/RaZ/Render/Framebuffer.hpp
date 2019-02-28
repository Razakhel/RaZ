#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "GL/glew.h"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Framebuffer;
using FramebufferPtr = std::unique_ptr<Framebuffer>;

class Framebuffer {
public:
  Framebuffer(unsigned int width, unsigned int height, const std::string& fragShaderPath);

  const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }
  TexturePtr& getDepthBuffer() { return m_depthBuffer; }
  const TexturePtr& getColorBuffer() const { return m_colorBuffer; }
  TexturePtr& getColorBuffer() { return m_colorBuffer; }
  const TexturePtr& getNormalBuffer() const { return m_normalBuffer; }
  TexturePtr& getNormalBuffer() { return m_normalBuffer; }
  const ShaderProgram& getProgram() const { return m_program; }

  template <typename... Args>
  static FramebufferPtr create(Args&&... args) { return std::make_unique<Framebuffer>(std::forward<Args>(args)...); }

  void initBuffers() const;
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
