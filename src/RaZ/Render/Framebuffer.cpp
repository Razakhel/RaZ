#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

namespace {

inline const MeshRenderer& getDisplaySurface() {
  // Creating a triangle large enough to cover the whole render frame:
  //
  //   3 | \                                3 | \
  //     |    \                               |  \
  //   2 |       \                          2 |    \
  //     |          \                         |     \
  //   1 ------------- \                    1 -------\
  //     |           |    \                   |     | \
  //   0 |           |       \              0 |     |   \
  //     |           |          \             |     |    \
  //  -1 -------------------------         -1 -------------
  //    -1     0     1     2     3           -1  0  1  2  3

  static const MeshRenderer surface(Mesh(Triangle(Vec3f(-1.f, -1.f, 0.f), Vec3f(3.f, -1.f, 0.f), Vec3f(-1.f, 3.f, 0.f)),
                                         Vec2f(0.f, 0.f), Vec2f(2.f, 0.f), Vec2f(0.f, 2.f)));
  return surface;
}

} // namespace

Framebuffer::Framebuffer() {
  Logger::debug("[Framebuffer] Creating...");
  Renderer::generateFramebuffer(m_index);
  Logger::debug("[Framebuffer] Created (ID: " + std::to_string(m_index) + ')');
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

  return VertexShader::loadFromSource(vertSource);
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
  Logger::debug("[Framebuffer] Mapping buffers (ID: " + std::to_string(m_index) + ")...");

  Renderer::bindFramebuffer(m_index);

  if (m_depthBuffer) {
    Logger::debug("[Framebuffer] Mapping depth buffer...");
    Renderer::setFramebufferTexture2D(FramebufferAttachment::DEPTH, TextureType::TEXTURE_2D, m_depthBuffer->getIndex(), 0);
  }

  if (!m_colorBuffers.empty()) {
    std::vector<DrawBuffer> drawBuffers(m_colorBuffers.size());

    for (std::size_t bufferIndex = 0; bufferIndex < m_colorBuffers.size(); ++bufferIndex) {
      Logger::debug("[Framebuffer] Mapping color buffer " + std::to_string(bufferIndex) + "...");

      const std::size_t colorBuffer = static_cast<unsigned int>(DrawBuffer::COLOR_ATTACHMENT0) + bufferIndex;

      Renderer::setFramebufferTexture2D(static_cast<FramebufferAttachment>(colorBuffer), TextureType::TEXTURE_2D, m_colorBuffers[bufferIndex]->getIndex(), 0);
      drawBuffers[bufferIndex] = static_cast<DrawBuffer>(colorBuffer);
    }

    Renderer::setDrawBuffers(static_cast<unsigned int>(drawBuffers.size()), drawBuffers.data());
  }

  if (!Renderer::isFramebufferComplete())
    Logger::error("Framebuffer is not complete.");

  unbind();

  Logger::debug("[Framebuffer] Mapped buffers");
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

  getDisplaySurface().draw();
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

  Logger::debug("[Framebuffer] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteFramebuffer(m_index);
  Logger::debug("[Framebuffer] Destroyed");
}

} // namespace Raz
