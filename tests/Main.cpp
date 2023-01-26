#define CATCH_CONFIG_RUNNER // Telling Catch we will define a main on our own
#include "Catch.hpp"
#include "TestUtils.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/Logger.hpp"

int main(int argc, char* argv[]) {
  // Increasing precision output for floating-point values
  Catch::StringMaker<float>::precision  = std::numeric_limits<float>::digits10 + 3;
  Catch::StringMaker<double>::precision = std::numeric_limits<double>::digits10 + 3;
  // 'long double' specializations are not available (yet?) in Catch
  //Catch::StringMaker<long double>::precision = std::numeric_limits<long double>::digits10 + 3;

  // Disabling all logging output to avoid cluttering the console with expected error messages
  Raz::Logger::setLoggingLevel(Raz::LoggingLevel::NONE);

#if !defined(RAZ_NO_LUA)
  // Initializing the Lua wrapper
  Raz::LuaWrapper::registerTypes();
#endif

  // Rendering tests require an OpenGL context to be created, which is done by the Window; the following instruction instantiates it
  TestUtils::getWindow();

  // Setting texture [un]pack alignment to 1, so that the elements are aligned on bytes. This allows direct comparison with raw data
  Raz::Renderer::setPixelStorage(Raz::PixelStorage::PACK_ALIGNMENT, 1);
  Raz::Renderer::setPixelStorage(Raz::PixelStorage::UNPACK_ALIGNMENT, 1);

  return Catch::Session().run(argc, argv);
}
