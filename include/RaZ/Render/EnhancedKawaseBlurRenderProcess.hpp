#pragma once

#ifndef RAZ_ENHANCEDKAWASEBLURRENDERPROCESS_HPP
#define RAZ_ENHANCEDKAWASEBLURRENDERPROCESS_HPP

#include "RaZ/Render/RenderProcess.hpp"

#include <vector>

namespace Raz {

class EnhancedKawaseBlurRenderProcess final : public RenderProcess {
public:
  EnhancedKawaseBlurRenderProcess(RenderGraph& renderGraph, unsigned int frameWidth, unsigned int frameHeight, unsigned int strength);

  bool isEnabled() const noexcept override;
  std::size_t getDownscalePassCount() const noexcept { return m_downscalePasses.size(); }
  const RenderPass& getDownscalePass(std::size_t passIndex) const noexcept { return *m_downscalePasses[passIndex]; }
  RenderPass& getDownscalePass(std::size_t passIndex) noexcept { return *m_downscalePasses[passIndex]; }
  std::size_t getDownscaleBufferCount() const noexcept { return m_downscaleBuffers.size(); }
  const Texture2D& getDownscaleBuffer(std::size_t bufferIndex) const noexcept { return *m_downscaleBuffers[bufferIndex].lock(); }
  std::size_t getUpscalePassCount() const noexcept { return m_upscalePasses.size(); }
  const RenderPass& getUpscalePass(std::size_t passIndex) const noexcept { return *m_upscalePasses[passIndex]; }
  RenderPass& getUpscalePass(std::size_t passIndex) noexcept { return *m_upscalePasses[passIndex]; }
  std::size_t getUpscaleBufferCount() const noexcept { return m_upscaleBuffers.size(); }
  const Texture2D& getUpscaleBuffer(std::size_t bufferIndex) const noexcept { return *m_upscaleBuffers[bufferIndex].lock(); }

  void setState(bool enabled) override;
  void addParent(RenderPass& parentPass) override;
  void addParent(RenderProcess& parentProcess) override;
  void addChild(RenderPass& childPass) override;
  void addChild(RenderProcess& childProcess) override;
  void resizeBuffers(unsigned int width, unsigned int height) override;
  void setInputBuffer(Texture2DPtr inputBuffer);
  void setOutputBuffer(Texture2DPtr outputBuffer);
  void setStrength(unsigned int strength);

private:
  std::vector<RenderPass*> m_downscalePasses {};
  std::vector<std::weak_ptr<Texture2D>> m_downscaleBuffers {};

  std::vector<RenderPass*> m_upscalePasses {};
  std::vector<std::weak_ptr<Texture2D>> m_upscaleBuffers {};

  RenderPass* m_finalPass {};
};

} // namespace Raz

#endif // RAZ_ENHANCEDKAWASEBLURRENDERPROCESS_HPP
