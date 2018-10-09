#include "catch/catch.hpp"
#include "RaZ/Utils/Bitset.hpp"

namespace {

// Declaring bitsets to be tested
const Raz::Bitset fullZeros(6, false); // 0 0 0 0 0 0
const Raz::Bitset fullOnes(6, true);   // 1 1 1 1 1 1

const Raz::Bitset alternated1({ true, false, true, false, true, false }); // 1 0 1 0 1 0
const Raz::Bitset alternated2({ false, true, false, true, false, true }); // 0 1 0 1 0 1

} // namespace

TEST_CASE("Bitset basic checks") {
  REQUIRE(fullZeros.isEmpty());
  REQUIRE_FALSE(fullOnes.isEmpty());

  REQUIRE(fullZeros.getSize() == 6);
  REQUIRE(fullZeros.getSize() == fullOnes.getSize());

  REQUIRE(fullZeros.getEnabledBitCount() == 0);
  REQUIRE(fullZeros.getDisabledBitCount() == 6);

  REQUIRE(fullOnes.getEnabledBitCount() == 6);
  REQUIRE(fullOnes.getDisabledBitCount() == 0);

  REQUIRE(alternated1.getEnabledBitCount() == 3);
  REQUIRE(alternated2.getEnabledBitCount() == 3);

  Raz::Bitset compBitset = fullZeros;
  REQUIRE(compBitset == fullZeros);

  compBitset.resize(7);
  REQUIRE_FALSE(compBitset.getSize() == fullZeros.getSize());
}
