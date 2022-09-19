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

protected:
  void setInputBuffer(Texture2DPtr inputBuffer, const std::string& uniformName);
  void setOutputBuffer(Texture2DPtr outputBuffer, unsigned int index);

  RenderPass& m_pass;
};

} // namespace Raz

#endif // RAZ_MONOPASSRENDERPROCESS_HPP
