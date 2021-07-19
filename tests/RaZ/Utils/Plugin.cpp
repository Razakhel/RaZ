#include "Catch.hpp"

#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Plugin.hpp"

using namespace std::literals;

TEST_CASE("Plugin load") {
#if defined(RAZ_PLATFORM_WINDOWS)
  Raz::Plugin plugin(RAZ_TESTS_ROOT + "assets/misc/PlûgïnTést.dll"s);
#else
  Raz::Plugin plugin(RAZ_TESTS_ROOT + "assets/misc/libPlûgïnTést.so"s);
#endif

  REQUIRE(plugin.isLoaded());

  CHECK(plugin.loadFunction<void(*)()>("dummy") != nullptr);

  auto func = plugin.loadFunction<int(*)(int)>("multByTwo");
  REQUIRE(func != nullptr);

  CHECK(func(3) == 6);

  plugin.close();
  CHECK_FALSE(plugin.isLoaded());
}
