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
  Framebuffer(unsigned int width, unsigned int height, ShaderProgram& program);
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&& fbo) noexcept;

  const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }
  const TexturePtr& getNormalBuffer() const { return m_normalBuffer; }
  const TexturePtr& getColorBuffer() const { return m_colorBuffer; }

  /// Initializes the buffers uniforms' indices to be bound later.
  /// \param program Shader program to which to send the uniforms.
  void initBuffers(const ShaderProgram& program) const;
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
  /// Assigns a basic vertex shader to the given program, to display the framebuffer.
  /// \param program Shader program to assign the vertex shader to.
  void assignVertexShader(ShaderProgram& program) const;

  Framebuffer& operator=(const Framebuffer&) = delete;
  Framebuffer& operator=(Framebuffer&& fbo) noexcept;

  ~Framebuffer();

private:
  unsigned int m_index {};
  TexturePtr m_depthBuffer {};
  TexturePtr m_normalBuffer {};
  TexturePtr m_colorBuffer {};
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
