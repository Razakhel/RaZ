#include "Catch.hpp"

#include "RaZ/World.hpp"

TEST_CASE("World entities manipulation") {
  Raz::World world(3);

  // The constructor argument simply reserves the size for entities
  CHECK(world.getEntities().empty());

  Raz::Entity& entity0 = world.addEntity();
  Raz::Entity& entity1 = world.addEntity();
  Raz::Entity& entity2 = world.addEntity();

  CHECK(entity0.getId() == 0);
  CHECK(entity1.getId() == 1);
  CHECK(entity2.getId() == 2);

  CHECK(world.getEntities().size() == 3);

  world.removeEntity(entity0);

  CHECK(world.getEntities().size() == 2);
  CHECK(world.getEntities()[0]->getId() == 1);
  CHECK(world.getEntities()[1]->getId() == 2);

  world.removeEntity(entity2);

  CHECK(world.getEntities().size() == 1);
  CHECK(world.getEntities()[0]->getId() == 1);

  Raz::Entity& entity4 = world.addEntity();

  CHECK(world.getEntities().size() == 2);
  CHECK(world.getEntities()[0]->getId() == 1);
  CHECK(world.getEntities()[1]->getId() == 3); // The entity indices are always growing

  // The entity removal is made by checking the pointers; if it isn't owned by this world, it throws an exception
  Raz::Entity extEntity(0);
  CHECK_THROWS(world.removeEntity(extEntity));
}

TEST_CASE("World get entities with components") {
  struct TestComp1 : public Raz::Component {};
  struct TestComp2 : public Raz::Component {};

  Raz::World world(2);

  Raz::Entity& entity1 = world.addEntity();
  Raz::Entity& entity2 = world.addEntity();

  CHECK(world.recoverEntitiesWithComponents<TestComp1>().empty());
  CHECK(world.recoverEntitiesWithComponents<TestComp2>().empty());
  CHECK(world.recoverEntitiesWithComponents<TestComp1, TestComp2>().empty());

  entity1.addComponent<TestComp1>();
  entity2.addComponent<TestComp2>();
  REQUIRE(world.recoverEntitiesWithComponents<TestComp1>().size() == 1);
  CHECK(world.recoverEntitiesWithComponents<TestComp1>().front() == &entity1);
  REQUIRE(world.recoverEntitiesWithComponents<TestComp2>().size() == 1);
  CHECK(world.recoverEntitiesWithComponents<TestComp2>().front() == &entity2);
  CHECK(world.recoverEntitiesWithComponents<TestComp1, TestComp2>().empty()); // No entity has both components

  entity1.addComponent<TestComp2>();
  CHECK(world.recoverEntitiesWithComponents<TestComp1>().front() == &entity1); // Fetching with the first component is still valid
  REQUIRE(world.recoverEntitiesWithComponents<TestComp1, TestComp2>().size() == 1);
  CHECK(world.recoverEntitiesWithComponents<TestComp1, TestComp2>().front() == &entity1);
  CHECK(world.recoverEntitiesWithComponents<TestComp2, TestComp1>().front() == &entity1); // The order doesn't matter

  for (Raz::Entity* entity : world.recoverEntitiesWithComponents<TestComp1, TestComp2>()) {
    CHECK(entity->hasComponent<TestComp1>());
    CHECK(entity->hasComponent<TestComp2>());
  }

  entity1.removeComponent<TestComp2>();
  CHECK(world.recoverEntitiesWithComponents<TestComp1, TestComp2>().empty());
  CHECK(world.recoverEntitiesWithComponents<TestComp1>().front() == &entity1); // Still has the first component
}

TEST_CASE("World refresh") {
  Raz::World world(3);

  Raz::Entity& entity0 = world.addEntity(true);
  Raz::Entity& entity1 = world.addEntity(true);
  Raz::Entity& entity2 = world.addEntity(true);

  CHECK(entity0.getId() == 0);
  CHECK(entity1.getId() == 1);
  CHECK(entity2.getId() == 2);

  world.refresh(); // Nothing to reorganize, should not change the result

  CHECK(world.getEntities()[0]->getId() == 0);
  CHECK(world.getEntities()[1]->getId() == 1);
  CHECK(world.getEntities()[2]->getId() == 2);

  // Disabling the entity 1; refreshing the world should put it at the end of the list

  entity1.disable();
  world.refresh();

  // [ 0; 1;  2 ]
  //   |    X
  // [ 0; 2; 1d ]

  CHECK(world.getEntities()[0]->getId() == 0);
  CHECK(world.getEntities()[1]->getId() == 2);
  CHECK(world.getEntities()[2]->getId() == 1);

  // Likewise for the entity 0, it ends swapped with the last enabled entity

  entity0.disable();
  world.refresh();

  // [ 0;  2; 1d ]
  //     X    |
  // [ 2; 0d; 1d ]

  CHECK(world.getEntities()[0]->getId() == 2);
  CHECK(world.getEntities()[1]->getId() == 0);
  CHECK(world.getEntities()[2]->getId() == 1);

  // Reenabling the entity 1 should make it advance so that the 0 remains at the end

  entity1.enable();
  world.refresh();

  // [ 2; 0d; 1d ]
  //   |     X
  // [ 2;  1; 0d ]

  CHECK(world.getEntities()[0]->getId() == 2);
  CHECK(world.getEntities()[1]->getId() == 1);
  CHECK(world.getEntities()[2]->getId() == 0);
}
