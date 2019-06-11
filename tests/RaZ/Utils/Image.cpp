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
