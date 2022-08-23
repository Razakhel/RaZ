#pragma once

#ifndef RAZ_RENDERPROCESS_HPP
#define RAZ_RENDERPROCESS_HPP

#include <memory>

namespace Raz {

class RenderGraph;
class RenderPass;
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

/// RenderProcess class, representing a set of render passes with fixed actions; can be derived to implement post effects.
class RenderProcess {
public:
  virtual bool isEnabled() const noexcept = 0;

  virtual void setState(bool enabled) = 0;
  void enable() noexcept { setState(true); }
  void disable() noexcept { setState(false); }
  virtual void addParent(RenderPass& parentPass) = 0;
  virtual void addParent(RenderProcess& parentProcess) = 0;
  virtual void addChild(RenderPass& childPass) = 0;
  virtual void addChild(RenderProcess& childProcess) = 0;
  virtual void setInputBuffer(TexturePtr inputBuffer) = 0;
  virtual void setOutputBuffer(TexturePtr outputBuffer) = 0;
  virtual void resizeBuffers([[maybe_unused]] unsigned int width, [[maybe_unused]] unsigned int height) {}

  virtual ~RenderProcess() = default;
};

} // namespace Raz

#endif // RAZ_RENDERPROCESS_HPP
