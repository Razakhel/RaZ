#include "Catch.hpp"

#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Script/LuaScript.hpp"
#include "RaZ/Script/ScriptSystem.hpp"

TEST_CASE("ScriptSystem accepted components", "[script][lua]") {
  Raz::World world;

  auto& scriptSystem = world.addSystem<Raz::ScriptSystem>();

  const Raz::Entity& luaScript = world.addEntityWithComponent<Raz::LuaScript>("function update() end");

  world.update({});

  CHECK(scriptSystem.containsEntity(luaScript));
}

TEST_CASE("ScriptSystem registration", "[script][lua]") {
  Raz::World world;

  world.addSystem<Raz::ScriptSystem>();

  Raz::Entity& entity = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(1.f));

  Raz::LuaScript& luaScript = entity.addComponent<Raz::LuaScript>(R"(
    function update()
      -- 'this' is automatically defined to the entity holding the script; the same entity is also registered as 'entity'
      return this == entity and this:getTransform().position == Vec3f.new(1)
    end
  )");

  CHECK_FALSE(luaScript.getEnvironment().exists("this")); // 'this' represents the entity containing the script, but is set later on entity linking

  luaScript.registerEntity(entity, "entity");
  CHECK(luaScript.getEnvironment().exists("entity"));

  world.update({});
  CHECK(luaScript.getEnvironment().exists("this")); // 'this' has now been set
  CHECK(luaScript.update({}));
}

TEST_CASE("ScriptSystem setup", "[script][lua]") {
  Raz::World world;

  world.addSystem<Raz::ScriptSystem>();

  Raz::Entity& entity = world.addEntity();
  const Raz::Transform& entityTrans = entity.addComponent<Raz::Transform>(Raz::Vec3f(1.f));

  entity.addComponent<Raz::LuaScript>(R"(
    function setup()
      this:getTransform():translate(Vec3f.new(1))
    end

    function update() end
  )");

  CHECK(entityTrans.getPosition() == Raz::Vec3f(1.f));
  world.update({});
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // The setup function is executed only once, when the system links the entity containing the script
  world.update({});
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // Updating the world again does not execute the setup
}

TEST_CASE("ScriptSystem update", "[script][lua]") {
  Raz::World world;

  world.addSystem<Raz::ScriptSystem>();

  Raz::Entity& entity = world.addEntity();
  const Raz::Transform& entityTrans = entity.addComponent<Raz::Transform>(Raz::Vec3f(1.f));

  entity.addComponent<Raz::LuaScript>(R"(
    function update(timeInfo)
      this:getTransform():translate(Vec3f.new(timeInfo.deltaTime))
      return true
    end
  )");

  CHECK(entityTrans.getPosition() == Raz::Vec3f(1.f));
  world.update({ 1.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(2.f)); // The update function is executed on each world update
  world.update({ 2.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(4.f));
  world.update({ 3.f });
  CHECK(entityTrans.getPosition() == Raz::Vec3f(7.f));
}
