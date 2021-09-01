#pragma once

#ifndef RAZ_RENDERPASSVK_HPP
#define RAZ_RENDERPASSVK_HPP

#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Utils/Graph.hpp"

namespace Raz {

class RenderPass : public GraphNode<RenderPass> {
public:
  RenderPass();
  RenderPass(VertexShader vertShader, FragmentShader fragShader) : m_program(std::move(vertShader), std::move(fragShader)) {}
  explicit RenderPass(FragmentShader fragShader) : RenderPass(Framebuffer::recoverVertexShader(), std::move(fragShader)) {}
  RenderPass(const RenderPass&) = delete;
  RenderPass(RenderPass&& renderPass) noexcept;

  bool isEnabled() const { return m_enabled; }
  /// Checks that every write buffer has an associated read buffer in each of its children.
  /// \note This doesn't check for precedency; if no subsequent pass exist, it is considered valid.
  /// \return True if no child pass or if the render pass is valid, false otherwise.
  /// \see RenderGraph::isValid()
  bool isValid() const;
  const ShaderProgram& getProgram() const { return m_program; }
  ShaderProgram& getProgram() { return m_program; }
  const Framebuffer& getFramebuffer() const { return m_writeFramebuffer; }

  void setProgram(ShaderProgram program) { m_program = std::move(program); }

  void addReadTexture(const Texture& texture, const std::string& uniformName);
  void addWriteTexture(const Texture& texture) { m_writeFramebuffer.addTextureBuffer(texture); }
  /// Resizes the render pass' write buffer textures.
  /// \param width New buffers width.
  /// \param height New buffers height.
  void resizeWriteBuffers(unsigned int width, unsigned int height) { m_writeFramebuffer.resizeBuffers(width, height); }
  void create();
  /// Changes the render pass' enabled state.
  /// \param enabled True if the render pass should be enabled, false if it should be disabled.
  void enable(bool enabled = true) { m_enabled = enabled; }
  /// Disables the render pass.
  void disable() { enable(false); }

  RenderPass& operator=(const RenderPass&) = delete;
  RenderPass& operator=(RenderPass&& renderPass) noexcept;

  ~RenderPass() override;

protected:
  VkRenderPass m_renderPass {};

  bool m_enabled = true;
  ShaderProgram m_program {};

  std::vector<const Texture*> m_readTextures {};
  Framebuffer m_writeFramebuffer {};
};

} // namespace Raz

#endif // RAZ_RENDERPASSVK_HPP
