#include "RaZ/Utils/Logger.hpp"
#include "RaZ/XR/XrSystem.hpp"

#include "openxr/openxr.h"

#include <array>

namespace Raz {

namespace {

const char* getResultStr(XrInstance instance, XrResult result) {
  static std::array<char, XR_MAX_RESULT_STRING_SIZE> errorStr {};
  xrResultToString(instance, result, errorStr.data());
  return errorStr.data();
}

std::string getErrorStr(const std::string& errorMsg, XrResult result, XrInstance instance) {
  return "[XrSystem] " + errorMsg + ": " + getResultStr(instance, result) + " (" + std::to_string(result) + ')';
}

void checkLog(XrResult result, const std::string& errorMsg, XrInstance instance) {
  if (XR_SUCCEEDED(result))
    return;

  Logger::error(getErrorStr(errorMsg, result, instance));
}

bool pollNextEvent(XrInstance instance, XrEventDataBuffer& eventData) {
  eventData      = {};
  eventData.type = XR_TYPE_EVENT_DATA_BUFFER;

  return (xrPollEvent(instance, &eventData) == XR_SUCCESS);
}

void processEventData(const XrEventDataEventsLost& eventsLost) {
  Logger::info("[XrSystem] {} events lost", eventsLost.lostEventCount);
}

void processEventData(const XrEventDataInstanceLossPending& instanceLossPending) {
  // After the period of time specified by lossTime, the application can try recreating an instance again
  // See: https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrEventDataInstanceLossPending.html#_description
  Logger::info("[XrSystem] Instance loss pending at: {}", instanceLossPending.lossTime);
}

void processEventData(const XrEventDataInteractionProfileChanged& interactionProfileChanged, ::XrSession session) {
  Logger::info("[XrSystem] Interaction profile changed for {} session", (interactionProfileChanged.session != session ? "unknown" : "current"));
}

void processEventData(const XrEventDataReferenceSpaceChangePending& referenceSpaceChangePending, ::XrSession session) {
  Logger::info("[XrSystem] Reference space changed pending for {} session", (referenceSpaceChangePending.session != session ? "unknown" : "current"));
}

} // namespace

XrSystem::XrSystem(const std::string& appName) : m_context(appName), m_session(m_context) {
  recoverViewConfigurations();

  m_optimalViewWidth  = m_viewConfigViews.front().recommendedImageRectWidth;
  m_optimalViewHeight = m_viewConfigViews.front().recommendedImageRectHeight;

  if (m_viewConfigViews.size() > 1) {
    for (const XrViewConfigurationView& viewConfigView : m_viewConfigViews) {
      if (viewConfigView.recommendedImageRectWidth != m_optimalViewWidth || viewConfigView.recommendedImageRectHeight != m_optimalViewHeight)
        Logger::warn("[XrSystem] The optimal configuration view size is not the same for all views; rendering may be altered");
    }
  }

  recoverEnvironmentBlendModes();
}

bool XrSystem::update(const FrameTimeInfo&) {
  XrEventDataBuffer eventData {};

  while (pollNextEvent(m_context.m_instance, eventData)) {
    switch (eventData.type) {
      case XR_TYPE_EVENT_DATA_EVENTS_LOST:
        processEventData(*reinterpret_cast<XrEventDataEventsLost*>(&eventData));
        break;

      case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
        processEventData(*reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData));
        m_session.m_isRunning = false;
        return false;

      case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
        processEventData(*reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData), m_session.m_handle);
        break;

      case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
        processEventData(*reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData), m_session.m_handle);
        break;

      case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
        processSessionStateChanged(*reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData));
        break;

      default:
        break;
    }
  }

  return true;
}

XrSystem::~XrSystem() = default;

void XrSystem::recoverViewConfigurations() {
  uint32_t viewConfigCount {};
  checkLog(xrEnumerateViewConfigurations(m_context.m_instance, m_context.m_systemId, 0, &viewConfigCount, nullptr),
           "Failed to get view configuration count",
           m_context.m_instance);
  m_viewConfigTypes.resize(viewConfigCount);
  checkLog(xrEnumerateViewConfigurations(m_context.m_instance, m_context.m_systemId, viewConfigCount, &viewConfigCount,
                                         reinterpret_cast<XrViewConfigurationType*>(m_viewConfigTypes.data())),
           "Failed to enumerate view configurations",
           m_context.m_instance);

  for (const XrViewConfigurationType viewConfigType : { XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO }) {
    if (std::ranges::find(m_viewConfigTypes, viewConfigType) == m_viewConfigTypes.cend())
      continue;

    m_viewConfigType = viewConfigType;
    break;
  }

  if (m_viewConfigType == 0) {
    Logger::warn("[XrSystem] Failed to find a view configuration type; defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO");
    m_viewConfigType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
  }

  uint32_t viewConfigViewCount {};
  checkLog(xrEnumerateViewConfigurationViews(m_context.m_instance, m_context.m_systemId, static_cast<XrViewConfigurationType>(m_viewConfigType),
                                             0, &viewConfigViewCount, nullptr),
           "Failed to get view configuration view count",
           m_context.m_instance);
  m_viewConfigViews.resize(viewConfigViewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
  checkLog(xrEnumerateViewConfigurationViews(m_context.m_instance, m_context.m_systemId, static_cast<XrViewConfigurationType>(m_viewConfigType),
                                             viewConfigViewCount, &viewConfigViewCount,
                                             m_viewConfigViews.data()),
           "Failed to enumerate view configuration views",
           m_context.m_instance);

#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  {
    std::string viewConfigViewsMsg = "[XrSystem] View configuration views:";
    for (const XrViewConfigurationView& viewConfigView : m_viewConfigViews) {
      viewConfigViewsMsg += std::format(
        "\n    View:"
        "\n        Recom. image rect width:       {}"
        "\n        Max. image rect width:         {}"
        "\n        Recom. image rect height:      {}"
        "\n        Max. image rect height:        {}"
        "\n        Recom. swapchain sample count: {}"
        "\n        Max. swapchain sample count:   {}",
        viewConfigView.recommendedImageRectWidth, viewConfigView.maxImageRectWidth, viewConfigView.recommendedImageRectHeight,
        viewConfigView.maxImageRectHeight, viewConfigView.recommendedSwapchainSampleCount, viewConfigView.maxSwapchainSampleCount);
    }
    Logger::debug(viewConfigViewsMsg);
  }
#endif
}

void XrSystem::recoverEnvironmentBlendModes() {
  uint32_t environmentBlendModeCount {};
  checkLog(xrEnumerateEnvironmentBlendModes(m_context.m_instance, m_context.m_systemId, static_cast<XrViewConfigurationType>(m_viewConfigType),
                                            0, &environmentBlendModeCount, nullptr),
           "Failed to get environment blend mode count",
           m_context.m_instance);
  m_environmentBlendModes.resize(environmentBlendModeCount);
  checkLog(xrEnumerateEnvironmentBlendModes(m_context.m_instance, m_context.m_systemId, static_cast<XrViewConfigurationType>(m_viewConfigType),
                                            environmentBlendModeCount, &environmentBlendModeCount,
                                            reinterpret_cast<XrEnvironmentBlendMode*>(m_environmentBlendModes.data())),
           "Failed to enumerate environment blend modes",
           m_context.m_instance);

  for (const XrEnvironmentBlendMode environmentBlendMode : { XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE }) {
    if (std::ranges::find(m_environmentBlendModes, environmentBlendMode) == m_environmentBlendModes.cend())
      continue;

    m_environmentBlendMode = environmentBlendMode;
    break;
  }

  if (m_environmentBlendMode == 0) {
    Logger::warn("Failed to find a compatible blend mode; defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE");
    m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
  }
}

void XrSystem::initializeSession() {
  m_session.initialize(m_context.m_systemId);
  m_session.createSwapchains(m_viewConfigViews);
}

bool XrSystem::renderFrame(const ViewRenderFunc& viewRenderFunc) const {
  return m_session.renderFrame(m_viewConfigViews, m_viewConfigType, m_environmentBlendMode, viewRenderFunc);
}

bool XrSystem::processSessionStateChanged(const XrEventDataSessionStateChanged& sessionStateChanged) {
  if (sessionStateChanged.session != m_session.m_handle) {
    Logger::info("[XrSystem] Data session state changed for unknown session");
    return true;
  }

  switch (sessionStateChanged.state) {
    case XR_SESSION_STATE_READY:
      m_session.begin(m_viewConfigType);
      m_session.m_isRunning = true;
      break;

    case XR_SESSION_STATE_STOPPING:
      m_session.end();
      m_session.m_isRunning = false;
      break;

    case XR_SESSION_STATE_LOSS_PENDING:
      // TODO: "It's possible to try to reestablish an XrInstance and XrSession"
      m_session.m_isRunning = false;
      return false;

    case XR_SESSION_STATE_EXITING:
      m_session.m_isRunning = false;
      return false;

    default:
      break;
  }

  m_session.m_state = sessionStateChanged.state;

  return true;
}

} // namespace Raz
