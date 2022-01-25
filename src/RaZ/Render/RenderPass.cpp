#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

bool RenderPass::isValid() const {
  // Since a pass can get read & write buffers from other sources than the previous pass, one may have more or less
  //  buffers than those its parent write to. Direct buffer compatibility is thus not checked

  const std::vector<const Texture*>& writeColorBuffers = m_writeFramebuffer.m_colorBuffers;

  for (const Texture* readTexture : m_readTextures) {
    // If the same depth buffer exists both in read & write, the pass is invalid
    if (readTexture->getImage().getColorspace() == ImageColorspace::DEPTH && m_writeFramebuffer.hasDepthBuffer()) {
      if (readTexture == &m_writeFramebuffer.getDepthBuffer())
        return false;
    }

    // Likewise for the color buffers: if any has been added as both read & write, the pass is invalid
    if (std::find(writeColorBuffers.cbegin(), writeColorBuffers.cend(), readTexture) != writeColorBuffers.cend())
      return false;
  }

  return true;
}

void RenderPass::addReadTexture(const Texture& texture, const std::string& uniformName) {
  m_readTextures.emplace_back(&texture);

  m_program.use();
  m_program.sendUniform(uniformName, texture.getBindingIndex());
}

void RenderPass::execute(const Framebuffer& prevFramebuffer) const {
  if (m_enabled) {
    for (const Texture* texture : m_readTextures) {
      texture->activate();
      texture->bind();
    }

    if (!m_writeFramebuffer.isEmpty())
      m_writeFramebuffer.bind();
    prevFramebuffer.display(m_program);
    m_writeFramebuffer.unbind();
  }

  for (const RenderPass* renderPass : m_children)
    renderPass->execute(m_writeFramebuffer);
}

} // namespace Raz
