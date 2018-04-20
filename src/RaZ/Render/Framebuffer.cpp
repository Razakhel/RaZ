#include <iostream>

#include "RaZ/Render/Framebuffer.hpp"

namespace Raz {

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
  : m_program(std::make_unique<VertexShader>("../shaders/framebufferVert.glsl"),
              std::make_unique<FragmentShader>("../shaders/ssr.glsl")) {

  m_depthBuffer = std::make_shared<Texture>(width, height, true);
  m_colorBuffer = std::make_shared<Texture>(width, height);
  m_normalBuffer = std::make_shared<Texture>(width, height);
  m_viewport = std::make_shared<Mesh>(Vec3f({ -1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, -1.f, 0.f }),
                                      Vec3f({ -1.f, -1.f, 0.f }));
  glGenFramebuffers(1, &m_index);

  const std::array<GLenum, 2> colorBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[0], GL_TEXTURE_2D, m_colorBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[1], GL_TEXTURE_2D, m_normalBuffer->getIndex(), 0);

  glDrawBuffers(2, colorBuffers.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error: Framebuffer is not complete." << std::endl;
  unbind();

  m_viewport->load(m_program);
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_index);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::display() const {
  glClear(GL_COLOR_BUFFER_BIT);

  m_viewport->draw(m_program);
}

} // namespace Raz
