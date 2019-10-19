#include "GL/glew.h"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Renderer.hpp"

#include <iostream>

namespace Raz {

Framebuffer::Framebuffer() {
  Renderer::generateFramebuffer(m_index);
}

Framebuffer::Framebuffer(ShaderProgram& program) : Framebuffer() {
  assignVertexShader(program);
  initBuffers(program);
}

Framebuffer::Framebuffer(Framebuffer&& fbo) noexcept
  : m_index{ std::exchange(fbo.m_index, std::numeric_limits<unsigned int>::max()) },
    m_depthBuffer{ std::move(fbo.m_depthBuffer) },
    m_colorBuffer{ std::move(fbo.m_colorBuffer) },
    m_normalBuffer{ std::move(fbo.m_normalBuffer) } {
  mapBuffers();
}

void Framebuffer::assignVertexShader(ShaderProgram& program) {
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

void Framebuffer::initBuffers(const ShaderProgram& program) const {
  bind();

  program.use();
  program.sendUniform("uniSceneBuffers.depth",  0);
  program.sendUniform("uniSceneBuffers.color",  1);
  program.sendUniform("uniSceneBuffers.normal", 2);

  unbind();
}

void Framebuffer::mapBuffers() const {
  constexpr std::array<GLenum, 2> colorBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

  bind();

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[0], GL_TEXTURE_2D, m_colorBuffer->getIndex(), 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorBuffers[1], GL_TEXTURE_2D, m_normalBuffer->getIndex(), 0);

  glDrawBuffers(static_cast<int>(colorBuffers.size()), colorBuffers.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cerr << "Error: Framebuffer is not complete." << std::endl;

  unbind();
}

void Framebuffer::bind() const {
  Renderer::bindFramebuffer(m_index);
  Renderer::clear(MaskType::COLOR, MaskType::DEPTH);
}

void Framebuffer::unbind() const {
  Renderer::unbindFramebuffer();
}

void Framebuffer::display(const ShaderProgram& program) const {
  Renderer::clear(MaskType::COLOR);

  program.use();

  Renderer::activateTexture(0);
  m_depthBuffer->bind();

  Renderer::activateTexture(1);
  m_colorBuffer->bind();

  Renderer::activateTexture(2);
  m_normalBuffer->bind();

  Mesh::drawUnitQuad();
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
  m_depthBuffer  = Texture::create(width, height, ImageColorspace::DEPTH);
  m_colorBuffer  = Texture::create(width, height, ImageColorspace::RGBA);
  m_normalBuffer = Texture::create(width, height, ImageColorspace::RGB);

  mapBuffers();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
  std::swap(m_index, fbo.m_index);
  m_depthBuffer  = std::move(fbo.m_depthBuffer);
  m_colorBuffer  = std::move(fbo.m_colorBuffer);
  m_normalBuffer = std::move(fbo.m_normalBuffer);

  mapBuffers();

  return *this;
}

Framebuffer::~Framebuffer() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteFramebuffer(m_index);
}

} // namespace Raz
