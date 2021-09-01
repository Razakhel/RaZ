#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Renderer.hpp"

#include <iostream>

namespace Raz {

Framebuffer::Framebuffer() {
  Renderer::generateFramebuffer(m_index);
}

Framebuffer::Framebuffer(Framebuffer&& fbo) noexcept
  : m_index{ std::exchange(fbo.m_index, std::numeric_limits<unsigned int>::max()) },
    m_depthBuffer{ std::exchange(fbo.m_depthBuffer, nullptr) },
    m_colorBuffers{ std::move(fbo.m_colorBuffers) } {
  mapBuffers();
}

VertexShader Framebuffer::recoverVertexShader() {
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  static constexpr std::string_view vertSource = R"(
    #version 330 core

    layout(location = 0) in vec2 vertPosition;
    layout(location = 1) in vec2 vertTexcoords;

    out vec2 fragTexcoords;

    void main() {
      fragTexcoords = vertTexcoords;

      gl_Position = vec4(vertPosition, 0.0, 1.0);
    }
  )";
#else // Emscripten/OpenGL ES
  // The version must be on the first line
  static constexpr std::string_view vertSource = R"(#version 300 es

    layout(location = 0) in vec2 vertPosition;
    layout(location = 1) in vec2 vertTexcoords;

    out vec2 fragTexcoords;

    void main() {
      fragTexcoords = vertTexcoords;

      gl_Position = vec4(vertPosition, 0.0, 1.0);
    }
  )";
#endif

//  return VertexShader::loadFromSource(vertSource);
  return {};
}

void Framebuffer::addTextureBuffer(const Texture& texture) {
  if (texture.getImage().getColorspace() == ImageColorspace::DEPTH) {
    assert("Error: There can be only one depth buffer in a Framebuffer." && !hasDepthBuffer());

    m_depthBuffer = &texture;
  } else {
    // Adding the color buffer only if it doesn't exist yet
    auto bufferIter = std::find(m_colorBuffers.cbegin(), m_colorBuffers.cend(), &texture);

    if (bufferIter == m_colorBuffers.cend())
      m_colorBuffers.emplace_back(&texture);
  }

  mapBuffers();
}

void Framebuffer::mapBuffers() const {

}

void Framebuffer::bind() const {
  Renderer::bindFramebuffer(m_index);
  Renderer::clear(MaskType::COLOR | MaskType::DEPTH);
}

void Framebuffer::unbind() const {
  Renderer::unbindFramebuffer();
}

void Framebuffer::display(const ShaderProgram& program) const {
  Renderer::clear(MaskType::COLOR);

  program.use();

  if (m_depthBuffer) {
    m_depthBuffer->activate();
    m_depthBuffer->bind();
  }

  for (const Texture* colorBuffer : m_colorBuffers) {
    colorBuffer->activate();
    colorBuffer->bind();
  }

  Mesh::drawUnitQuad();
}

void Framebuffer::resizeBuffers(unsigned int width, unsigned int height) {
  if (m_depthBuffer)
    m_depthBuffer->resize(width, height);

  for (const Texture* colorBuffer : m_colorBuffers)
    colorBuffer->resize(width, height);

  mapBuffers();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
  std::swap(m_index, fbo.m_index);
  std::swap(m_depthBuffer, fbo.m_depthBuffer);
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
