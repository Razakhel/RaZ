#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaCore Application", "[script][lua][core]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local frameTimeInfo = FrameTimeInfo.new()

    frameTimeInfo.deltaTime    = 0
    frameTimeInfo.globalTime   = 0
    frameTimeInfo.substepCount = 0
    frameTimeInfo.substepTime  = 0
    assert(frameTimeInfo.deltaTime == 0)
    assert(frameTimeInfo.globalTime == 0)
    assert(frameTimeInfo.substepCount == 0)
    assert(frameTimeInfo.substepTime == 0)

    local application = Application.new()
    application       = Application.new(1)

    application:addWorld()
    assert(#application:getWorlds() == 1)
    application:setFixedTimeStep(0.5)
    assert(application:getTimeInfo().substepTime == 0.5)
    application:run()
    application:run(function () end)
    assert(not application:runOnce())
    application:quit()
  )"));
}

TEST_CASE("LuaCore Entity", "[script][lua][core]") {
  CHECK(TestUtils::executeLuaScript(R"(
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
    entity:addComponent(Mesh.new())
    entity:addComponent(MeshRenderer.new())
    entity:addComponent(RigidBody.new(0, 0))
    entity:addComponent(Transform.new())

    assert(entity:hasCamera())
    assert(entity:hasCollider())
    assert(entity:hasLight())
    assert(entity:hasMesh())
    assert(entity:hasMeshRenderer())
    assert(entity:hasRigidBody())
    assert(entity:hasTransform())

    assert(entity:getCamera() ~= nil)
    assert(entity:getCollider() ~= nil)
    assert(entity:getLight() ~= nil)
    assert(entity:getMesh() ~= nil)
    assert(entity:getMeshRenderer() ~= nil)
    assert(entity:getRigidBody() ~= nil)
    assert(entity:getTransform() ~= nil)
  )"));

#if defined(RAZ_USE_AUDIO)
  CHECK(TestUtils::executeLuaScript(R"(
    local entity = Entity.new(0)

    entity:addComponent(Listener.new())
    entity:addComponent(Sound.new())

    assert(entity:hasListener())
    assert(entity:hasSound())

    assert(entity:getListener() ~= nil)
    assert(entity:getSound() ~= nil)
  )"));
#endif
}

TEST_CASE("LuaCore World", "[script][lua][core]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local world = World.new()
    world       = World.new(0)

    world:addEntity()
    world:removeEntity(world:addEntity(true))
    world:update(FrameTimeInfo.new())
    world:refresh()
    world:destroy()

    world:addBoundingVolumeHierarchySystem()
    world:addPhysicsSystem()
    world:addRenderSystem()
    world:addRenderSystem(1, 1)
    world:addTriggerSystem()
  )"));

#if defined(RAZ_USE_AUDIO)
  CHECK(TestUtils::executeLuaScript(R"(
    local world = World.new()
    world:addAudioSystem()
    world:addAudioSystem("")
  )"));
#endif

#if !defined(RAZ_NO_WINDOW)
  CHECK(TestUtils::executeLuaScript(R"(
    local world = World.new()
    -- The constructors with less arguments are not tested to avoid spawning multiple windows
    world:addRenderSystem(1, 1, "", WindowSetting.INVISIBLE)
    world:addRenderSystem(1, 1, "", WindowSetting.INVISIBLE, 1)
  )"));
#endif
}
