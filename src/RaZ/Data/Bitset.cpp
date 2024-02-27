#include "RaZ/Data/Bitset.hpp"

#include <ostream>

namespace Raz {

void Bitset::setBit(std::size_t index, bool value) {
  if (index >= m_bits.size())
    m_bits.resize(index + 1);

  m_bits[index] = value;
}

Bitset Bitset::operator~() const noexcept {
  Bitset res = *this;

  for (auto bit : res.m_bits)
    bit = !bit;

  return res;
}

Bitset Bitset::operator&(const Bitset& bitset) const noexcept {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + static_cast<std::ptrdiff_t>(res.getSize()), res.m_bits.begin());

  res &= bitset;
  return res;
}

Bitset Bitset::operator|(const Bitset& bitset) const noexcept {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + static_cast<std::ptrdiff_t>(res.getSize()), res.m_bits.begin());

  res |= bitset;
  return res;
}

Bitset Bitset::operator^(const Bitset& bitset) const noexcept {
  Bitset res(std::min(m_bits.size(), bitset.getSize()));
  std::copy(m_bits.cbegin(), m_bits.cbegin() + static_cast<std::ptrdiff_t>(res.getSize()), res.m_bits.begin());

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

Bitset& Bitset::operator&=(const Bitset& bitset) noexcept {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] && bitset[bitIndex];
  return *this;
}

Bitset& Bitset::operator|=(const Bitset& bitset) noexcept {
  for (std::size_t bitIndex = 0; bitIndex < std::min(m_bits.size(), bitset.getSize()); ++bitIndex)
    m_bits[bitIndex] = m_bits[bitIndex] || bitset[bitIndex];
  return *this;
}

Bitset& Bitset::operator^=(const Bitset& bitset) noexcept {
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
    stream << ", " << bitset[i];

  stream << " ]";

  return stream;
}

} // namespace Raz
