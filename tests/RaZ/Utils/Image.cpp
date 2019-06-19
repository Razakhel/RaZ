#include "catch/catch.hpp"
#include "RaZ/Utils/Image.hpp"

TEST_CASE("Image manual creation") {
  const Raz::Image imgEmpty(0, 0);

  REQUIRE(imgEmpty.getWidth() == 0);
  REQUIRE(imgEmpty.getHeight() == 0);
  REQUIRE(imgEmpty.getColorspace() == Raz::ImageColorspace::RGB);
  REQUIRE(imgEmpty.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE(imgEmpty.isEmpty());

  const Raz::Image imgSmall(1, 1);

  REQUIRE(imgSmall.getWidth() == 1);
  REQUIRE(imgSmall.getHeight() == 1);
  REQUIRE(imgSmall.getColorspace() == Raz::ImageColorspace::RGB);
  REQUIRE(imgSmall.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(imgSmall.isEmpty());
}

TEST_CASE("Image imported PNG") {
  const Raz::Image img("../../tests/assets/images/defaultTest.png");

  REQUIRE(img.getWidth() == 2);
  REQUIRE(img.getHeight() == 2);
  REQUIRE(img.getColorspace() == Raz::ImageColorspace::RGBA);
  REQUIRE(img.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(img.isEmpty());

  REQUIRE(*static_cast<const uint8_t*>(img.getDataPtr())       == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 1) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 2) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 3) == 255);

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 4) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 5) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 6) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 7) == 255);
}

TEST_CASE("Image imported TGA") {
  const Raz::Image img("../../assets/textures/lava.tga");

  REQUIRE(img.getWidth() == 512);
  REQUIRE(img.getHeight() == 512);
  REQUIRE(img.getColorspace() == Raz::ImageColorspace::RGB);
  REQUIRE(img.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(img.isEmpty());
}
