#include "Catch.hpp"

#include "RaZ/Entity.hpp"
#include "RaZ/System.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"

class TestSystem : public Raz::System {
public:
  TestSystem() { registerComponents<Raz::Transform>(); } // [ 0 1 ]

  void linkEntity(const Raz::EntityPtr& entity) override { System::linkEntity(entity); }
  void unlinkEntity(const Raz::EntityPtr& entity) override { System::unlinkEntity(entity); }

  bool update(const Raz::FrameTimeInfo&) override { return true; }
};

TEST_CASE("System basic", "[core]") {
  TestSystem testSystem {};

  Raz::EntityPtr mesh = Raz::Entity::create(0);
  mesh->addComponent<Raz::Mesh>();

  // If the system is supposed to contain the entity, link it
  // This operation is normally made into a World
  if (!(mesh->getEnabledComponents() & testSystem.getAcceptedComponents()).isEmpty())
    testSystem.linkEntity(mesh);

  CHECK_FALSE(testSystem.containsEntity(*mesh));

  Raz::EntityPtr transform = Raz::Entity::create(1);
  transform->addComponent<Raz::Transform>();

  if (!(transform->getEnabledComponents() & testSystem.getAcceptedComponents()).isEmpty())
    testSystem.linkEntity(transform);

  CHECK(testSystem.containsEntity(*transform));

  // Removing our Transform component, the entity shouldn't be processed by the system anymore
  transform->removeComponent<Raz::Transform>();

  // Unlink the entity if none of the components match
  if ((transform->getEnabledComponents() & testSystem.getAcceptedComponents()).isEmpty())
    testSystem.unlinkEntity(transform);

  CHECK_FALSE(testSystem.containsEntity(*transform));

  // Creating an entity without components
  const Raz::EntityPtr emptyEntity = Raz::Entity::create(2);

  // The entity will not be linked since there's no component to be matched
  if (!(emptyEntity->getEnabledComponents() & testSystem.getAcceptedComponents()).isEmpty())
    testSystem.linkEntity(emptyEntity);

  CHECK_FALSE(testSystem.containsEntity(*emptyEntity));
}
