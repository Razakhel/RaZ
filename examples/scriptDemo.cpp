#include "RaZ/RaZ.hpp"

int main(int argc, char* argv[]) {
  try {
    Raz::LuaWrapper::registerTypes();
    Raz::LuaWrapper::execute("RAZ_ROOT = \"" RAZ_ROOT "\""); // Adding the root path definition as a global variable
    Raz::LuaWrapper::executeFromFile((argc >= 2 ? argv[1] : RAZ_ROOT "scripts/demo.lua"));
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
