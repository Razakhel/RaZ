#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

bool RenderPass::isValid() const {
  // Since a pass can get read & write buffers from other sources than the previous pass, one may have more or less
  //  buffers than its parent write to. Direct buffer compatibility is thus not checked

  const std::vector<TexturePtr>& writeColorBuffers = m_writeFramebuffer.m_colorBuffers;

  for (const TexturePtr& readTexture : m_readTextures) {
    // If the same depth buffer exists both in read & write, the pass is invalid
    if (readTexture->getColorspace() == TextureColorspace::DEPTH && m_writeFramebuffer.hasDepthBuffer()) {
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
  m_program.sendUniform(uniformName, static_cast<int>(m_readTextures.size() - 1));
}

void RenderPass::removeReadTexture(const TexturePtr& texture) {
  const auto textureIt = std::find(m_readTextures.cbegin(), m_readTextures.cend(), texture);
  if (textureIt != m_readTextures.cend())
    m_readTextures.erase(textureIt);
}

void RenderPass::bindTextures() const noexcept {
  m_program.use();

  unsigned int textureIndex = 0;

  for (const TexturePtr& texture : m_readTextures) {
    Renderer::activateTexture(textureIndex++);
    texture->bind();
  }
}

void RenderPass::execute() const {
  if (!m_enabled)
    return;

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3) && !m_name.empty())
    Renderer::pushDebugGroup(m_name);
#endif

  bindTextures();

  if (!m_writeFramebuffer.isEmpty())
    m_writeFramebuffer.bind();
  m_writeFramebuffer.display();
  m_writeFramebuffer.unbind();

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3) && !m_name.empty())
    Renderer::popDebugGroup();
#endif
}

} // namespace Raz
