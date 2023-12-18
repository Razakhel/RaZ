#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"

#include <array>

namespace {

constexpr std::array<uint8_t, 4> rawValues = { 191, 239,
                                               239, 191 };

constexpr std::array<uint8_t, 4> jpegValues = { 197, 232,
                                                230, 198 };

void checkImageData(const Raz::Image& loadedImg,
                    uint8_t expectedChannelCount,
                    Raz::ImageColorspace expectedColorspace,
                    const std::array<uint8_t, 4>& expectedValues) {
  REQUIRE(loadedImg.getWidth() == 2);
  REQUIRE(loadedImg.getHeight() == 2);
  REQUIRE(loadedImg.getChannelCount() == expectedChannelCount);
  REQUIRE(loadedImg.getColorspace() == expectedColorspace);
  REQUIRE(loadedImg.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(loadedImg.isEmpty());

  CHECK(loadedImg.recoverByteValue(0, 0, 0) == expectedValues[0]);
  CHECK(loadedImg.recoverByteValue(1, 0, 0) == expectedValues[1]);
  CHECK(loadedImg.recoverByteValue(0, 1, 0) == expectedValues[2]);
  CHECK(loadedImg.recoverByteValue(1, 1, 0) == expectedValues[3]);

  if (expectedChannelCount >= 3) {
    CHECK(loadedImg.recoverByteValue(0, 0, 1) == expectedValues[0]);
    CHECK(loadedImg.recoverByteValue(0, 0, 2) == expectedValues[0]);

    CHECK(loadedImg.recoverByteValue(1, 0, 1) == expectedValues[1]);
    CHECK(loadedImg.recoverByteValue(1, 0, 2) == expectedValues[1]);

    CHECK(loadedImg.recoverByteValue(0, 1, 1) == expectedValues[2]);
    CHECK(loadedImg.recoverByteValue(0, 1, 2) == expectedValues[2]);

    CHECK(loadedImg.recoverByteValue(1, 1, 1) == expectedValues[3]);
    CHECK(loadedImg.recoverByteValue(1, 1, 2) == expectedValues[3]);
  }

  if (expectedColorspace == Raz::ImageColorspace::GRAY_ALPHA || expectedColorspace == Raz::ImageColorspace::RGBA) {
    const uint8_t alphaChannelIndex = (expectedChannelCount == 2 ? 1 : 3);
    CHECK(loadedImg.recoverByteValue(0, 0, alphaChannelIndex) == 255);
    CHECK(loadedImg.recoverByteValue(1, 0, alphaChannelIndex) == 255);
    CHECK(loadedImg.recoverByteValue(0, 1, alphaChannelIndex) == 255);
    CHECK(loadedImg.recoverByteValue(1, 1, alphaChannelIndex) == 255);
  }
}

void checkImage(const Raz::FilePath& filePath, uint8_t expectedChannelCount, const std::array<uint8_t, 4>& expectedValues) {
  const Raz::ImageColorspace expectedColorspace = (expectedChannelCount == 4 ? Raz::ImageColorspace::RGBA
                                                : (expectedChannelCount == 3 ? Raz::ImageColorspace::RGB
                                                : (expectedChannelCount == 2 ? Raz::ImageColorspace::GRAY_ALPHA
                                                                             : Raz::ImageColorspace::GRAY)));

  checkImageData(Raz::ImageFormat::load(filePath),
                 expectedChannelCount,
                 expectedColorspace,
                 expectedValues);
  checkImageData(Raz::ImageFormat::load(filePath, true),
                 expectedChannelCount,
                 expectedColorspace,
                 { expectedValues[2], expectedValues[3], expectedValues[0], expectedValues[1] });
}

} // namespace

TEST_CASE("ImageFormat load BMP") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.bmp", 4, rawValues);
}

TEST_CASE("ImageFormat load GIF") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.gif", 4, rawValues);
}

TEST_CASE("ImageFormat load JPEG") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.jpg", 3, jpegValues);
}

TEST_CASE("ImageFormat load PGM") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.pgm", 1, rawValues);
}

TEST_CASE("ImageFormat load PNG") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png", 4, rawValues);
}

TEST_CASE("ImageFormat load PPM") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.ppm", 3, rawValues);
}

TEST_CASE("ImageFormat load TGA") {
  checkImage(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga", 3, rawValues);
}
