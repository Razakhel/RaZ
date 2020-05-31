#include "Catch.hpp"

#include "RaZ/Render/Cubemap.hpp"

TEST_CASE("Cubemap move") {
  Raz::Cubemap cubemap;

  const unsigned int cubemapIndex = cubemap.getIndex();
  const unsigned int cubemapProgIndex = cubemap.getProgram().getIndex();

  // Move ctor

  Raz::Cubemap movedCubemapCtor(std::move(cubemap));

  // The new cubemap has the same values as the original one
  CHECK(movedCubemapCtor.getIndex() == cubemapIndex);
  CHECK(movedCubemapCtor.getProgram().getIndex() == cubemapProgIndex);

  // The moved cubemap is now invalid
  CHECK(cubemap.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(cubemap.getProgram().getIndex() == std::numeric_limits<unsigned int>::max());

  // Move assignment operator

  Raz::Cubemap movedCubemapOp;

  const unsigned int movedCubemapOpIndex = movedCubemapOp.getIndex();
  const unsigned int movedCubemapOpProgramIndex = movedCubemapOp.getProgram().getIndex();

  movedCubemapOp = std::move(movedCubemapCtor);

  // The new cubemap has the same values as the previous one
  CHECK(movedCubemapOp.getIndex() == cubemapIndex);
  CHECK(movedCubemapOp.getProgram().getIndex() == cubemapProgIndex);

  // After being moved, the values are swapped: the moved-from cubemap now has the previous moved-to's values
  CHECK(movedCubemapCtor.getIndex() == movedCubemapOpIndex);
  CHECK(movedCubemapCtor.getProgram().getIndex() == movedCubemapOpProgramIndex);
}
