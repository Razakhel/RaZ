#include <algorithm>

#include "RaZ/Utils/Bitset.hpp"

namespace Raz {

void Bitset::setBit(std::size_t position, bool value) {
  if (position >= m_bits.size())
    m_bits.resize(position + 1);

  m_bits[position] = value;
}

Bitset Bitset::operator&(const Bitset& bitset) const {
  Bitset res = *this;
  res &= bitset;
  return res;
}

Bitset Bitset::operator|(const Bitset& bitset) const {
  Bitset res = *this;
  res |= bitset;
  return res;
}

Bitset Bitset::operator^(const Bitset& bitset) const {
  Bitset res = *this;
  res ^= bitset;
  return res;
}

Bitset& Bitset::operator&=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] & bitset.getBits()[bitIndex];
  return *this;
}

Bitset& Bitset::operator|=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] | bitset.getBits()[bitIndex];
  return *this;
}

Bitset& Bitset::operator^=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] ^ bitset.getBits()[bitIndex];
  return *this;
}

} // namespace Raz
