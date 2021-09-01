#include "RaZ/Render/RenderPass.hpp"

namespace Raz {

RenderPass::RenderPass() {
//  Renderer::createRenderPass(m_renderPass,
//                             m_swapchainImageFormat,
//                             SampleCount::ONE,
//                             AttachmentLoadOp::CLEAR,
//                             AttachmentStoreOp::STORE,
//                             AttachmentLoadOp::DONT_CARE,
//                             AttachmentStoreOp::DONT_CARE,
//                             ImageLayout::UNDEFINED,
//                             ImageLayout::PRESENT_SRC,
//                             ImageLayout::COLOR_ATTACHMENT,
//                             PipelineBindPoint::GRAPHICS,
//                             VK_SUBPASS_EXTERNAL,
//                             0,
//                             PipelineStage::COLOR_ATTACHMENT_OUTPUT,
//                             PipelineStage::COLOR_ATTACHMENT_OUTPUT,
//                             {},
//                             MemoryAccess::COLOR_ATTACHMENT_READ | MemoryAccess::COLOR_ATTACHMENT_WRITE);
}

RenderPass::RenderPass(RenderPass&& renderPass) noexcept
  : m_renderPass{ std::exchange(renderPass.m_renderPass, nullptr) },
    m_enabled{ renderPass.m_enabled },
    m_readTextures{ std::move(renderPass.m_readTextures) },
    m_writeFramebuffer{ std::move(renderPass.m_writeFramebuffer) } {}

bool RenderPass::isValid() const {
  return true;
}

void RenderPass::addReadTexture(const Texture& texture, const std::string& uniformName) {
  m_readTextures.emplace_back(&texture);
}

void RenderPass::create() {

}

RenderPass& RenderPass::operator=(RenderPass&& renderPass) noexcept {
  std::swap(m_renderPass, renderPass.m_renderPass);
  m_enabled          = renderPass.m_enabled;
  m_readTextures     = std::move(renderPass.m_readTextures);
  m_writeFramebuffer = std::move(renderPass.m_writeFramebuffer);

  return *this;
}

RenderPass::~RenderPass() {
  Renderer::destroyRenderPass(m_renderPass);
}

} // namespace Raz
