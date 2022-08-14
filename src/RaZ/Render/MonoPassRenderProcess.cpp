#include "RaZ/Render/MonoPassRenderProcess.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/FilePath.hpp"

namespace Raz {

MonoPassRenderProcess::MonoPassRenderProcess(RenderGraph& renderGraph, FragmentShader&& fragShader)
  : m_pass{ renderGraph.addNode(std::move(fragShader)) } {}

bool MonoPassRenderProcess::isEnabled() const noexcept {
  return m_pass.isEnabled();
}

void MonoPassRenderProcess::enable(bool enabled) {
  m_pass.enable(enabled);
}

void MonoPassRenderProcess::addParent(RenderPass& parentPass) {
  m_pass.addParents(parentPass);
}

void MonoPassRenderProcess::addParent(RenderProcess& parentProcess) {
  parentProcess.addChild(m_pass);
}

void MonoPassRenderProcess::addChild(RenderPass& childPass) {
  m_pass.addChildren(childPass);
}

void MonoPassRenderProcess::addChild(RenderProcess& childProcess) {
  childProcess.addParent(m_pass);
}

void MonoPassRenderProcess::setOutputBuffer(TexturePtr outputBuffer) {
  m_pass.addWriteTexture(std::move(outputBuffer));
}

void MonoPassRenderProcess::resizeBuffers(unsigned int width, unsigned int height) {
  m_pass.resizeWriteBuffers(width, height);
}

void MonoPassRenderProcess::setInput(TexturePtr inputBuffer, const std::string& uniformName) {
  m_pass.addReadTexture(std::move(inputBuffer), uniformName);
}

} // namespace Raz
