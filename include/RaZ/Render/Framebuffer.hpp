#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

/// Framebuffer class, handling buffers used for deferred rendering.
class Framebuffer {
  friend class RenderPass;

public:
  Framebuffer();
  Framebuffer(unsigned int width, unsigned int height) : Framebuffer() { resizeBuffers(width, height); }
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&& fbo) noexcept;

  bool hasDepthBuffer() const { return (m_depthBuffer != nullptr); }
  bool isEmpty() const { return (!hasDepthBuffer() && m_colorBuffers.empty()); }
  const Texture& getDepthBuffer() const { assert("Error: Framebuffer doesn't contain a depth buffer." && hasDepthBuffer()); return *m_depthBuffer; }
  const Texture& getColorBuffer(std::size_t bufferIndex) const { return *m_colorBuffers[bufferIndex]; }

  /// Gives a basic vertex shader, to display the framebuffer.
  /// \return Basic display vertex shader.
  static VertexShader recoverVertexShader();

  /// Adds an existing buffer texture.
  /// \param texture Buffer texture to be added. This must commonly be a texture owned by the render graph.
  void addTextureBuffer(const Texture& texture);
  /// Maps the buffers textures onto the graphics card.
  void mapBuffers() const;
  /// Binds the framebuffer and clears the color & depth buffers.
  void bind() const;
  /// Unbinds the framebuffer.
  void unbind() const;
  /// Binds the buffers textures & displays the framebuffer.
  /// \param program Shader program to display with.
  void display(const ShaderProgram& program) const;
  /// Resizes the buffer textures.
  /// \param width Width to be resized to.
  /// \param height Height to be resized to.
  void resizeBuffers(unsigned int width, unsigned int height);

  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer& operator=(Framebuffer&& fbo) noexcept;

  ~Framebuffer();

private:
  unsigned int m_index {};
  const Texture* m_depthBuffer {};
  std::vector<const Texture*> m_colorBuffers {};
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
