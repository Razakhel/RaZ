#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/TgaFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("TgaFormat load") {
  const Raz::Image img = Raz::TgaFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga");

  CHECK(img.getWidth() == 2);
  CHECK(img.getHeight() == 2);
  CHECK(img.getColorspace() == Raz::ImageColorspace::RGB);
  CHECK(img.getDataType() == Raz::ImageDataType::BYTE);
  CHECK_FALSE(img.isEmpty());

  CHECK(*static_cast<const uint8_t*>(img.getDataPtr())       == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 1) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 2) == 191);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 3) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 4) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 5) == 239);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 6) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 7) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 8) == 239);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 9)  == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 10) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 11) == 191);

  // Checking that the same image with a vertical flip has correct values
  const Raz::Image imgFlipped = Raz::TgaFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga", true);

  CHECK(imgFlipped != img);

  CHECK(*static_cast<const uint8_t*>(imgFlipped.getDataPtr())       == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 1) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 2) == 239);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 3) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 4) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 5) == 191);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 6) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 7) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 8) == 191);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 9)  == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 10) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 11) == 239);
}
