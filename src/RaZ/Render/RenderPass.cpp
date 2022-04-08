#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

bool RenderPass::isValid() const {
  // Since a pass can get read & write buffers from other sources than the previous pass, one may have more or less
  //  buffers than those its parent write to. Direct buffer compatibility is thus not checked

  const std::vector<TexturePtr>& writeColorBuffers = m_writeFramebuffer.m_colorBuffers;

  for (const TexturePtr& readTexture : m_readTextures) {
    // If the same depth buffer exists both in read & write, the pass is invalid
    if (readTexture->getImage().getColorspace() == ImageColorspace::DEPTH && m_writeFramebuffer.hasDepthBuffer()) {
      if (readTexture.get() == &m_writeFramebuffer.getDepthBuffer())
        return false;
    }

    // Likewise for the color buffers: if any has been added as both read & write, the pass is invalid
    if (std::find(writeColorBuffers.cbegin(), writeColorBuffers.cend(), readTexture) != writeColorBuffers.cend())
      return false;
  }

  return true;
}

void RenderPass::addReadTexture(TexturePtr texture, const std::string& uniformName) {
  m_readTextures.emplace_back(std::move(texture));

  // TODO: this binding will be lost if the program is updated; store the uniform name
  m_program.use();
  m_program.sendUniform(uniformName, m_readTextures.back()->getBindingIndex());
}

void RenderPass::bindTextures() const noexcept {
  m_program.use();

  for (const TexturePtr& texture : m_readTextures) {
    texture->activate();
    texture->bind();
  }
}

void RenderPass::execute(const Framebuffer& prevFramebuffer) const {
  if (m_enabled) {
    bindTextures();

    if (!m_writeFramebuffer.isEmpty())
      m_writeFramebuffer.bind();
    prevFramebuffer.display(m_program);
    m_writeFramebuffer.unbind();
  }

  for (const RenderPass* renderPass : m_children)
    renderPass->execute(m_writeFramebuffer);
}

} // namespace Raz
