#pragma once

#ifndef RAZ_RENDERPASS_HPP
#define RAZ_RENDERPASS_HPP

#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Utils/Graph.hpp"

namespace Raz {

class RenderPass : public GraphNode<RenderPass> {
public:
  RenderPass() = default;
  RenderPass(VertexShader&& vertShader, FragmentShader&& fragShader) : m_program(std::move(vertShader), std::move(fragShader)) {}
  explicit RenderPass(FragmentShader&& fragShader) : RenderPass(Framebuffer::recoverVertexShader(), std::move(fragShader)) {}
  RenderPass(const RenderPass&) = delete;
  RenderPass(RenderPass&&) noexcept = default;

  bool isEnabled() const { return m_enabled; }
  /// Checks that the current render pass is valid, that is, if none of its buffer has been defined as both read & write.
  /// \return True if the render pass is valid, false otherwise.
  /// \see RenderGraph::isValid()
  bool isValid() const;
  const RenderShaderProgram& getProgram() const { return m_program; }
  RenderShaderProgram& getProgram() { return m_program; }
  std::size_t getReadTextureCount() const noexcept { return m_readTextures.size(); }
  const Texture& getReadTexture(std::size_t textureIndex) const noexcept { return *m_readTextures[textureIndex]; }
  const Framebuffer& getFramebuffer() const { return m_writeFramebuffer; }

  void setProgram(RenderShaderProgram&& program) { m_program = std::move(program); }

  void addReadTexture(const Texture& texture, const std::string& uniformName);
  void addWriteTexture(const Texture& texture) { m_writeFramebuffer.addTextureBuffer(texture); }
  /// Resizes the render pass' write buffer textures.
  /// \param width New buffers width.
  /// \param height New buffers height.
  void resizeWriteBuffers(unsigned int width, unsigned int height) { m_writeFramebuffer.resizeBuffers(width, height); }
  /// Changes the render pass' enabled state.
  /// \param enabled True if the render pass should be enabled, false if it should be disabled.
  void enable(bool enabled = true) { m_enabled = enabled; }
  /// Disables the render pass.
  void disable() { enable(false); }
  /// Binds the pass' read textures to its program.
  void bindTextures() const noexcept;
  /// Executes the render pass.
  /// \param prevFramebuffer Framebuffer written by the previous render pass.
  void execute(const Framebuffer& prevFramebuffer) const;

  RenderPass& operator=(const RenderPass&) = delete;
  RenderPass& operator=(RenderPass&&) noexcept = default;

  ~RenderPass() override = default;

protected:
  bool m_enabled = true;
  RenderShaderProgram m_program {};

  std::vector<const Texture*> m_readTextures {};
  Framebuffer m_writeFramebuffer {};
};

} // namespace Raz

#endif // RAZ_RENDERPASS_HPP
