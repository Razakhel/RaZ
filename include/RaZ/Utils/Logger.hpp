#pragma once

#ifndef RAZ_LOGGER_HPP
#define RAZ_LOGGER_HPP

#include <functional>
#include <string>

namespace Raz {

enum class LoggingLevel {
  NONE = 0, ///< Ignore all logs.
  ERROR,    ///< Output logs of error category.
  WARNING,  ///< Output logs of warning category & below.
  INFO,     ///< Output logs of info category & below.
  DEBUG,    ///< Output logs of debug category & below.
  ALL       ///< Output all logs.
};

class Logger {
public:
  Logger() = delete;

  static void setLoggingLevel(LoggingLevel level) { m_logLevel = level; }
  static void setLoggingFunction(std::function<void(LoggingLevel, const std::string&)> logFunc) { m_logFunc = std::move(logFunc); }
  static void resetLoggingFunction() { m_logFunc = nullptr; }

  /// Prints an error message.
  /// \note Requires a logging level of "error" or above.
  /// \param message Message to be printed.
  static void error(const std::string& message);
  /// Prints a warning message.
  /// \note Requires a logging level of "warning" or above.
  /// \param message Message to be printed.
  static void warn(const std::string& message);
  /// Prints an information message.
  /// \note Requires a logging level of "info" or above.
  /// \param message Message to be printed.
  static void info(const std::string& message);
#if !defined(NDEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  /// Prints a debug message.
  /// \note Does nothing in a configuration other than Debug, unless RAZ_FORCE_DEBUG_LOG is defined.
  /// \note Requires a logging level of "debug" or above.
  /// \param message Message to be printed.
  static void debug(const std::string& message);
#else
  static void debug(const char*) {}
  static void debug(const std::string&) {}
#endif

  ~Logger() = delete;

private:
  static inline LoggingLevel m_logLevel = LoggingLevel::ERROR;
  static inline std::function<void(LoggingLevel, const std::string&)> m_logFunc {};
};

} // namespace Raz

#endif // RAZ_LOGGER_HPP
