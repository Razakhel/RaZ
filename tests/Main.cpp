#define CATCH_CONFIG_RUNNER // Telling Catch we will define a main on our own
#include "Catch.hpp"

#include "RaZ/Utils/Window.hpp"

int main(int argc, char* argv[]) {
  // Increasing precision output for floating-point values
  Catch::StringMaker<float>::precision  = std::numeric_limits<float>::digits10 + 1;
  Catch::StringMaker<double>::precision = std::numeric_limits<double>::digits10 + 1;
  // 'long double' specializations are not available (yet?) in Catch
  //Catch::StringMaker<long double>::precision = std::numeric_limits<long double>::digits10 + 1;

  // Some tests require an OpenGL context to be instantiated, which is done by the Window
  const Raz::Window window(1, 1);

  return Catch::Session().run(argc, argv);
}
