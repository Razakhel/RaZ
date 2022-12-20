#pragma once

#ifndef RAZ_GAUSSIANBLURRENDERPROCESS_HPP
#define RAZ_GAUSSIANBLURRENDERPROCESS_HPP

#include "RaZ/Render/RenderProcess.hpp"

namespace Raz {

class GaussianBlurRenderProcess final : public RenderProcess {
public:
  explicit GaussianBlurRenderProcess(RenderGraph& renderGraph);

  bool isEnabled() const noexcept override;
  const RenderPass& getHorizontalPass() const noexcept { return *m_horizontalPass; }
  const RenderPass& getVerticalPass() const noexcept { return *m_verticalPass; }

  void setState(bool enabled) override;
  void addParent(RenderPass& parentPass) override;
  void addParent(RenderProcess& parentProcess) override;
  void addChild(RenderPass& childPass) override;
  void addChild(RenderProcess& childProcess) override;
  void resizeBuffers(unsigned int width, unsigned int height) override;
  float recoverElapsedTime() const override;
  void setInputBuffer(Texture2DPtr inputBuffer);
  void setOutputBuffer(Texture2DPtr outputBuffer);

private:
  RenderPass* m_horizontalPass {};
  RenderPass* m_verticalPass {};

  Texture2DPtr m_horizontalBuffer {};
};

} // namespace Raz

#endif // RAZ_GAUSSIANBLURRENDERPROCESS_HPP
