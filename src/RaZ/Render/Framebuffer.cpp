#include <iostream>

#include "RaZ/Render/Texture.hpp"
#include "RaZ/Render/Framebuffer.hpp"

namespace Raz {

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_program(VertexShader("../shaders/framebufferVert.glsl"),
                                                                              FragmentShader("../shaders/ssr.glsl")) {
  m_depthBuffer = std::make_shared<Texture>(width, height, true);
  m_colorBuffer = std::make_shared<Texture>(width, height);
  m_normalBuffer = std::make_shared<Texture>(width, height);
  m_viewport = std::make_shared<Mesh>(Vec3f({ -1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, 1.f, 0.f }),
                                      Vec3f({ 1.f, -1.f, 0.f }),
                                      Vec3f({ -1.f, -1.f, 0.f }));
  glGenFramebuffers(1, &m_index);

  m_program.use();

  glUniform1i(glGetUniformLocation(m_program.getIndex(), "uniSceneDepthBuffer"), 0);
  glUniform1i(glGetUniformLocation(m_program.getIndex(), "uniSceneColorBuffer"), 1);
  glUniform1i(glGetUniformLocation(m_program.getIndex(), "uniSceneNormalBuffer"), 2);

  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalBuffer->getIndex(), 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error: Framebuffer is not complete." << std::endl;
  unbind();
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_index);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::display() const {
  glClear(GL_COLOR_BUFFER_BIT);

  m_program.use();

  glActiveTexture(GL_TEXTURE0);
  m_depthBuffer->bind();
  glActiveTexture(GL_TEXTURE1);
  m_colorBuffer->bind();
  glActiveTexture(GL_TEXTURE2);
  m_normalBuffer->bind();

  m_viewport->draw();
}

} // namespace Raz
