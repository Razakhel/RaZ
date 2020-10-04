#pragma once

#ifndef RAZ_BITSET_HPP
#define RAZ_BITSET_HPP

#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <vector>

namespace Raz {

class Bitset {
public:
  Bitset() = default;
  explicit Bitset(std::size_t bitCount, bool initVal = false) : m_bits(bitCount, initVal) {}
  Bitset(std::initializer_list<bool> values) : m_bits(values.begin(), values.end()) {}

  const std::vector<bool>& getBits() const noexcept { return m_bits; }
  std::vector<bool>& getBits() noexcept { return m_bits; }
  std::size_t getSize() const noexcept { return m_bits.size(); }

  bool isEmpty() const noexcept { return (std::find(m_bits.cbegin(), m_bits.cend(), true) == m_bits.cend()); }
  std::size_t getEnabledBitCount() const noexcept { return static_cast<std::size_t>(std::count(m_bits.cbegin(), m_bits.cend(), true)); }
  std::size_t getDisabledBitCount() const noexcept { return m_bits.size() - getEnabledBitCount(); }
  void setBit(std::size_t position, bool value = true);
  void resize(std::size_t newSize) { m_bits.resize(newSize); }

  Bitset operator~() const noexcept;
  Bitset operator&(const Bitset& bitset) const noexcept;
  Bitset operator|(const Bitset& bitset) const noexcept;
  Bitset operator^(const Bitset& bitset) const noexcept;
  Bitset operator<<(std::size_t shift) const;
  Bitset operator>>(std::size_t shift) const;
  Bitset& operator&=(const Bitset& bitset) noexcept;
  Bitset& operator|=(const Bitset& bitset) noexcept;
  Bitset& operator^=(const Bitset& bitset) noexcept;
  Bitset& operator<<=(std::size_t shift);
  Bitset& operator>>=(std::size_t shift);
  bool operator[](std::size_t index) const noexcept { return m_bits[index]; }
  bool operator==(const Bitset& bitset) const noexcept { return std::equal(m_bits.cbegin(), m_bits.cend(), bitset.getBits().cbegin()); }
  bool operator!=(const Bitset& bitset) const noexcept { return !(*this == bitset); }
  friend std::ostream& operator<<(std::ostream& stream, const Bitset& bitset);

private:
  std::vector<bool> m_bits {};
};

} // namespace Raz

#endif // RAZ_BITSET_HPP

