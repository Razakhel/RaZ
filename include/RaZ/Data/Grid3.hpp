#pragma once

#ifndef RAZ_GRID3_HPP
#define RAZ_GRID3_HPP

#include <cassert>
#include <stdexcept>
#include <vector>

namespace Raz {

/// 3-dimensional grid of values.
/// \tparam T Type of the values.
template <typename T>
class Grid3 {
public:
  /// Creates a 3D grid.
  /// \param width Number of values along the width; must be equal to or greater than 1.
  /// \param height Number of values along the height; must be equal to or greater than 1.
  /// \param depth Number of values along the depth; must be equal to or greater than 1.
  /// \param initialValue Value to be initially set at each point in the grid.
  Grid3(std::size_t width, std::size_t height, std::size_t depth, const T& initialValue = {})
    : m_width{ width }, m_height{ height }, m_depth{ depth }, m_values(width * height * depth, initialValue) {
    if (width == 0 || height == 0 || depth == 0)
      throw std::invalid_argument("[Grid3] The width, height & depth must not be 0.");
  }

  constexpr std::size_t getWidth() const noexcept { return m_width; }
  constexpr std::size_t getHeight() const noexcept { return m_height; }
  constexpr std::size_t getDepth() const noexcept { return m_depth; }
  std::conditional_t<std::is_same_v<T, bool>, T, const T&> getValue(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex) const noexcept {
    return m_values[computeIndex(widthIndex, heightIndex, depthIndex)];
  }

  void setValue(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex, T value) noexcept(std::is_nothrow_move_assignable_v<T>) {
    m_values[computeIndex(widthIndex, heightIndex, depthIndex)] = std::move(value);
  }

protected:
  constexpr std::size_t computeIndex(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex) const noexcept {
    assert("Error: The given width index is invalid." && widthIndex < m_width);
    assert("Error: The given height index is invalid." && heightIndex < m_height);
    assert("Error: The given channel depth is invalid." && depthIndex < m_depth);
    return depthIndex * m_height * m_width + heightIndex * m_width + widthIndex;
  }

  std::size_t m_width {};
  std::size_t m_height {};
  std::size_t m_depth {};
  std::vector<T> m_values {};
};

using Grid3b = Grid3<bool>;
using Grid3f = Grid3<float>;

} // namespace Raz

#endif // RAZ_GRID3_HPP
