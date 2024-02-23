#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace Raz {

class RenderShaderProgram;
class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
class VertexShader;

/// Framebuffer class, handling buffers used for deferred rendering.
class Framebuffer {
  friend class RenderPass;

public:
  Framebuffer();
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&&) noexcept = default;

  unsigned int getIndex() const noexcept { return m_index; }
  bool isEmpty() const noexcept { return (!hasDepthBuffer() && m_colorBuffers.empty()); }
  bool hasDepthBuffer() const noexcept { return (m_depthBuffer != nullptr); }
  const Texture2D& getDepthBuffer() const noexcept { assert("Error: Framebuffer doesn't contain a depth buffer." && hasDepthBuffer()); return *m_depthBuffer; }
  std::size_t getColorBufferCount() const noexcept { return m_colorBuffers.size(); }
  const Texture2D& getColorBuffer(std::size_t bufferIndex) const noexcept { return *m_colorBuffers[bufferIndex].first; }

  /// Gives a basic vertex shader, to display the framebuffer.
  /// \return Basic display vertex shader.
  static VertexShader recoverVertexShader();

  /// Sets the write depth buffer texture.
  /// \param texture Depth buffer texture to be set; must have a depth colorspace.
  void setDepthBuffer(Texture2DPtr texture);
  /// Adds a write color buffer texture.
  /// \param texture Color buffer texture to be added; must have a non-depth colorspace.
  /// \param index Buffer's index (location of the shader's output value).
  void addColorBuffer(Texture2DPtr texture, unsigned int index);
  /// Removes a write buffer texture.
  /// \param texture Buffer texture to be removed.
  void removeTextureBuffer(const Texture2DPtr& texture);
  /// Removes the depth buffer.
  void clearDepthBuffer() { m_depthBuffer.reset(); }
  /// Removes all color buffers.
  void clearColorBuffers() { m_colorBuffers.clear(); }
  /// Removes both depth & color buffers.
  void clearTextureBuffers();
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
  Framebuffer& operator=(Framebuffer&&) noexcept = default;

  ~Framebuffer();

private:
  OwnerValue<unsigned int> m_index {};
  Texture2DPtr m_depthBuffer {};
  std::vector<std::pair<Texture2DPtr, unsigned int>> m_colorBuffers {};
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
