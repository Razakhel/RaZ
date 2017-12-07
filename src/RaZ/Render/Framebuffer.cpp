#include <iostream>

#include "RaZ/Render/Texture.hpp"
#include "RaZ/Render/Framebuffer.hpp"

namespace Raz {

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
  : m_program(VertexShader("../shaders/framebufferVert.glsl"),
              FragmentShader("../shaders/framebufferFrag.glsl")) {
  m_texture = std::make_shared<Texture>(width, height);
  m_viewport = std::make_shared<Mesh>(Vec3f({ -1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, -1.f, 0.f }),
                                      Vec3f({ -1.f, -1.f, 0.f }));
  glGenFramebuffers(1, &m_index);
  glGenRenderbuffers(1, &m_rboIndex);

  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getIndex(), 0);

  glBindRenderbuffer(GL_RENDERBUFFER, m_rboIndex);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboIndex);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  unbind();
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_index);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  m_texture->unbind();
}

void Framebuffer::display() const {
  glClear(GL_COLOR_BUFFER_BIT);

  m_program.use();
  m_texture->bind();
  m_viewport->draw();
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &m_index);
  glDeleteRenderbuffers(1, &m_rboIndex);
}

} // namespace Raz
