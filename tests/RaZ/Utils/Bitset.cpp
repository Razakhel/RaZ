#include "Catch.hpp"

#include "RaZ/Utils/Bitset.hpp"

#include <sstream>

#include "catch/catch.hpp"
#include "RaZ/Utils/Bitset.hpp"

namespace {

// Declaring bitsets to be tested
const Raz::Bitset fullZeros(6, false); // 0 0 0 0 0 0
const Raz::Bitset fullOnes(6, true);   // 1 1 1 1 1 1

const Raz::Bitset alternated1({ true, false, true, false, true, false }); // 1 0 1 0 1 0
const Raz::Bitset alternated2({ false, true, false, true, false, true }); // 0 1 0 1 0 1

} // namespace

TEST_CASE("Bitset basic") {
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

TEST_CASE("Bitset manipulations") {
  REQUIRE((alternated1 & alternated1) == alternated1);
  REQUIRE((alternated2 & alternated2) == alternated2);

  REQUIRE((alternated1 | alternated1) == alternated1);
  REQUIRE((alternated2 | alternated2) == alternated2);

  REQUIRE((alternated1 ^ alternated1) == fullZeros);
  REQUIRE((alternated2 ^ alternated2) == fullZeros);

  //     AND test
  //       ---
  //    1 0 1 0 1 0
  // &  0 1 0 1 0 1
  //   _____________
  // =  0 0 0 0 0 0
  REQUIRE((alternated1 & alternated2) == fullZeros);

  //      OR test
  //        ---
  //    1 0 1 0 1 0
  // |  0 1 0 1 0 1
  //   _____________
  // =  1 1 1 1 1 1
  REQUIRE((alternated1 | alternated2) == fullOnes);

  //     XOR test
  //       ---
  //    1 0 1 0 1 0
  // ^  0 1 0 1 0 1
  //   _____________
  // =  1 1 1 1 1 1
  REQUIRE((alternated1 ^ alternated2) == fullOnes);

  REQUIRE(~fullZeros == fullOnes);
  REQUIRE(~fullOnes == fullZeros);
  REQUIRE(~alternated1 == alternated2);
  REQUIRE(~alternated2 == alternated1);
}

TEST_CASE("Bitset shifts") {
  REQUIRE((alternated1 << 1) == Raz::Bitset({ true, false, true, false, true, false, false })); // 1 0 1 0 1 0 0
  REQUIRE((alternated1 >> 1) == Raz::Bitset({ true, false, true, false, true })); // 1 0 1 0 1

  REQUIRE((alternated1 >> alternated1.getSize()).getSize() == 0);

  Raz::Bitset shiftTest = alternated1;
  shiftTest >>= 1; // 1 0 1 0 1 0 0
  shiftTest <<= 1; // 1 0 1 0 1 0

  REQUIRE(shiftTest == alternated1);
}

TEST_CASE("Bitset printing") {
  std::stringstream stream;

  stream << fullOnes;
  REQUIRE(stream.str() == "[ 1; 1; 1; 1; 1; 1 ]");

  stream.str(""); // Clearing the stream

  stream << alternated1;
  REQUIRE(stream.str() == "[ 1; 0; 1; 0; 1; 0 ]");
}
