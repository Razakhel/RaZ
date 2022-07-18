#include "Catch.hpp"

#include "RaZ/World.hpp"
#include "RaZ/Data/BvhSystem.hpp"
#include "RaZ/Data/Mesh.hpp"

TEST_CASE("BvhSystem accepted components") {
  Raz::World world(1);

  auto& bvh = world.addSystem<Raz::BvhSystem>();

  Raz::Entity& mesh = world.addEntityWithComponent<Raz::Mesh>();

  world.update(0.f);

  CHECK(bvh.containsEntity(mesh));
}

TEST_CASE("BvhSystem basic") {
  Raz::BvhSystem bvh;
  CHECK(bvh.getRootNode().isLeaf());
  CHECK(bvh.getRootNode().getBoundingBox() == Raz::AABB(Raz::Vec3f(0.f), Raz::Vec3f(0.f)));
  CHECK(bvh.getRootNode().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  bvh.build();
  CHECK(bvh.getRootNode().isLeaf());
  CHECK(bvh.getRootNode().getBoundingBox() == Raz::AABB(Raz::Vec3f(0.f), Raz::Vec3f(0.f)));
  CHECK(bvh.getRootNode().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  CHECK_FALSE(bvh.query(Raz::Ray(Raz::Vec3f(0.f), Raz::Axis::Z)));
}

TEST_CASE("BvhSystem build") {
  Raz::World world(4);

  auto& bvh = world.addSystem<Raz::BvhSystem>();

  const Raz::Triangle triangle1(Raz::Vec3f(-1.f), Raz::Vec3f(1.f, 1.5f, -1.f), Raz::Vec3f(-1.5f, 1.f, 1.f));
  const Raz::Triangle triangle2(Raz::Vec3f(-1.f, 1.f, -1.5f), Raz::Vec3f(1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.f, 1.5f));

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle1.getFirstPos() }, { triangle1.getSecondPos() }, { triangle1.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle2.getFirstPos() }, { triangle2.getSecondPos() }, { triangle2.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  world.update(0.f);

  //           root
  //          /    \
  // triangle1      triangle2

  CHECK_FALSE(bvh.getRootNode().isLeaf());
  CHECK(bvh.getRootNode().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.5f, 1.5f)));
  CHECK(bvh.getRootNode().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  {
    CHECK(bvh.getRootNode().getLeftChild().isLeaf());
    CHECK(bvh.getRootNode().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, 1.f)));
    CHECK(bvh.getRootNode().getLeftChild().getTriangle() == triangle1);
  }

  {
    CHECK(bvh.getRootNode().getRightChild().isLeaf());
    CHECK(bvh.getRootNode().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.f, 1.5f)));
    CHECK(bvh.getRootNode().getRightChild().getTriangle() == triangle2);
  }

  // Adding a third triangle to get two levels

  const Raz::Triangle triangle3(Raz::Vec3f(2.f, -2.5f, 1.f), Raz::Vec3f(2.5f, -2.5f, 2.f), Raz::Vec3f(2.5f, 0.f, 0.5f));

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle3.getFirstPos() }, { triangle3.getSecondPos() }, { triangle3.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  world.update(0.f);

  //           root
  //          /    \
  // triangle1      x
  //              /   \
  //     triangle2     triangle3

  CHECK_FALSE(bvh.getRootNode().isLeaf());
  CHECK(bvh.getRootNode().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -2.5f, -1.5f), Raz::Vec3f(2.5f, 1.5f, 2.f)));
  CHECK(bvh.getRootNode().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  {
    CHECK(bvh.getRootNode().getLeftChild().isLeaf());
    CHECK(bvh.getRootNode().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, 1.f)));
    CHECK(bvh.getRootNode().getLeftChild().getTriangle() == triangle1);
  }

  {
    CHECK_FALSE(bvh.getRootNode().getRightChild().isLeaf());
    CHECK(bvh.getRootNode().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -2.5f, -1.5f), Raz::Vec3f(2.5f, 1.f, 2.f)));
    CHECK(bvh.getRootNode().getRightChild().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

    {
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().isLeaf());
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.f, 1.5f)));
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().getTriangle() == triangle2);

      CHECK(bvh.getRootNode().getRightChild().getRightChild().isLeaf());
      CHECK(bvh.getRootNode().getRightChild().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(2.f, -2.5f, 0.5f), Raz::Vec3f(2.5f, 0.f, 2.f)));
      CHECK(bvh.getRootNode().getRightChild().getRightChild().getTriangle() == triangle3);
    }
  }

  // Adding a fourth triangle to get a balanced tree

  const Raz::Triangle triangle4(Raz::Vec3f(-2.f, 2.f, 2.5f), Raz::Vec3f(3.f, 1.5f, 2.5f), Raz::Vec3f(-1.f, 0.5f, -2.f));

  {
    Raz::Submesh& submesh = world.addEntity().addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle4.getFirstPos() }, { triangle4.getSecondPos() }, { triangle4.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  world.update(0.f);

  //          ----- root -----
  //          |              |
  //       --- x ---       --- x ---
  //       |       |       |       |
  //       |       | triangle2   triangle3
  // triangle1   triangle4

  CHECK_FALSE(bvh.getRootNode().isLeaf());
  CHECK(bvh.getRootNode().getBoundingBox() == Raz::AABB(Raz::Vec3f(-2.f, -2.5f, -2.f), Raz::Vec3f(3.f, 2.f, 2.5f)));
  CHECK(bvh.getRootNode().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

  {
    CHECK_FALSE(bvh.getRootNode().getLeftChild().isLeaf());
    CHECK(bvh.getRootNode().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-2.f, -1.f, -2.f), Raz::Vec3f(3.f, 2.f, 2.5f)));
    CHECK(bvh.getRootNode().getLeftChild().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

    {
      CHECK(bvh.getRootNode().getLeftChild().getLeftChild().isLeaf());
      CHECK(bvh.getRootNode().getLeftChild().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.5f, -1.f, -1.f), Raz::Vec3f(1.f, 1.5f, 1.f)));
      CHECK(bvh.getRootNode().getLeftChild().getLeftChild().getTriangle() == triangle1);

      CHECK(bvh.getRootNode().getLeftChild().getRightChild().isLeaf());
      CHECK(bvh.getRootNode().getLeftChild().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-2.f, 0.5f, -2.f), Raz::Vec3f(3.f, 2.f, 2.5f)));
      CHECK(bvh.getRootNode().getLeftChild().getRightChild().getTriangle() == triangle4);
    }
  }

  {
    CHECK_FALSE(bvh.getRootNode().getRightChild().isLeaf());
    CHECK(bvh.getRootNode().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -2.5f, -1.5f), Raz::Vec3f(2.5f, 1.f, 2.f)));
    CHECK(bvh.getRootNode().getRightChild().getTriangle() == Raz::Triangle(Raz::Vec3f(0.f), Raz::Vec3f(0.f), Raz::Vec3f(0.f)));

    {
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().isLeaf());
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(-1.f, -1.f, -1.5f), Raz::Vec3f(1.5f, 1.f, 1.5f)));
      CHECK(bvh.getRootNode().getRightChild().getLeftChild().getTriangle() == triangle2);

      CHECK(bvh.getRootNode().getRightChild().getRightChild().isLeaf());
      CHECK(bvh.getRootNode().getRightChild().getRightChild().getBoundingBox() == Raz::AABB(Raz::Vec3f(2.f, -2.5f, 0.5f), Raz::Vec3f(2.5f, 0.f, 2.f)));
      CHECK(bvh.getRootNode().getRightChild().getRightChild().getTriangle() == triangle3);
    }
  }
}

TEST_CASE("BvhSystem query") {
  // See: https://www.geogebra.org/m/tabbfjfd

  Raz::World world(3);

  auto& bvh = world.addSystem<Raz::BvhSystem>();

  const Raz::Triangle triangle1(Raz::Vec3f(-1.f, 0.f, 1.f), Raz::Vec3f(1.f, 0.f, 1.f), Raz::Vec3f(0.f, 0.f, -1.f));
  const Raz::Triangle triangle2(Raz::Vec3f(-1.f, -1.f, 0.f), Raz::Vec3f(1.f, -1.f, 0.f), Raz::Vec3f(0.f, 1.f, 0.f));
  const Raz::Triangle triangle3(Raz::Vec3f(-3.f, -1.f, 0.f), Raz::Vec3f(-1.f, -1.f, 0.f), Raz::Vec3f(-2.f, 1.f, 0.f));

  Raz::Entity& mesh1 = world.addEntity();
  Raz::Entity& mesh2 = world.addEntity();
  Raz::Entity& mesh3 = world.addEntity();

  {
    Raz::Submesh& submesh = mesh1.addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle1.getFirstPos() }, { triangle1.getSecondPos() }, { triangle1.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  {
    Raz::Submesh& submesh = mesh2.addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle2.getFirstPos() }, { triangle2.getSecondPos() }, { triangle2.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  {
    Raz::Submesh& submesh = mesh3.addComponent<Raz::Mesh>().addSubmesh();
    submesh.getVertices() = { { triangle3.getFirstPos() }, { triangle3.getSecondPos() }, { triangle3.getThirdPos() } };
    submesh.getTriangleIndices() = { 0, 1, 2 };
  }

  world.update(0.f);

  Raz::RayHit hit;

  const Raz::Entity* entity = bvh.query(Raz::Ray(Raz::Vec3f(0.f, 1.f, 0.5f), -Raz::Axis::Y), &hit);

  CHECK(entity == &mesh1);
  CHECK(hit.position == Raz::Vec3f(0.f, 0.f, 0.5f));
  CHECK_THROWS(triangle1.contains(hit.position)); // TODO: wait for the triangle's point containment check to be available
  CHECK(hit.normal == triangle1.computeNormal());
  CHECK(hit.distance == 1.f);

  entity = bvh.query(Raz::Ray(Raz::Vec3f(0.f, 0.5f, 1.f), -Raz::Axis::Z), &hit);

  CHECK(entity == &mesh2);
  CHECK(hit.position == Raz::Vec3f(0.f, 0.5f, 0.f));
  CHECK_THROWS(triangle2.contains(hit.position)); // TODO: wait for the triangle's point containment check to be available
  CHECK(hit.normal == triangle2.computeNormal());
  CHECK(hit.distance == 1.f);

  entity = bvh.query(Raz::Ray(Raz::Vec3f(-2.f, 0.f, 1.f), -Raz::Axis::Z), &hit);

  CHECK(entity == &mesh3);
  CHECK(hit.position == Raz::Vec3f(-2.f, 0.f, 0.f));
  CHECK_THROWS(triangle3.contains(hit.position)); // TODO: wait for the triangle's point containment check to be available
  CHECK(hit.normal == triangle3.computeNormal());
  CHECK(hit.distance == 1.f);

  // When hitting two triangles (here, in order, triangles 1 & 2), the one closest to the ray must be returned
  entity = bvh.query(Raz::Ray(Raz::Vec3f(0.f, -1.f, 1.25f), Raz::Vec3f(0.f, 1.f, -1.f).normalize()), &hit);

  CHECK(entity == &mesh1);
  CHECK(hit.position == Raz::Vec3f(0.f, 0.f, 0.25f));
  CHECK_THROWS(triangle1.contains(hit.position)); // TODO: wait for the triangle's point containment check to be available
  CHECK(hit.normal == -triangle1.computeNormal());
  CHECK(hit.distance == 1.414213538f);

  // Same as the above, but in the opposite direction
  entity = bvh.query(Raz::Ray(Raz::Vec3f(0.f, 1.375f, -1.f), Raz::Vec3f(0.f, -1.f, 1.f).normalize()), &hit);

  CHECK(entity == &mesh2);
  CHECK(hit.position == Raz::Vec3f(0.f, 0.375f, 0.f));
  CHECK_THROWS(triangle2.contains(hit.position)); // TODO: wait for the triangle's point containment check to be available
  CHECK(hit.normal == -triangle2.computeNormal());
  CHECK(hit.distance == 1.414213538f);

  // If the ray hits nothing, a null pointer is returned
  // The hit structure is either invalid if no bounding box has been intersected, or contains information about the bounding box that has been hit last
  entity = bvh.query(Raz::Ray(Raz::Vec3f(-1.25f, 0.f, 1.f), -Raz::Axis::Z), &hit);

  CHECK(entity == nullptr);
  CHECK(hit.position == Raz::Vec3f(0.f));
  CHECK(hit.normal == Raz::Vec3f(0.f));
  CHECK(hit.distance == std::numeric_limits<float>::max());
}
