#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <utility>

namespace Raz {

namespace {

inline void drawDisplaySurface() {
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

  static const std::pair<VertexArray, VertexBuffer> vertexObjects = [] () {
    VertexArray vao;
    VertexBuffer vbo;

    vao.bind();
    vbo.bind();

    static constexpr std::array<Vec2f, 6> vertices = { Vec2f(-1.f, -1.f), Vec2f(0.f, 0.f),
                                                       Vec2f( 3.f, -1.f), Vec2f(2.f, 0.f),
                                                       Vec2f(-1.f,  3.f), Vec2f(0.f, 2.f) };
    Renderer::sendBufferData(BufferType::ARRAY_BUFFER, static_cast<std::ptrdiff_t>(sizeof(Vec2f) * 12), vertices.data(), BufferDataUsage::STATIC_DRAW);

    Renderer::setVertexAttrib(0, AttribDataType::FLOAT, 2, sizeof(Vec2f) * 2, 0); // Position
    Renderer::setVertexAttrib(1, AttribDataType::FLOAT, 2, sizeof(Vec2f) * 2, sizeof(Vec2f)); // Texcoords
    Renderer::enableVertexAttribArray(0);
    Renderer::enableVertexAttribArray(1);

    vbo.unbind();
    vao.unbind();

    return std::make_pair(std::move(vao), std::move(vbo));
  }();

  vertexObjects.first.bind();
  Renderer::drawArrays(PrimitiveType::TRIANGLES, 3);
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
  mapBuffers(); // TODO: may be unnecessary
}

VertexShader Framebuffer::recoverVertexShader() {
  static constexpr std::string_view vertSource = R"(
    layout(location = 0) in vec2 vertPosition;
    layout(location = 1) in vec2 vertTexcoords;

    out vec2 fragTexcoords;

    void main() {
      fragTexcoords = vertTexcoords;
      gl_Position   = vec4(vertPosition, 0.0, 1.0);
    }
  )";

  return VertexShader::loadFromSource(vertSource);
}

void Framebuffer::setDepthBuffer(Texture2DPtr texture) {
  if (texture->getColorspace() != TextureColorspace::DEPTH)
    throw std::invalid_argument("Error: Invalid depth buffer");

  m_depthBuffer = std::move(texture);

  mapBuffers();
}

void Framebuffer::addColorBuffer(Texture2DPtr texture, unsigned int index) {
  if (texture->getColorspace() == TextureColorspace::DEPTH || texture->getColorspace() == TextureColorspace::INVALID)
    throw std::invalid_argument("Error: Invalid color buffer");

  const auto bufferIt = std::find_if(m_colorBuffers.cbegin(), m_colorBuffers.cend(), [&texture, index] (const auto& colorBuffer) {
    return (texture == colorBuffer.first && index == colorBuffer.second);
  });

  // Adding the color buffer only if it doesn't exist yet
  if (bufferIt == m_colorBuffers.cend())
    m_colorBuffers.emplace_back(std::move(texture), index);

  mapBuffers();
}

void Framebuffer::removeTextureBuffer(const Texture2DPtr& texture) {
  if (texture == m_depthBuffer) {
    m_depthBuffer.reset();
  } else {
    const auto bufferIt = std::remove_if(m_colorBuffers.begin(), m_colorBuffers.end(), [&texture] (const auto& buffer) {
      return (texture == buffer.first);
    });
    m_colorBuffers.erase(bufferIt, m_colorBuffers.end());
  }

  mapBuffers();
}

void Framebuffer::clearTextureBuffers() {
  clearDepthBuffer();
  clearColorBuffers();
}

void Framebuffer::resizeBuffers(unsigned int width, unsigned int height) {
  if (m_depthBuffer)
    m_depthBuffer->resize(width, height);

  for (const auto& [colorBuffer, _] : m_colorBuffers)
    colorBuffer->resize(width, height);

  mapBuffers(); // TODO: may be unnecessary
}

void Framebuffer::mapBuffers() const {
  Logger::debug("[Framebuffer] Mapping buffers (ID: " + std::to_string(m_index) + ")...");

  Renderer::bindFramebuffer(m_index);

  if (m_depthBuffer) {
    Logger::debug("[Framebuffer] Mapping depth buffer...");
    Renderer::setFramebufferTexture2D(FramebufferAttachment::DEPTH, m_depthBuffer->getIndex(), 0, TextureType::TEXTURE_2D);
  }

  if (!m_colorBuffers.empty()) {
    std::vector<DrawBuffer> drawBuffers(m_colorBuffers.size(), DrawBuffer::NONE);

    for (const auto& [colorBuffer, bufferIndex] : m_colorBuffers) {
      Logger::debug("[Framebuffer] Mapping color buffer " + std::to_string(bufferIndex) + "...");

      const std::size_t colorAttachment = static_cast<unsigned int>(DrawBuffer::COLOR_ATTACHMENT0) + bufferIndex;

      Renderer::setFramebufferTexture2D(static_cast<FramebufferAttachment>(colorAttachment), colorBuffer->getIndex(), 0, TextureType::TEXTURE_2D);

      if (bufferIndex >= drawBuffers.size())
        drawBuffers.resize(bufferIndex + 1, DrawBuffer::NONE);
      drawBuffers[bufferIndex] = static_cast<DrawBuffer>(colorAttachment);
    }

    Renderer::setDrawBuffers(static_cast<unsigned int>(drawBuffers.size()), drawBuffers.data());
  }

  unbind();

  Logger::debug("[Framebuffer] Mapped buffers");
}

void Framebuffer::bind() const {
  Renderer::bindFramebuffer(m_index);
  Renderer::clear(MaskType::COLOR | MaskType::DEPTH | MaskType::STENCIL);
}

void Framebuffer::unbind() const {
  Renderer::unbindFramebuffer();
}

void Framebuffer::display() const {
  Renderer::clear(MaskType::COLOR);
  drawDisplaySurface();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fbo) noexcept {
  std::swap(m_index, fbo.m_index);
  m_depthBuffer  = std::move(fbo.m_depthBuffer);
  m_colorBuffers = std::move(fbo.m_colorBuffers);

  mapBuffers(); // TODO: may be unnecessary

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
