#include "Catch.hpp"

#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaCore Application") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local application = Application.new()
    application       = Application.new(1)

    application:addWorld()
    assert(#application:getWorlds() == 1)
    application:run()
    application:run(function () end)
    assert(not application:runOnce())
    assert(application:getDeltaTime() >= 0)
    assert(application:getGlobalTime() >= 0)
    application:quit()
  )"));
}

TEST_CASE("LuaCore Entity") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local entity = Entity.new(0)
    entity       = Entity.new(0, true)

    assert(entity:getId() == 0)
    entity:enable()
    entity:enable(true)
    entity:disable()
    assert(not entity:isEnabled())
    assert(entity:getEnabledComponents() ~= nil)

    entity:addComponent(Camera.new())
    entity:addComponent(Collider.new())
    entity:addComponent(Light.new(LightType.POINT, 1))
    entity:addComponent(Listener.new())
    entity:addComponent(Mesh.new())
    entity:addComponent(MeshRenderer.new())
    entity:addComponent(RigidBody.new(0, 0))
    entity:addComponent(Sound.new())
    entity:addComponent(Transform.new())
  )"));
}

TEST_CASE("LuaCore World") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local world = World.new()
    world       = World.new(0)

    world:addEntity()
    world:removeEntity(world:addEntity(true))
    world:update(0)
    world:refresh()
    world:destroy()

    world:addAudioSystem()
    world:addAudioSystem("")
    world:addBvhSystem()
    world:addPhysicsSystem()
    world:addRenderSystem()
    world:addRenderSystem(1, 1)
  )"));

#if !defined(RAZ_NO_WINDOW)
  CHECK(Raz::LuaWrapper::execute(R"(
    local world = World.new()
    -- The constructors with less arguments are not tested to avoid spawning multiple windows
    world:addRenderSystem(1, 1, "", WindowSetting.INVISIBLE)
    world:addRenderSystem(1, 1, "", WindowSetting.INVISIBLE, 1)
  )"));
#endif
}
