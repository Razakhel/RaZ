#include "RaZ/Application.hpp"
#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaScript validity", "[script][lua]") {
  CHECK_THROWS(Raz::LuaScript("")); // Empty script
  CHECK_THROWS(Raz::LuaScript("local a = 3")); // No update() function
  CHECK_NOTHROW(Raz::LuaScript("function update () end"));
}

TEST_CASE("LuaScript code", "[script][lua]") {
  Raz::LuaScript script(R"(
    tests_script_var = 0
    function tests_script_func () end
    function update () end
  )");

  CHECK(script.getEnvironment().exists("tests_script_var"));
  CHECK(script.getEnvironment().exists("tests_script_func"));
  CHECK(script.getEnvironment().exists("update"));

  script.loadCode("function update () end");

  // Setting code unregisters all existing symbols
  CHECK_FALSE(script.getEnvironment().exists("tests_script_var"));
  CHECK_FALSE(script.getEnvironment().exists("tests_script_func"));
  CHECK(script.getEnvironment().exists("update"));
}

TEST_CASE("LuaScript register", "[script][lua]") {
  Raz::LuaScript script("function update () return entity1 == entity2 end");
  const Raz::Entity entity(0);

  CHECK_FALSE(script.getEnvironment().exists("entity1"));
  CHECK_FALSE(script.getEnvironment().exists("entity2"));

  script.registerEntity(entity, "entity1");
  CHECK(script.getEnvironment().exists("entity1"));
  script.registerEntity(entity, "entity2");
  CHECK(script.getEnvironment().exists("entity2"));

  CHECK(script.update({})); // Both entities are the same

  script.loadCode("function update () end");

  // Setting code unregisters all existing symbols
  CHECK_FALSE(script.getEnvironment().exists("entity1"));
  CHECK_FALSE(script.getEnvironment().exists("entity2"));
}

TEST_CASE("LuaScript execution", "[script][lua]") {
  Raz::LuaScript script("function update() end");
  CHECK(script.update({})); // update() does not return anything, which is the same as returning true

  script.loadCode("function update() return nil end");
  CHECK_FALSE(script.update({})); // Returning nil is the same as returning false

  script.loadCode("function update() return 0 end");
  CHECK(script.update({})); // Returning any non-boolean value (except nil) is the same as returning true

  script.loadCode("function update() return false end");
  CHECK_FALSE(script.update({}));

  script.loadCode("function update() return true end");
  CHECK(script.update({}));

  // The update function can take the time structure that is given when updating the script
  script.loadCode(R"(
    function update(timeInfo)
      return timeInfo.deltaTime + timeInfo.globalTime + timeInfo.substepCount + timeInfo.substepTime == 10.0
    end
  )");
  CHECK(script.update({ 1.f, 2.f, 3, 4.f }));

  CHECK_THROWS(script.loadCodeFromFile(RAZ_TESTS_ROOT "assets/scripts/invalid.lua"));

  script.loadCodeFromFile(RAZ_TESTS_ROOT "assets/scripts/basic.lua");
  CHECK(script.update({}));
}
