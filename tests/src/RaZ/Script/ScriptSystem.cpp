#include "Catch.hpp"

#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Script/ScriptSystem.hpp"

TEST_CASE("ScriptSystem accepted components") {
  Raz::World world;

  auto& scriptSystem = world.addSystem<Raz::ScriptSystem>();

  const Raz::Entity& luaScript = world.addEntityWithComponent<Raz::LuaScript>("function update() end");

  world.update({});

  CHECK(scriptSystem.containsEntity(luaScript));
}

TEST_CASE("ScriptSystem setup") {
  Raz::World world;

  world.addSystem<Raz::ScriptSystem>();

  Raz::Entity& entity = world.addEntity();
  const Raz::Transform& entityTrans = entity.addComponent<Raz::Transform>(Raz::Vec3f(1.f));

  Raz::LuaScript& luaScript = world.addEntity().addComponent<Raz::LuaScript>(R"(
    function setup()
      entity:getTransform():translate(Vec3f.new(1))
    end

    function update() end
  )");

  luaScript.registerEntity(entity, "entity");
  REQUIRE(luaScript.getEnvironment().exists("entity"));

  CHECK(entityTrans.getPosition() == Raz::Vec3f(1.f));
  world.update({});
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // The setup function is executed only once, when the system links the entity containing the script
  world.update({});
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // Updating the world again does not execute the setup
}

TEST_CASE("ScriptSystem update") {
  Raz::World world;

  world.addSystem<Raz::ScriptSystem>();

  Raz::Entity& entity = world.addEntity();
  const Raz::Transform& entityTrans = entity.addComponent<Raz::Transform>(Raz::Vec3f(1.f));

  Raz::LuaScript& luaScript = world.addEntity().addComponent<Raz::LuaScript>(R"(
    function update(timeInfo)
      entity:getTransform():translate(Vec3f.new(timeInfo.deltaTime))
      return true
    end
  )");

  luaScript.registerEntity(entity, "entity");
  REQUIRE(luaScript.getEnvironment().exists("entity"));

  CHECK(entityTrans.getPosition() == Raz::Vec3f(1.f));
  world.update({ 1.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // The update function is executed on each world update
  world.update({ 2.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(4.f));
  world.update({ 3.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(7.f));
}
