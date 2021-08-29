#include "RaZ/Utils/Logger.hpp"

#include <iostream>

namespace Raz {

void Logger::error(const std::string& message) {
  if (static_cast<int>(m_logLevel) < static_cast<int>(LoggingLevel::ERROR))
    return;

  if (m_logFunc)
    m_logFunc(LoggingLevel::ERROR, message);
  else
    std::cerr << "[RaZ] [Error] - " << message << '\n';
}

void Logger::warn(const std::string& message) {
  if (static_cast<int>(m_logLevel) < static_cast<int>(LoggingLevel::WARNING))
    return;

  if (m_logFunc)
    m_logFunc(LoggingLevel::WARNING, message);
  else
    std::cerr << "[RaZ] [Warning] - " << message << '\n';
}

void Logger::info(const std::string& message) {
  if (static_cast<int>(m_logLevel) < static_cast<int>(LoggingLevel::INFO))
    return;

  if (m_logFunc)
    m_logFunc(LoggingLevel::INFO, message);
  else
    std::cout << "[RaZ] [Info] - " << message << '\n';
}

#if defined(RAZ_CONFIG_DEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
void Logger::debug(const std::string& message) {
  if (static_cast<int>(m_logLevel) < static_cast<int>(LoggingLevel::DEBUG))
    return;

  if (m_logFunc)
    m_logFunc(LoggingLevel::DEBUG, message);
  else
    std::cout << "[RaZ] [Debug] - " << message << '\n';
}
#endif

} // namespace Raz
