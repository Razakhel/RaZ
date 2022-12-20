#pragma once

#ifndef RAZ_RENDERPASS_HPP
#define RAZ_RENDERPASS_HPP

#include "RaZ/Data/Graph.hpp"
#include "RaZ/Render/Framebuffer.hpp"
#if !defined(USE_OPENGL_ES)
#include "RaZ/Render/RenderTimer.hpp"
#endif
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class RenderPass final : public GraphNode<RenderPass> {
  friend class RenderGraph;

public:
  RenderPass() = default;
  RenderPass(VertexShader&& vertShader, FragmentShader&& fragShader, std::string passName = {}) noexcept
    : m_name{ std::move(passName) }, m_program(std::move(vertShader), std::move(fragShader)) {}
  explicit RenderPass(FragmentShader&& fragShader, std::string passName = {}) noexcept
    : RenderPass(Framebuffer::recoverVertexShader(), std::move(fragShader), std::move(passName)) {}
  RenderPass(const RenderPass&) = delete;
  RenderPass(RenderPass&&) noexcept = default;

  bool isEnabled() const { return m_enabled; }
  const std::string& getName() const { return m_name; }
  const RenderShaderProgram& getProgram() const { return m_program; }
  RenderShaderProgram& getProgram() { return m_program; }
  std::size_t getReadTextureCount() const noexcept { return m_program.getTextureCount(); }
  const Texture& getReadTexture(std::size_t textureIndex) const noexcept { return m_program.getTexture(textureIndex); }
  bool hasReadTexture(const std::string& uniformName) const noexcept { return m_program.hasTexture(uniformName); }
  const Texture& getReadTexture(const std::string& uniformName) const noexcept { return m_program.getTexture(uniformName); }
  const Framebuffer& getFramebuffer() const { return m_writeFramebuffer; }
  /// Recovers the elapsed time (in milliseconds) of the pass' execution.
  /// \note This action is not available with OpenGL ES and will always return 0.
  /// \return Time taken to execute the pass.
  float recoverElapsedTime() const noexcept {
#if !defined(USE_OPENGL_ES)
    return m_timer.recoverTime();
#else
    return 0.f;
#endif
  }

  void setName(std::string name) noexcept { m_name = std::move(name); }
  void setProgram(RenderShaderProgram&& program) { m_program = std::move(program); }

  /// Checks that the current render pass is valid, that is, if none of its buffer has been defined as both read & write.
  /// \return True if the render pass is valid, false otherwise.
  /// \see RenderGraph::isValid()
  bool isValid() const;
  void addReadTexture(TexturePtr texture, const std::string& uniformName);
  void removeReadTexture(const Texture& texture) { m_program.removeTexture(texture); }
  void clearReadTextures() { m_program.clearTextures(); }
  /// Sets the write depth buffer texture.
  /// \param texture Depth buffer texture to be set; must have a depth colorspace.
  void setWriteDepthTexture(Texture2DPtr texture) { m_writeFramebuffer.setDepthBuffer(std::move(texture)); }
  /// Adds a write color buffer texture.
  /// \param texture Color buffer texture to be added; must have a non-depth colorspace.
  /// \param index Buffer's index (location of the shader's output value).
  void addWriteColorTexture(Texture2DPtr texture, unsigned int index) { m_writeFramebuffer.addColorBuffer(std::move(texture), index); }
  void removeWriteTexture(const Texture2DPtr& texture) { m_writeFramebuffer.removeTextureBuffer(texture); }
  void clearWriteTextures() { m_writeFramebuffer.clearTextureBuffers(); }
  /// Resizes the render pass' write buffer textures.
  /// \param width New buffers width.
  /// \param height New buffers height.
  void resizeWriteBuffers(unsigned int width, unsigned int height) { m_writeFramebuffer.resizeBuffers(width, height); }
  /// Changes the render pass' enabled state.
  /// \param enabled True if the render pass should be enabled, false if it should be disabled.
  void enable(bool enabled = true) { m_enabled = enabled; }
  /// Disables the render pass.
  void disable() { enable(false); }
  /// Executes the render pass.
  void execute() const;

  RenderPass& operator=(const RenderPass&) = delete;
  RenderPass& operator=(RenderPass&&) noexcept = default;

  ~RenderPass() override = default;

private:
  bool m_enabled = true;
  std::string m_name {};
  RenderShaderProgram m_program {};
  Framebuffer m_writeFramebuffer {};

#if !defined(USE_OPENGL_ES)
  RenderTimer m_timer {};
#endif
};

} // namespace Raz

#endif // RAZ_RENDERPASS_HPP
