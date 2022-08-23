#pragma once

#ifndef RAZ_MONOPASSRENDERPROCESS_HPP
#define RAZ_MONOPASSRENDERPROCESS_HPP

#include "RaZ/Render/RenderProcess.hpp"

#include <string>

namespace Raz {

class FragmentShader;

class MonoPassRenderProcess : public RenderProcess {
public:
  MonoPassRenderProcess(RenderGraph& renderGraph, FragmentShader&& fragShader, std::string passName = {});

  bool isEnabled() const noexcept override;

  void setState(bool enabled) override;
  void addParent(RenderPass& parentPass) override;
  void addParent(RenderProcess& parentProcess) override;
  void addChild(RenderPass& childPass) override;
  void addChild(RenderProcess& childProcess) override;
  void setOutputBuffer(TexturePtr outputBuffer) override;

protected:
  void setInput(TexturePtr inputBuffer, const std::string& uniformName);

  RenderPass& m_pass;
};

} // namespace Raz

#endif // RAZ_MONOPASSRENDERPROCESS_HPP
