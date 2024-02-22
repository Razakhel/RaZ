#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/TgaFormat.hpp"

TEST_CASE("TgaFormat load", "[data]") {
  const Raz::Image img = Raz::TgaFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga");

  CHECK(img.getWidth() == 2);
  CHECK(img.getHeight() == 2);
  CHECK(img.getChannelCount() == 3);
  CHECK(img.getColorspace() == Raz::ImageColorspace::RGB);
  CHECK(img.getDataType() == Raz::ImageDataType::BYTE);
  CHECK_FALSE(img.isEmpty());

  CHECK(img.recoverByteValue(0, 0, 0) == 191);
  CHECK(img.recoverByteValue(0, 0, 1) == 191);
  CHECK(img.recoverByteValue(0, 0, 2) == 191);

  CHECK(img.recoverByteValue(1, 0, 0) == 239);
  CHECK(img.recoverByteValue(1, 0, 1) == 239);
  CHECK(img.recoverByteValue(1, 0, 2) == 239);

  CHECK(img.recoverByteValue(0, 1, 0) == 239);
  CHECK(img.recoverByteValue(0, 1, 1) == 239);
  CHECK(img.recoverByteValue(0, 1, 2) == 239);

  CHECK(img.recoverByteValue(1, 1, 0) == 191);
  CHECK(img.recoverByteValue(1, 1, 1) == 191);
  CHECK(img.recoverByteValue(1, 1, 2) == 191);

  // Checking that the same image with a vertical flip has correct values
  const Raz::Image imgFlipped = Raz::TgaFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga", true);

  CHECK(imgFlipped != img);

  CHECK(imgFlipped.recoverByteValue(0, 0, 0) == 239);
  CHECK(imgFlipped.recoverByteValue(0, 0, 1) == 239);
  CHECK(imgFlipped.recoverByteValue(0, 0, 2) == 239);

  CHECK(imgFlipped.recoverByteValue(1, 0, 0) == 191);
  CHECK(imgFlipped.recoverByteValue(1, 0, 1) == 191);
  CHECK(imgFlipped.recoverByteValue(1, 0, 2) == 191);

  CHECK(imgFlipped.recoverByteValue(0, 1, 0) == 191);
  CHECK(imgFlipped.recoverByteValue(0, 1, 1) == 191);
  CHECK(imgFlipped.recoverByteValue(0, 1, 2) == 191);

  CHECK(imgFlipped.recoverByteValue(1, 1, 0) == 239);
  CHECK(imgFlipped.recoverByteValue(1, 1, 1) == 239);
  CHECK(imgFlipped.recoverByteValue(1, 1, 2) == 239);
}
