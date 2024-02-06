#include "RaZ/Application.hpp"
#include "RaZ/World.hpp"
#include "RaZ/Data/BoundingVolumeHierarchySystem.hpp"
#include "RaZ/Data/Mesh.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("BoundingVolumeHierarchySystem accepted components", "[data]") {
  Raz::World world(1);

  auto& bvh = world.addSystem<Raz::BoundingVolumeHierarchySystem>();

  const Raz::Entity& mesh = world.addEntityWithComponent<Raz::Mesh>();

  world.update({});

  CHECK(bvh.containsEntity(mesh));
}

TEST_CASE("BoundingVolumeHierarchySystem auto rebuild", "[data]") {
  Raz::World world(2);

  auto& bvhSystem = world.addSystem<Raz::BoundingVolumeHierarchySystem>();
  const Raz::BoundingVolumeHierarchyNode& rootNode = bvhSystem.getBvh().getRootNode();

  const Raz::Triangle triangle1(Raz::Vec3f(-1.f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, -1.f), Raz::Vec3f(-1.5f, 1.f, 1.f));
  const Raz::Triangle triangle2(Raz::Vec3f(-1.f, 1.f, -1.5f), Raz::Vec3f(1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.f, 1.5f));

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle1.getFirstPos() }, { triangle1.getSecondPos() }, { triangle1.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  // The BVH is rebuilt on each world update if entities are linked to/unlinked from it
  world.update({});

  CHECK(rootNode.isLeaf());
  CHECK(rootNode.getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, 1.f)));
  CHECK(rootNode.getTriangle() == triangle1);

  CHECK_FALSE(rootNode.hasLeftChild());
  CHECK_FALSE(rootNode.hasRightChild());

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle2.getFirstPos() }, { triangle2.getSecondPos() }, { triangle2.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  world.update({});

  REQUIRE_FALSE(rootNode.isLeaf());
  CHECK(rootNode.getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.5f, 1.5f)));
  CHECK(rootNode.getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  {
    CHECK(rootNode.getLeftChild().isLeaf());
    CHECK(rootNode.getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, 1.f)));
    CHECK(rootNode.getLeftChild().getTriangle() == triangle1);
  }

  {
    CHECK(rootNode.getRightChild().isLeaf());
    CHECK(rootNode.getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.f, 1.5f)));
    CHECK(rootNode.getRightChild().getTriangle() == triangle2);
  }
}
