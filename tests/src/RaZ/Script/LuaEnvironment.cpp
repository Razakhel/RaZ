#include "Catch.hpp"

#include "RaZ/Script/LuaEnvironment.hpp"
#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaEnvironment exists") {
  Raz::LuaWrapper::execute(R"(
    tests_env_var = 0
    tests_env_func = function () end
  )");

  Raz::LuaEnvironment env1;
  // Creating an environment recovers all globals from the main state
  CHECK(env1.exists("tests_env_var"));
  CHECK(env1.exists("tests_env_func"));

  env1.execute("tests_env1_var = 0");
  CHECK(env1.exists("tests_env1_var"));

  Raz::LuaEnvironment env2;
  CHECK_FALSE(env2.exists("tests_env1_var")); // Creating another environment does not get existing symbols from the others

  env2.execute("tests_env2_var = 0");
  CHECK(env2.exists("tests_env2_var"));
  CHECK_FALSE(env1.exists("tests_env2_var")); // Adding a symbol to an environment does not share it with the others

  Raz::LuaWrapper::execute("tests_env_var2 = 0");
  // Adding symbols to the global state adds them to all environments created from it
  CHECK(env1.exists("tests_env_var2"));
  CHECK(env2.exists("tests_env_var2"));

  env1.clear();
  env2.clear();

  // Clearing the environments removes all symbols created from them...
  CHECK_FALSE(env1.exists("tests_env1_var"));
  CHECK_FALSE(env2.exists("tests_env2_var"));
  // ... but not those created from the global state
  CHECK(env1.exists("tests_env_var"));
  CHECK(env1.exists("tests_env_func"));
  CHECK(env1.exists("tests_env_var2"));
  CHECK(env2.exists("tests_env_var2"));
}
