#pragma once

#ifndef RAZ_XRSYSTEM_HPP
#define RAZ_XRSYSTEM_HPP

#include "RaZ/System.hpp"
#include "RaZ/XR/XrContext.hpp"
#include "RaZ/XR/XrSession.hpp"

#include <vector>

struct XrEventDataSessionStateChanged;
struct XrViewConfigurationView;
enum XrEnvironmentBlendMode : int;
enum XrViewConfigurationType : int;

namespace Raz {

class XrSystem final : public System {
public:
  explicit XrSystem(const std::string& appName);

  bool update(const FrameTimeInfo&) override;

  ~XrSystem() override;

private:
  void recoverViewConfigurations();
  void recoverEnvironmentBlendModes();
  bool processSessionStateChanged(const XrEventDataSessionStateChanged& sessionStateChanged);

  XrContext m_context;
  XrSession m_session;

  std::vector<XrViewConfigurationType> m_viewConfigs;
  XrViewConfigurationType m_viewConfig {};
  std::vector<XrViewConfigurationView> m_viewConfigViews;

  std::vector<XrEnvironmentBlendMode> m_environmentBlendModes;
  XrEnvironmentBlendMode m_environmentBlendMode {};
};

} // namespace Raz

#endif // RAZ_XRSYSTEM_HPP
