#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Renderer.hpp"

#include <iostream>

namespace Raz {

Framebuffer::Framebuffer() {
  Renderer::generateFramebuffer(m_index);
}

Framebuffer::Framebuffer(ShaderProgram& program) : Framebuffer() {
  program.setVertexShader(recoverVertexShader());
  initBuffers(program);
}

Framebuffer::Framebuffer(Framebuffer&& fbo) noexcept
  : m_index{ std::exchange(fbo.m_index, std::numeric_limits<unsigned int>::max()) },
    m_depthBuffer{ std::move(fbo.m_depthBuffer) },
    m_colorBuffers{ std::move(fbo.m_colorBuffers) } {
  mapBuffers();
}

VertexShader Framebuffer::recoverVertexShader() {
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

  return VertexShader::loadFromSource(vertSource);
}

void Framebuffer::addDepthBuffer(unsigned int width, unsigned int height) {
  assert("Error: There can be only one depth buffer in a Framebuffer." && !hasDepthBuffer());

  m_depthBuffer = Texture::create(width, height, ImageColorspace::DEPTH, false);
  mapBuffers();
}

void Framebuffer::addColorBuffer(unsigned int width, unsigned int height, ImageColorspace colorspace) {
  if (colorspace == ImageColorspace::DEPTH) {
    addDepthBuffer(width, height);
    return;
  }

  m_colorBuffers.emplace_back(Texture::create(width, height, colorspace, false));
  mapBuffers();
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
  bind();

  Renderer::setFramebufferTexture2D(FramebufferAttachment::DEPTH, TextureType::TEXTURE_2D, m_depthBuffer->getIndex(), 0);

  std::vector<DrawBuffer> drawBuffers(m_colorBuffers.size());

  for (std::size_t bufferIndex = 0; bufferIndex < m_colorBuffers.size(); ++bufferIndex) {
    std::size_t colorBuffer = static_cast<unsigned int>(DrawBuffer::COLOR_ATTACHMENT0) + bufferIndex;

    Renderer::setFramebufferTexture2D(static_cast<FramebufferAttachment>(colorBuffer), TextureType::TEXTURE_2D, m_colorBuffers[bufferIndex]->getIndex(), 0);
    drawBuffers[bufferIndex] = static_cast<DrawBuffer>(colorBuffer);
  }

  Renderer::setDrawBuffers(static_cast<unsigned int>(drawBuffers.size()), drawBuffers.data());

  if (!Renderer::isFramebufferComplete())
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

  for (std::size_t bufferIndex = 0; bufferIndex < m_colorBuffers.size(); ++bufferIndex) {
    Renderer::activateTexture(static_cast<unsigned int>(bufferIndex + 1));
    m_colorBuffers[bufferIndex]->bind();
  }

  Mesh::drawUnitQuad();
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
  m_depthBuffer = Texture::create(width, height, ImageColorspace::DEPTH);

  for (TexturePtr& colorBuffer : m_colorBuffers)
    colorBuffer = Texture::create(width, height, colorBuffer->getImage().getColorspace(), false);

  mapBuffers();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
  std::swap(m_index, fbo.m_index);
  m_depthBuffer  = std::move(fbo.m_depthBuffer);
  m_colorBuffers = std::move(fbo.m_colorBuffers);

  mapBuffers();

  return *this;
}

Framebuffer::~Framebuffer() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteFramebuffer(m_index);
}

} // namespace Raz
