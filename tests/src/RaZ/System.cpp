#include "RaZ/Application.hpp"
#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"
#include "RaZ/World.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

struct FirstTestComponent : public Raz::Component {};
struct SecondTestComponent : public Raz::Component {};

} // namespace

class TestSystem : public Raz::System {
public:
  TestSystem() { registerComponents<SecondTestComponent>(); }

  void linkEntity(const Raz::EntityPtr& entity) override {
    ++linkedEntityCount;
    System::linkEntity(entity);
  }
  void unlinkEntity(const Raz::EntityPtr& entity) override {
    --linkedEntityCount;
    System::unlinkEntity(entity);
  }

  bool update(const Raz::FrameTimeInfo&) override {
    ++updateCount;
    return (updateCount != 4);
  }

  std::size_t linkedEntityCount = 0;
  std::size_t updateCount = 0;
};

TEST_CASE("System basic", "[core]") {
  Raz::World world(3);

  auto& testSystem = world.addSystem<TestSystem>();
  CHECK(testSystem.getAcceptedComponents().getEnabledBitCount() == 1);
  // A system's accepted components bitset gets a value at the component's index, thus is resized accordingly
  CHECK(testSystem.getAcceptedComponents().getSize() == Raz::Component::getId<SecondTestComponent>() + 1);
  CHECK(testSystem.getAcceptedComponents()[Raz::Component::getId<SecondTestComponent>()]);

  Raz::Entity& firstEntity = world.addEntity();
  firstEntity.addComponent<FirstTestComponent>();

  // As the entity doesn't contain any of the accepted components, it won't be linked
  // The world's update returns true while there are still active systems
  CHECK(world.update({}));
  CHECK_FALSE(testSystem.containsEntity(firstEntity));
  CHECK(testSystem.linkedEntityCount == 0);
  CHECK(testSystem.updateCount == 1);

  Raz::Entity& secondEntity = world.addEntity();
  secondEntity.addComponent<SecondTestComponent>();

  CHECK(world.update({})); // The entity is linked to the system as it has at least one of the accepted components
  CHECK(testSystem.containsEntity(secondEntity));
  CHECK(testSystem.linkedEntityCount == 1);
  CHECK(testSystem.updateCount == 2);

  // Removing the component, the entity shouldn't be processed by the system anymore
  secondEntity.removeComponent<SecondTestComponent>();

  CHECK(world.update({})); // The entity is unlinked from the system
  CHECK_FALSE(testSystem.containsEntity(secondEntity));
  CHECK(testSystem.linkedEntityCount == 0);
  CHECK(testSystem.updateCount == 3);

  // Creating an entity without components
  const Raz::Entity& emptyEntity = world.addEntity();

  // The entity will not be linked since there's no component to be matched
  // As the only existing system has returned false on update there are no active system anymore, thus the world update also returns false
  CHECK_FALSE(world.update({}));
  CHECK_FALSE(testSystem.containsEntity(emptyEntity));
  CHECK(testSystem.linkedEntityCount == 0);
  CHECK(testSystem.updateCount == 4);

  // The latest system update made it return false; this system should not be updated ever again and should ignore all entities, even compatible ones
  secondEntity.addComponent<SecondTestComponent>();
  CHECK_FALSE(world.update({}));
  CHECK_FALSE(testSystem.containsEntity(secondEntity));
  CHECK(testSystem.linkedEntityCount == 0);
  CHECK(testSystem.updateCount == 4);
}
