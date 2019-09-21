#include "Catch.hpp"

#include "RaZ/Utils/Bitset.hpp"

#include <sstream>

namespace {

// Declaring bitsets to be tested
const Raz::Bitset fullZeros(6, false); // 0 0 0 0 0 0
const Raz::Bitset fullOnes(6, true);   // 1 1 1 1 1 1

const Raz::Bitset alternated1({ true, false, true, false, true, false }); // 1 0 1 0 1 0
const Raz::Bitset alternated2({ false, true, false, true, false, true }); // 0 1 0 1 0 1

} // namespace

TEST_CASE("Bitset basic") {
  CHECK(fullZeros.isEmpty());
  CHECK_FALSE(fullOnes.isEmpty());

  CHECK(fullZeros.getSize() == 6);
  CHECK(fullZeros.getSize() == fullOnes.getSize());

  CHECK(fullZeros.getEnabledBitCount() == 0);
  CHECK(fullZeros.getDisabledBitCount() == 6);

  CHECK(fullOnes.getEnabledBitCount() == 6);
  CHECK(fullOnes.getDisabledBitCount() == 0);

  CHECK(alternated1.getEnabledBitCount() == 3);
  CHECK(alternated2.getEnabledBitCount() == 3);

  Raz::Bitset compBitset = fullZeros;
  CHECK(compBitset == fullZeros);

  compBitset.resize(7);
  CHECK_FALSE(compBitset.getSize() == fullZeros.getSize());
}

TEST_CASE("Bitset manipulations") {
  CHECK((alternated1 & alternated1) == alternated1);
  CHECK((alternated2 & alternated2) == alternated2);

  CHECK((alternated1 | alternated1) == alternated1);
  CHECK((alternated2 | alternated2) == alternated2);

  CHECK((alternated1 ^ alternated1) == fullZeros);
  CHECK((alternated2 ^ alternated2) == fullZeros);

  //     AND test
  //       ---
  //    1 0 1 0 1 0
  // &  0 1 0 1 0 1
  //   _____________
  // =  0 0 0 0 0 0
  CHECK((alternated1 & alternated2) == fullZeros);

  //      OR test
  //        ---
  //    1 0 1 0 1 0
  // |  0 1 0 1 0 1
  //   _____________
  // =  1 1 1 1 1 1
  CHECK((alternated1 | alternated2) == fullOnes);

  //     XOR test
  //       ---
  //    1 0 1 0 1 0
  // ^  0 1 0 1 0 1
  //   _____________
  // =  1 1 1 1 1 1
  CHECK((alternated1 ^ alternated2) == fullOnes);

  CHECK(~fullZeros == fullOnes);
  CHECK(~fullOnes == fullZeros);
  CHECK(~alternated1 == alternated2);
  CHECK(~alternated2 == alternated1);
}

TEST_CASE("Bitset shifts") {
  CHECK((alternated1 << 1) == Raz::Bitset({ true, false, true, false, true, false, false })); // 1 0 1 0 1 0 0
  CHECK((alternated1 >> 1) == Raz::Bitset({ true, false, true, false, true })); // 1 0 1 0 1

  CHECK((alternated1 >> alternated1.getSize()).getSize() == 0);

  Raz::Bitset shiftTest = alternated1;
  shiftTest >>= 1; // 1 0 1 0 1 0 0
  shiftTest <<= 1; // 1 0 1 0 1 0

  CHECK(shiftTest == alternated1);
}

TEST_CASE("Bitset printing") {
  std::stringstream stream;

  stream << fullOnes;
  CHECK(stream.str() == "[ 1; 1; 1; 1; 1; 1 ]");

  stream.str(""); // Clearing the stream

  stream << alternated1;
  CHECK(stream.str() == "[ 1; 0; 1; 0; 1; 0 ]");
}
