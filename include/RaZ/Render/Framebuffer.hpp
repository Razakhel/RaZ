#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

/// Framebuffer class, handling buffers used for deferred rendering.
class Framebuffer {
public:
  Framebuffer();
  explicit Framebuffer(ShaderProgram& program);
  Framebuffer(unsigned int width, unsigned int height, ShaderProgram& program) : Framebuffer(program) { resize(width, height); }
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&& fbo) noexcept;

  bool hasDepthBuffer() const { return (m_depthBuffer != nullptr); }
  const Texture& getDepthBuffer() const { assert("Error: Framebuffer doesn't contain a depth buffer." && m_depthBuffer); return *m_depthBuffer; }
  const std::vector<TexturePtr>& getColorBuffers() const { return m_colorBuffers; }

  /// Gives a basic vertex shader, to display the framebuffer.
  /// \return Basic display vertex shader.
  static VertexShader recoverVertexShader();

  /// Adds a depth buffer. There can be only one depth buffer in a single framebuffer.
  /// \param width Width of the depth buffer to be added.
  /// \param height Height of the depth buffer to be added.
  void addDepthBuffer(unsigned int width, unsigned int height);
  /// Adds a color buffer to the framebuffer.
  /// \param width Width of the color buffer to be added.
  /// \param height Height of the color buffer to be added.
  /// \param colorspace Colorspace of the color buffer to be added. Passing ImageColorspace::DEPTH is equivalent to calling addDepthBuffer().
  void addColorBuffer(unsigned int width, unsigned int height, ImageColorspace colorspace);
  /// Initializes the buffers uniforms' indices to be bound later.
  /// \param program Shader program to which to send the uniforms.
  void initBuffers(const ShaderProgram& program) const;
  /// Maps the buffers textures onto the graphics card.
  void mapBuffers() const;
  /// Binds the framebuffer and clears the color & depth buffers.
  void bind() const;
  /// Unbinds the framebuffer.
  void unbind() const;
  /// Binds the buffers textures & displays the framebuffer.
  /// \param program Shader program to display with.
  void display(const ShaderProgram& program) const;
  /// Resizes the buffers textures.
  /// \param width Width to be resized to.
  /// \param height Height to be resized to.
  void resize(unsigned int width, unsigned int height);

  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer& operator=(Framebuffer&& fbo) noexcept;

  ~Framebuffer();

private:
  unsigned int m_index {};
  TexturePtr m_depthBuffer;
  std::vector<TexturePtr> m_colorBuffers;
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
