namespace Raz {

template <typename ShapeT>
const ShapeT& Collider::getShape() const noexcept {
  static_assert(std::is_base_of_v<Shape, ShapeT>, "Error: The fetched collider shape type must be derived from Shape.");
  static_assert(!std::is_same_v<Shape, ShapeT>, "Error: The fetched collider shape type must not be of specific type 'Shape'.");
  assert("Error: Invalid collider shape type." && dynamic_cast<const ShapeT*>(m_colliderShape.get()));

  return static_cast<const ShapeT&>(getShape());
}

} // namespace Raz
