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
  const Raz::Image img("../../assets/textures/default.png");

  REQUIRE(img.getWidth() == 128);
  REQUIRE(img.getHeight() == 128);
  REQUIRE(img.getColorspace() == Raz::ImageColorspace::RGBA);
  REQUIRE(img.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(img.isEmpty());
}

TEST_CASE("Image imported TGA") {
  const Raz::Image img("../../assets/textures/lava.tga");

  REQUIRE(img.getWidth() == 512);
  REQUIRE(img.getHeight() == 512);
  REQUIRE(img.getColorspace() == Raz::ImageColorspace::RGB);
  REQUIRE(img.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(img.isEmpty());
}
