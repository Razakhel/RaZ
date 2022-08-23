#pragma once

#ifndef RAZ_BLOOMRENDERPROCESS_HPP
#define RAZ_BLOOMRENDERPROCESS_HPP

#include "RaZ/Render/RenderProcess.hpp"

#include <vector>

namespace Raz {

class BloomRenderProcess final : public RenderProcess {
public:
  BloomRenderProcess(RenderGraph& renderGraph, unsigned int frameWidth, unsigned int frameHeight);

  bool isEnabled() const noexcept override;
  RenderPass& getThresholdPass() noexcept { return *m_thresholdPass; }
  std::size_t getDownscalePassCount() const noexcept { return m_downscalePasses.size(); }
  const RenderPass& getDownscalePass(std::size_t passIndex) const noexcept { return *m_downscalePasses[passIndex]; }
  RenderPass& getDownscalePass(std::size_t passIndex) noexcept { return *m_downscalePasses[passIndex]; }
  std::size_t getDownscaleBufferCount() const noexcept { return m_downscaleBuffers.size(); }
  const Texture& getDownscaleBuffer(std::size_t bufferIndex) const noexcept { return *m_downscaleBuffers[bufferIndex].lock(); }
  std::size_t getUpscalePassCount() const noexcept { return m_upscalePasses.size(); }
  const RenderPass& getUpscalePass(std::size_t passIndex) const noexcept { return *m_upscalePasses[passIndex]; }
  RenderPass& getUpscalePass(std::size_t passIndex) noexcept { return *m_upscalePasses[passIndex]; }
  std::size_t getUpscaleBufferCount() const noexcept { return m_upscaleBuffers.size(); }
  const Texture& getUpscaleBuffer(std::size_t bufferIndex) const noexcept { return *m_upscaleBuffers[bufferIndex].lock(); }

  void setState(bool enabled) override;
  void addParent(RenderPass& parentPass) override;
  void addParent(RenderProcess& parentProcess) override;
  void addChild(RenderPass& childPass) override;
  void addChild(RenderProcess& childProcess) override;
  void setOutputBuffer(TexturePtr outputBuffer) override;
  void resizeBuffers(unsigned int width, unsigned int height) override;
  void setInputColorBuffer(TexturePtr colorBuffer);
  void setThresholdValue(float threshold);

private:
  RenderPass* m_thresholdPass {};

  std::vector<RenderPass*> m_downscalePasses {};
  std::vector<std::weak_ptr<Texture>> m_downscaleBuffers {};

  std::vector<RenderPass*> m_upscalePasses {};
  std::vector<std::weak_ptr<Texture>> m_upscaleBuffers {};

  RenderPass* m_finalPass {};
};

} // namespace Raz

#endif // RAZ_BLOOMRENDERPROCESS_HPP
