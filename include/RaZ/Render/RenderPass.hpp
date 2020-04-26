#pragma once

#ifndef RAZ_RENDERPASS_HPP
#define RAZ_RENDERPASS_HPP

#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

enum class RenderPassType : uint8_t {
  GEOMETRY = 0,
  //LIGHTING,
  //SSAO,
  SSR,
  //SHADOW,
  //TRANSPARENCY,

  RENDER_PASS_COUNT
};

class RenderPass;
using RenderPassPtr = std::unique_ptr<RenderPass>;

class RenderPass {
public:
  RenderPass(const RenderPass&) = delete;
  RenderPass(RenderPass&&) noexcept = default;

  const ShaderProgram& getProgram() const { return m_program; }
  ShaderProgram& getProgram() { return m_program; }
  const Framebuffer& getFramebuffer() const { return m_framebuffer; }

  void setProgram(ShaderProgram program) { m_program = std::move(program); }

  /// Resizes the render pass's framebuffer.
  /// \param width New framebuffer's width.
  /// \param height New framebuffer's height.
  void resize(unsigned int width, unsigned int height) { m_framebuffer.resize(width, height); }

  RenderPass& operator=(const RenderPass&) = delete;
  RenderPass& operator=(RenderPass&&) noexcept = default;

  virtual ~RenderPass() = default;

protected:
  RenderPass(VertexShader vertShader, FragmentShader fragShader) : m_program(std::move(vertShader), std::move(fragShader)) {}
  explicit RenderPass(FragmentShader fragShader) : RenderPass(Framebuffer::recoverVertexShader(), std::move(fragShader)) {}

  ShaderProgram m_program;
  Framebuffer m_framebuffer {};
};

class GeometryPass : public RenderPass {
public:
  GeometryPass(unsigned int width, unsigned int height, VertexShader vertShader, FragmentShader fragShader);
};

class SSRPass : public RenderPass {
public:
  SSRPass(unsigned int width, unsigned int height, FragmentShader fragShader);
};

} // namespace Raz

#endif // RAZ_RENDERPASS_HPP
