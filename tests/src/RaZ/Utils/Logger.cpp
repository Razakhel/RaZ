#include "Catch.hpp"

#include "RaZ/Utils/Logger.hpp"

TEST_CASE("Logger custom function", "[utils]") {
  Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

  std::string str;

  Raz::Logger::setLoggingFunction([&str] (Raz::LoggingLevel level, const std::string& message) {
    switch (level) {
      case Raz::LoggingLevel::ERROR:
        str += "Error";
        break;

      case Raz::LoggingLevel::WARNING:
        str += "Warning";
        break;

      case Raz::LoggingLevel::INFO:
        str += "Info";
        break;

      case Raz::LoggingLevel::DEBUG:
        str += "Debug";
        break;

      default:
        break;
    }

    str += " (" + message + ')';
  });

  Raz::Logger::error("Error message");
  CHECK(str == "Error (Error message)");
  str.clear();

  Raz::Logger::warn("Warning message");
  CHECK(str == "Warning (Warning message)");
  str.clear();

  Raz::Logger::info("Info message");
  CHECK(str == "Info (Info message)");
  str.clear();

  // In Debug builds, Logger::debug() does nothing by default
  Raz::Logger::debug("Debug message");
#if defined(RAZ_CONFIG_DEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
  CHECK(str == "Debug (Debug message)");
#else
  CHECK(str.empty());
#endif

  Raz::Logger::resetLoggingFunction();
  Raz::Logger::setLoggingLevel(Raz::LoggingLevel::NONE);
}

TEST_CASE("Logger levels", "[utils]") {
  std::string str;

  Raz::Logger::setLoggingFunction([&str] (Raz::LoggingLevel, const std::string&) { str = 'a'; });

  {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::DEBUG);

    Raz::Logger::error("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::warn("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::info("");
    CHECK(str == "a");
    str.clear();

#if defined(RAZ_CONFIG_DEBUG) || defined(RAZ_FORCE_DEBUG_LOG)
    Raz::Logger::debug("");
    CHECK(str == "a");
#endif
  }

  {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::INFO);

    Raz::Logger::error("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::warn("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::info("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::debug("");
    CHECK(str.empty());
  }

  {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::WARNING);

    Raz::Logger::error("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::warn("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::info("");
    CHECK(str.empty());

    Raz::Logger::debug("");
    CHECK(str.empty());
  }

  {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ERROR);

    Raz::Logger::error("");
    CHECK(str == "a");
    str.clear();

    Raz::Logger::warn("");
    CHECK(str.empty());

    Raz::Logger::info("");
    CHECK(str.empty());

    Raz::Logger::debug("");
    CHECK(str.empty());
  }

  {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::NONE);

    Raz::Logger::error("");
    CHECK(str.empty());

    Raz::Logger::warn("");
    CHECK(str.empty());

    Raz::Logger::info("");
    CHECK(str.empty());

    Raz::Logger::debug("");
    CHECK(str.empty());
  }

  Raz::Logger::resetLoggingFunction();
}
