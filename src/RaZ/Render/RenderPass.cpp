#include "RaZ/Render/RenderPass.hpp"

namespace Raz {

GeometryPass::GeometryPass(unsigned int width, unsigned int height,
                           VertexShader vertShader, FragmentShader fragShader) : RenderPass(std::move(vertShader), std::move(fragShader)) {
  m_framebuffer.addDepthBuffer(width, height);
  m_framebuffer.addColorBuffer(width, height, ImageColorspace::RGBA); // Color buffer
  m_framebuffer.addColorBuffer(width, height, ImageColorspace::RGB); // Normal buffer
}

SSRPass::SSRPass(unsigned int width, unsigned int height, FragmentShader fragShader) : RenderPass(Framebuffer::recoverVertexShader(), std::move(fragShader)) {
  m_framebuffer.addDepthBuffer(width, height);
  m_framebuffer.addColorBuffer(width, height, ImageColorspace::RGBA); // Color buffer
  m_framebuffer.addColorBuffer(width, height, ImageColorspace::RGB); // Normal buffer
}

} // namespace Raz
