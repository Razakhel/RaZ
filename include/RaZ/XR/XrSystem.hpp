#pragma once

#ifndef RAZ_XRSYSTEM_HPP
#define RAZ_XRSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/XR/XrContext.hpp"
#include "RaZ/XR/XrSession.hpp"

#include <vector>

struct XrEventDataSessionStateChanged;
struct XrViewConfigurationView;

namespace Raz {

class XrSystem final : public System {
  friend class RenderSystem;

public:
  explicit XrSystem(const std::string& appName);

  unsigned int getOptimalViewWidth() const { return m_optimalViewWidth; }
  unsigned int getOptimalViewHeight() const { return m_optimalViewHeight; }

  bool update(const FrameTimeInfo&) override;

  ~XrSystem() override;

private:
  void recoverViewConfigurations();
  void recoverEnvironmentBlendModes();
  void initializeSession();
  bool renderFrame(const ViewRenderFunc& viewRenderFunc) const;
  bool processSessionStateChanged(const XrEventDataSessionStateChanged& sessionStateChanged);

  XrContext m_context;
  XrSession m_session;

  std::vector<unsigned int> m_viewConfigTypes;
  unsigned int m_viewConfigType {};
  std::vector<XrViewConfigurationView> m_viewConfigViews;
  unsigned int m_optimalViewWidth {};
  unsigned int m_optimalViewHeight {};

  std::vector<unsigned int> m_environmentBlendModes;
  unsigned int m_environmentBlendMode {};
};

} // namespace Raz

#endif // RAZ_XRSYSTEM_HPP
