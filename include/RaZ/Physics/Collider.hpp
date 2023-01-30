#pragma once

#ifndef RAZ_COLLIDER_HPP
#define RAZ_COLLIDER_HPP

#include "RaZ/Component.hpp"
#include "RaZ/Utils/Shape.hpp"

namespace Raz {

class Collider final : public Component {
public:
  Collider() = default;
  explicit Collider(Shape&& shape) { setShape(std::move(shape)); }
  Collider(const Collider&) = delete;
  Collider(Collider&&) noexcept = default;

  ShapeType getShapeType() const noexcept { return m_shapeType; }
  bool hasShape() const noexcept { return (m_colliderShape != nullptr); }
  const Shape& getShape() const noexcept { assert("Error: No collider shape defined." && hasShape()); return *m_colliderShape; }
  Shape& getShape() noexcept { assert("Error: No collider shape defined." && hasShape()); return *m_colliderShape; }
  template <typename ShapeT> const ShapeT& getShape() const noexcept;
  template <typename ShapeT> ShapeT& getShape() noexcept { return const_cast<ShapeT&>(static_cast<const Collider*>(this)->getShape<ShapeT>()); }

  void setShape(Shape&& shape);

  bool intersects(const Collider& collider) const { return intersects(*collider.m_colliderShape); }
  bool intersects(const Shape& shape) const;
  bool intersects(const Ray& ray, RayHit* hit = nullptr) const;

  Collider& operator=(const Collider&) = delete;
  Collider& operator=(Collider&&) noexcept = default;

private:
  ShapeType m_shapeType {};
  std::unique_ptr<Shape> m_colliderShape {};
};

} // namespace Raz

#include "RaZ/Physics/Collider.inl"

#endif // RAZ_COLLIDER_HPP
