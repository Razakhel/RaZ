#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Render/Texture.hpp"

#include <cassert>
#include <vector>

namespace Raz {

class RenderShaderProgram;
class Texture;
class VertexShader;

/// Framebuffer class, handling buffers used for deferred rendering.
class Framebuffer {
  friend class RenderPass;

public:
  Framebuffer();
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&& fbo) noexcept;

  unsigned int getIndex() const noexcept { return m_index; }
  bool isEmpty() const noexcept { return (!hasDepthBuffer() && m_colorBuffers.empty()); }
  bool hasDepthBuffer() const noexcept { return (m_depthBuffer != nullptr); }
  const Texture& getDepthBuffer() const noexcept { assert("Error: Framebuffer doesn't contain a depth buffer." && hasDepthBuffer()); return *m_depthBuffer; }
  std::size_t getColorBufferCount() const noexcept { return m_colorBuffers.size(); }
  const Texture& getColorBuffer(std::size_t bufferIndex) const noexcept { return *m_colorBuffers[bufferIndex]; }

  /// Gives a basic vertex shader, to display the framebuffer.
  /// \return Basic display vertex shader.
  static VertexShader recoverVertexShader();

  /// Adds a write buffer texture.
  /// \param texture Buffer texture to be added.
  void addTextureBuffer(TexturePtr texture);
  /// Removes a write buffer texture.
  /// \param texture Buffer texture to be removed.
  void removeTextureBuffer(const TexturePtr& texture);
  /// Resizes the buffer textures.
  /// \param width Width to be resized to.
  /// \param height Height to be resized to.
  void resizeBuffers(unsigned int width, unsigned int height);
  /// Maps the buffers textures onto the graphics card.
  void mapBuffers() const;
  /// Binds the framebuffer and clears the color & depth buffers.
  void bind() const;
  /// Unbinds the framebuffer.
  void unbind() const;
  /// Displays the framebuffer.
  void display() const;

  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer& operator=(Framebuffer&& fbo) noexcept;

  ~Framebuffer();

private:
  unsigned int m_index {};
  TexturePtr m_depthBuffer {};
  std::vector<TexturePtr> m_colorBuffers {};
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
