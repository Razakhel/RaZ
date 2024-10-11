#pragma once

#ifndef RAZ_XRCONTEXT_HPP
#define RAZ_XRCONTEXT_HPP

#include <cstdint>
#include <string>
#include <vector>

struct XrInstance_T;
struct XrDebugUtilsMessengerEXT_T;

namespace Raz {

class XrContext {
  friend class XrSession;
  friend class XrSystem;

public:
  explicit XrContext(const std::string& appName);

  ~XrContext();

private:
  using XrInstance               = XrInstance_T*;
  using XrDebugUtilsMessengerEXT = XrDebugUtilsMessengerEXT_T*;

  void recoverApiLayers();
  void recoverExtensions();
  void createInstance(const std::string& appName);
  void destroyInstance();
  void createDebugMessenger();
  void destroyDebugMessenger();

  std::vector<std::string> m_apiLayers;
  std::vector<const char*> m_activeApiLayers;
  std::vector<std::string> m_extensions;
  std::vector<const char*> m_activeExtensions;

  XrInstance m_instance {};
  XrDebugUtilsMessengerEXT m_debugMsgr {};
  uint64_t m_systemId {};
};

} // namespace Raz

#endif // RAZ_XRCONTEXT_HPP
