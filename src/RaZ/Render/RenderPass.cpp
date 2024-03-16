#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

bool RenderPass::isValid() const {
  // Since a pass can get read & write buffers from other sources than the previous pass, one may have more or less
  //  buffers than its parent write to. Direct buffer compatibility is thus not checked

  const std::vector<std::pair<Texture2DPtr, unsigned int>>& writeColorBuffers = m_writeFramebuffer.m_colorBuffers;

  for (const auto& [texture, _] : m_program.getTextures()) {
    // If the same depth buffer exists both in read & write, the pass is invalid
    if (texture->getColorspace() == TextureColorspace::DEPTH && m_writeFramebuffer.hasDepthBuffer()) {
      if (texture.get() == &m_writeFramebuffer.getDepthBuffer())
        return false;
    }

    const auto bufferIt = std::find_if(writeColorBuffers.cbegin(), writeColorBuffers.cend(), [&readTexture = texture] (const auto& buffer) {
      return (readTexture == buffer.first);
    });

    // Likewise for the color buffers: if any has been added as both read & write, the pass is invalid
    if (bufferIt != writeColorBuffers.cend())
      return false;
  }

  return true;
}

void RenderPass::addReadTexture(TexturePtr texture, const std::string& uniformName) {
  m_program.setTexture(std::move(texture), uniformName);
  m_program.initTextures();
}

void RenderPass::execute() const {
  ZoneScopedN("RenderPass::execute");

  if (!m_enabled)
    return;

  TracyGpuZoneTransient(_, (m_name.empty() ? "[Unnamed pass]" : m_name.c_str()), true)

#if !defined(USE_OPENGL_ES)
#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3) && !m_name.empty())
    Renderer::pushDebugGroup(m_name);
#endif

  m_timer.start();
#endif

  // Binding the program's textures marks it as used
  m_program.bindTextures();

  if (!m_writeFramebuffer.isEmpty())
    m_writeFramebuffer.bind();
  m_writeFramebuffer.display();
  m_writeFramebuffer.unbind();

#if !defined(USE_OPENGL_ES)
  m_timer.stop();

#if defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3) && !m_name.empty())
    Renderer::popDebugGroup();
#endif
#endif
}

} // namespace Raz
