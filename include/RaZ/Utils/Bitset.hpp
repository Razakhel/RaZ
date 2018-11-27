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

  const std::vector<bool>& getBits() const { return m_bits; }
  std::vector<bool>& getBits() { return m_bits; }
  std::size_t getSize() const { return m_bits.size(); }

  bool isEmpty() const;
  std::size_t getEnabledBitCount() const { return static_cast<std::size_t>(std::count(m_bits.cbegin(), m_bits.cend(), true)); }
  std::size_t getDisabledBitCount() const { return m_bits.size() - getEnabledBitCount(); }
  void setBit(std::size_t position, bool value = true);
  void resize(std::size_t newSize) { m_bits.resize(newSize); }

  Bitset operator~() const;
  Bitset operator&(const Bitset& bitset) const;
  Bitset operator|(const Bitset& bitset) const;
  Bitset operator^(const Bitset& bitset) const;
  Bitset operator<<(std::size_t shift) const;
  Bitset operator>>(std::size_t shift) const;
  Bitset& operator|=(const Bitset& bitset);
  Bitset& operator&=(const Bitset& bitset);
  Bitset& operator^=(const Bitset& bitset);
  Bitset& operator<<=(std::size_t shift);
  Bitset& operator>>=(std::size_t shift);
  bool operator[](std::size_t index) const { return m_bits[index]; }
  bool operator==(const Bitset& bitset) const { return std::equal(m_bits.cbegin(), m_bits.cend(), bitset.getBits().cbegin()); }
  bool operator!=(const Bitset& bitset) const { return !(*this == bitset); }
  friend std::ostream& operator<<(std::ostream& stream, const Bitset& bitset);

private:
  std::vector<bool> m_bits {};
};

} // namespace Raz

#endif // RAZ_BITSET_HPP

