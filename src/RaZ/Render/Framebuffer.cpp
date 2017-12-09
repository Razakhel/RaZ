#include <iostream>

#include "RaZ/Render/Texture.hpp"
#include "RaZ/Render/Framebuffer.hpp"

namespace Raz {

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_program(VertexShader("../shaders/framebufferVert.glsl"),
                                                                              FragmentShader("../shaders/framebufferFrag.glsl")) {
  m_colorBuffer = std::make_shared<Texture>(width, height);
  m_depthBuffer = std::make_shared<Texture>(width, height, true);
  m_viewport = std::make_shared<Mesh>(Vec3f({ -1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, -1.f, 0.f }),
                                      Vec3f({ -1.f, -1.f, 0.f }));
  glGenFramebuffers(1, &m_index);

  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->getIndex(), 0);

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
}

void Framebuffer::display() const {
  glClear(GL_COLOR_BUFFER_BIT);

  m_program.use();
  m_colorBuffer->bind();
  m_viewport->draw();
}

} // namespace Raz
