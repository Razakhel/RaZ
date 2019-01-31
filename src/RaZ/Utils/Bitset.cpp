#include <algorithm>

#include "RaZ/Utils/Bitset.hpp"

namespace Raz {

bool Bitset::isEmpty() const {
  for (auto&& bit : m_bits) {
    if (bit)
      return false;
  }

  return true;
}

void Bitset::setBit(std::size_t position, bool value) {
  if (position >= m_bits.size())
    m_bits.resize(position + 1);

  m_bits[position] = value;
}

Bitset Bitset::operator~() const {
  Bitset res = *this;

  for (auto&& bit : res.getBits())
    bit = !bit;

  return res;
}

Bitset Bitset::operator&(const Bitset& bitset) const {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + res.getSize(), res.getBits().begin());

  res &= bitset;
  return res;
}

Bitset Bitset::operator|(const Bitset& bitset) const {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + res.getSize(), res.getBits().begin());

  res |= bitset;
  return res;
}

Bitset Bitset::operator^(const Bitset& bitset) const {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + res.getSize(), res.getBits().begin());

  res ^= bitset;
  return res;
}

Bitset Bitset::operator<<(std::size_t shift) const {
  Bitset res = *this;
  res <<= shift;
  return res;
}

Bitset Bitset::operator>>(std::size_t shift) const {
  Bitset res = *this;
  res >>= shift;
  return res;
}

Bitset& Bitset::operator&=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] & bitset[bitIndex];
  return *this;
}

Bitset& Bitset::operator|=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] | bitset[bitIndex];
  return *this;
}

Bitset& Bitset::operator^=(const Bitset& bitset) {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] ^ bitset[bitIndex];
  return *this;
}

Bitset& Bitset::operator<<=(std::size_t shift) {
  m_bits.resize(m_bits.size() + shift);
  return *this;
}

Bitset& Bitset::operator>>=(std::size_t shift) {
  m_bits.resize(m_bits.size() - shift);
  return *this;
}

std::ostream& operator<<(std::ostream& stream, const Bitset& bitset) {
  stream << "[ " << bitset[0];

  for (std::size_t i = 1; i < bitset.getSize(); ++i)
    stream << "; " << bitset[i];

  stream << " ]";

  return stream;
}

} // namespace Raz
