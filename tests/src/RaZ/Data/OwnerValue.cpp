#include "Catch.hpp"

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

TEST_CASE("OwnerValue move") {
  Raz::OwnerValue<int, std::numeric_limits<int>::max()> valLimit;
  CHECK(valLimit == std::numeric_limits<int>::max());
  CHECK_FALSE(valLimit.isValid()); // If an initial value is unspecified, the invalid one is assigned

  Raz::OwnerValue<int> valZero;
  CHECK(valZero == 0); // If unspecified, the default invalid value is default-initialized
  CHECK_FALSE(valZero.isValid());

  valLimit = 3;
  CHECK(valLimit.isValid()); // The given value is not equal to the invalid one, thus making it valid

  Raz::OwnerValue valMoveCtor(std::move(valLimit));
  CHECK(valMoveCtor == 3); // The moved-to value has taken the moved-from's
  CHECK(valLimit == std::numeric_limits<int>::max()); // The moved-from value is now invalid
  CHECK_FALSE(valLimit.isValid());

  valZero = 10;
  CHECK(valZero.isValid());

  Raz::OwnerValue valMoveOp(1);
  valMoveOp = std::move(valZero);
  CHECK(valMoveOp == 10); // The values have been swapped
  CHECK(valZero == 1);
}
