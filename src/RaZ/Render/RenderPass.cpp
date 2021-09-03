#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

bool RenderPass::isValid() const {
  const std::size_t currentBufferCount = m_writeFramebuffer.m_colorBuffers.size() + m_writeFramebuffer.hasDepthBuffer();

  // If the current pass has any buffer and no child pass, the buffers obviously can't match
  if (m_children.empty())
    return (currentBufferCount == 0);

  for (const RenderPass* nextPass : m_children) {
    // If the amount of read textures doesn't match the write's (colors + depth), the pass necessarily isn't valid
    if (nextPass->m_readTextures.size() != currentBufferCount)
      return false;

    for (const Texture* nextReadTexture : nextPass->m_readTextures) {
      // If the following pass contains a read depth buffer, it is invalid...
      if (nextReadTexture->getImage().getColorspace() == ImageColorspace::DEPTH) {
        // ... if the current pass has either no local write counterpart...
        if (!m_writeFramebuffer.hasDepthBuffer())
          return false;

        // ... or has a different one
        if (nextReadTexture != &m_writeFramebuffer.getDepthBuffer())
          return false;
      } else {
        // If any of the color buffers don't match, the pass is invalid
        const std::vector<const Texture*>& writeColorBuffers = m_writeFramebuffer.m_colorBuffers;
        if (std::find(writeColorBuffers.cbegin(), writeColorBuffers.cend(), nextReadTexture) == writeColorBuffers.cend())
          return false;
      }
    }
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
