#include "Catch.hpp"

#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaWrapper code execution") {
  CHECK_FALSE(Raz::LuaWrapper::execute(""));
  CHECK_FALSE(Raz::LuaWrapper::execute("this code is invalid"));
  CHECK(Raz::LuaWrapper::execute(R"(
    local a = 1
    local b = 2
    local c = a + b
  )"));
}

TEST_CASE("LuaWrapper file execution") {
  CHECK_FALSE(Raz::LuaWrapper::executeFromFile(""));
  CHECK_FALSE(Raz::LuaWrapper::executeFromFile(RAZ_TESTS_ROOT "assets/scripts/invalid.lua"));
  CHECK(Raz::LuaWrapper::executeFromFile(RAZ_TESTS_ROOT "assets/scripts/basic.lua"));
}
