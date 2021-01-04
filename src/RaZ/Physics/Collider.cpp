#include "RaZ/Physics/Collider.hpp"

namespace Raz {

void Collider::setShape(Shape&& shape) {
  m_shapeType = shape.getType();

  switch (m_shapeType) {
    case ShapeType::LINE:
      m_colliderShape = std::make_unique<Line>(static_cast<Line&&>(shape));
      break;

    case ShapeType::PLANE:
      m_colliderShape = std::make_unique<Plane>(static_cast<Plane&&>(shape));
      break;

    case ShapeType::SPHERE:
      m_colliderShape = std::make_unique<Sphere>(static_cast<Sphere&&>(shape));
      break;

    case ShapeType::TRIANGLE:
      m_colliderShape = std::make_unique<Triangle>(static_cast<Triangle&&>(shape));
      break;

    case ShapeType::QUAD:
      m_colliderShape = std::make_unique<Quad>(static_cast<Quad&&>(shape));
      break;

    case ShapeType::AABB:
      m_colliderShape = std::make_unique<AABB>(static_cast<AABB&&>(shape));
      break;

    case ShapeType::OBB:
      m_colliderShape = std::make_unique<OBB>(static_cast<OBB&&>(shape));
      break;

    default:
      throw std::invalid_argument("Error: Unhandled shape type in the collider shape setter");
  }
}

bool Collider::intersects(const Shape& shape) const {
  switch (m_shapeType) {
    case ShapeType::LINE:
      return shape.intersects(static_cast<const Line&>(*m_colliderShape));

    case ShapeType::PLANE:
      return shape.intersects(static_cast<const Plane&>(*m_colliderShape));

    case ShapeType::SPHERE:
      return shape.intersects(static_cast<const Sphere&>(*m_colliderShape));

    case ShapeType::TRIANGLE:
      return shape.intersects(static_cast<const Triangle&>(*m_colliderShape));

    case ShapeType::QUAD:
      return shape.intersects(static_cast<const Quad&>(*m_colliderShape));

    case ShapeType::AABB:
      return shape.intersects(static_cast<const AABB&>(*m_colliderShape));

    case ShapeType::OBB:
      return shape.intersects(static_cast<const OBB&>(*m_colliderShape));

    default:
      break;
  }

  throw std::invalid_argument("Error: Unhandled shape type in the collider/shape intersection check");
}

bool Collider::intersects(const Ray& ray, RayHit* hit) const {
  switch (m_shapeType) {
    case ShapeType::LINE:
      //return ray.intersects(static_cast<const Line&>(*m_colliderShape), hit);
      break;

    case ShapeType::PLANE:
      return ray.intersects(static_cast<const Plane&>(*m_colliderShape), hit);

    case ShapeType::SPHERE:
      return ray.intersects(static_cast<const Sphere&>(*m_colliderShape), hit);

    case ShapeType::TRIANGLE:
      return ray.intersects(static_cast<const Triangle&>(*m_colliderShape), hit);

    case ShapeType::QUAD:
      //return ray.intersects(static_cast<const Quad&>(*m_colliderShape), hit);
      break;

    case ShapeType::AABB:
      return ray.intersects(static_cast<const AABB&>(*m_colliderShape), hit);

    case ShapeType::OBB:
      //return ray.intersects(static_cast<const OBB&>(*m_colliderShape), hit);
      break;

    default:
      break;
  }

  throw std::invalid_argument("Error: Unhandled shape type in the collider/ray intersection check");
}

} // namespace Raz
