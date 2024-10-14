#include "RaZ/RaZ.hpp"

using namespace std::literals;

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::XrContext xrContext("RaZ - XR demo");

    Raz::RenderSystem render(1280, 720, "RaZ");
    Raz::XrSession xrSession(xrContext);
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
