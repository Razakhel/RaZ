#pragma once

#ifndef RAZ_GRID2_HPP
#define RAZ_GRID2_HPP

#include <cassert>
#include <stdexcept>
#include <vector>

namespace Raz {

/// 2-dimensional grid of values.
/// \tparam T Type of the values.
template <typename T>
class Grid2 {
public:
  /// Creates a 2D grid.
  /// \param width Number of values along the width; must be equal to or greater than 1.
  /// \param height Number of values along the height; must be equal to or greater than 1.
  /// \param initialValue Value to be initially set at each point in the grid.
  Grid2(std::size_t width, std::size_t height, const T& initialValue = {})
    : m_width{ width }, m_height{ height }, m_values(width * height, initialValue) {
    if (width == 0 || height == 0)
      throw std::invalid_argument("[Grid2] The width & height must not be 0.");
  }

  constexpr std::size_t getWidth() const noexcept { return m_width; }
  constexpr std::size_t getHeight() const noexcept { return m_height; }
  std::conditional_t<std::is_same_v<T, bool>, T, const T&> getValue(std::size_t widthIndex, std::size_t heightIndex) const noexcept {
    return m_values[computeIndex(widthIndex, heightIndex)];
  }

  void setValue(std::size_t widthIndex, std::size_t heightIndex, T value) noexcept(std::is_nothrow_move_assignable_v<T>) {
    m_values[computeIndex(widthIndex, heightIndex)] = std::move(value);
  }

protected:
  constexpr std::size_t computeIndex(std::size_t widthIndex, std::size_t heightIndex) const noexcept {
    assert("Error: The given width index is invalid." && widthIndex < m_width);
    assert("Error: The given height index is invalid." && heightIndex < m_height);
    return heightIndex * m_width + widthIndex;
  }

  std::size_t m_width {};
  std::size_t m_height {};
  std::vector<T> m_values {};
};

using Grid2b = Grid2<bool>;
using Grid2f = Grid2<float>;

} // namespace Raz

#endif // RAZ_GRID2_HPP
