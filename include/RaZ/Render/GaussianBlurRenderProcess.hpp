#pragma once

#ifndef RAZ_GAUSSIANBLURRENDERPROCESS_HPP
#define RAZ_GAUSSIANBLURRENDERPROCESS_HPP

#include "RaZ/Render/RenderProcess.hpp"

namespace Raz {

class GaussianBlurRenderProcess final : public RenderProcess {
public:
  GaussianBlurRenderProcess(RenderGraph& renderGraph, unsigned int frameWidth, unsigned int frameHeight);

  bool isEnabled() const noexcept override;
  const RenderPass& getHorizontalPass() const noexcept { return *m_horizontalPass; }
  const RenderPass& getVerticalPass() const noexcept { return *m_verticalPass; }

  void setState(bool enabled) override;
  void addParent(RenderPass& parentPass) override;
  void addParent(RenderProcess& parentProcess) override;
  void addChild(RenderPass& childPass) override;
  void addChild(RenderProcess& childProcess) override;
  void setOutputBuffer(TexturePtr outputBuffer) override;
  void setInputBuffer(TexturePtr inputBuffer);

private:
  RenderPass* m_horizontalPass {};
  RenderPass* m_verticalPass {};
};

} // namespace Raz

#endif // RAZ_GAUSSIANBLURRENDERPROCESS_HPP
