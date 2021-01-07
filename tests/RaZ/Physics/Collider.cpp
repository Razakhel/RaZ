#include "Catch.hpp"

#include "RaZ/Physics/Collider.hpp"

TEST_CASE("Collider basic") {
  Raz::Collider collider(Raz::Plane(1.5f));
  CHECK(collider.getShapeType() == Raz::ShapeType::PLANE);
  CHECK(collider.getShape<Raz::Plane>().getDistance() == 1.5f);

  constexpr Raz::Vec3f center(1.f, 2.f, 3.f);
  collider.setShape(Raz::Sphere(center, 3.f));
  CHECK(collider.getShapeType() == Raz::ShapeType::SPHERE);
  CHECK(collider.getShape().computeCentroid() == center);
  CHECK(collider.getShape<Raz::Sphere>().getRadius() == 3.f);

  collider.setShape(Raz::Line(Raz::Vec3f(0.f), Raz::Vec3f(1.f)));
  CHECK(collider.getShapeType() == Raz::ShapeType::LINE);
  CHECK(collider.getShape<Raz::Line>().computeLength() == 1.73205078f);

  collider.setShape(Raz::AABB(Raz::Vec3f(-1.f), Raz::Vec3f(1.f)));
  CHECK(collider.getShapeType() == Raz::ShapeType::AABB);
  CHECK(collider.getShape<Raz::AABB>().computeCentroid() == Raz::Vec3f(0.f));
}
