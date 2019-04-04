#include <iostream>

#include "GL/glew.h"
#include "RaZ/Render/Framebuffer.hpp"

namespace Raz {

Framebuffer::Framebuffer() {
  glGenFramebuffers(1, &m_index);
}

Framebuffer::Framebuffer(ShaderProgram& program) : Framebuffer() {
  assignVertexShader(program);
  initBuffers(program);
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height, ShaderProgram& program) : Framebuffer(program) {
  resize(width, height);

  constexpr std::array<GLenum, 2> colorBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[0], GL_TEXTURE_2D, m_normalBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[1], GL_TEXTURE_2D, m_colorBuffer->getIndex(), 0);

  glDrawBuffers(2, colorBuffers.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error: Framebuffer is not complete." << std::endl;
  unbind();
}

void Framebuffer::initBuffers(const ShaderProgram& program) const {
  bind();

  program.use();
  program.sendUniform("uniSceneBuffers.depth",  0);
  program.sendUniform("uniSceneBuffers.normal", 1);
  program.sendUniform("uniSceneBuffers.color",  2);

  unbind();
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_index);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::display(const ShaderProgram& program) const {
  glClear(GL_COLOR_BUFFER_BIT);

  Texture::activate(0);
  m_depthBuffer->bind();

  Texture::activate(1);
  m_normalBuffer->bind();

  Texture::activate(2);
  m_colorBuffer->bind();

  program.use();
  Mesh::drawUnitQuad();
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
  m_depthBuffer  = Texture::create(width, height, ImageColorspace::DEPTH);
  m_normalBuffer = Texture::create(width, height, ImageColorspace::RGB);
  m_colorBuffer  = Texture::create(width, height, ImageColorspace::RGBA);
}

void Framebuffer::assignVertexShader(ShaderProgram& program) const {
  static const std::string vertSource = R"(
    #version 330 core

    layout (location = 0) in vec2 vertPosition;
    layout (location = 1) in vec2 vertTexcoords;

    out vec2 fragTexcoords;

    void main() {
      fragTexcoords = vertTexcoords;

      gl_Position = vec4(vertPosition, 0.0, 1.0);
    }
  )";

  program.setVertexShader(VertexShader::loadFromSource(vertSource));
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &m_index);
}

} // namespace Raz
