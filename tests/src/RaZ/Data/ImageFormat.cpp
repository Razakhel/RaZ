#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>

namespace {

constexpr std::array<uint8_t, 4> rawValues = { 191, 239,
                                               239, 191 };

constexpr std::array<uint8_t, 4> jpegOrigValues = { 197, 232,
                                                    230, 198 };

constexpr std::array<uint8_t, 4> jpegSavedBaseValues = { 195, 231,
                                                         233, 200 };

constexpr std::array<uint8_t, 4> jpegSavedFlippedValues = { 235, 199,
                                                            197, 230 };

constexpr Raz::ImageColorspace recoverColorspace(uint8_t channelCount) {
  return (channelCount == 4 ? Raz::ImageColorspace::RGBA
       : (channelCount == 3 ? Raz::ImageColorspace::RGB
       : (channelCount == 2 ? Raz::ImageColorspace::GRAY_ALPHA
                            : Raz::ImageColorspace::GRAY)));

}

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

void checkImageLoad(const Raz::FilePath& filePath, uint8_t expectedChannelCount, const std::array<uint8_t, 4>& expectedValues) {
  const Raz::ImageColorspace expectedColorspace = recoverColorspace(expectedChannelCount);

  checkImageData(Raz::ImageFormat::load(filePath),
                 expectedChannelCount,
                 expectedColorspace,
                 expectedValues);
  checkImageData(Raz::ImageFormat::load(filePath, true),
                 expectedChannelCount,
                 expectedColorspace,
                 { expectedValues[2], expectedValues[3], expectedValues[0], expectedValues[1] });

  const std::vector<unsigned char> fileContent = Raz::FileUtils::readFileToArray(filePath);
  checkImageData(Raz::ImageFormat::loadFromData(fileContent),
                 expectedChannelCount,
                 expectedColorspace,
                 expectedValues);
  checkImageData(Raz::ImageFormat::loadFromData(fileContent, true),
                 expectedChannelCount,
                 expectedColorspace,
                 { expectedValues[2], expectedValues[3], expectedValues[0], expectedValues[1] });
}

void checkImageSave(const Raz::FilePath& filePath,
                    uint8_t channelCount,
                    const std::array<uint8_t, 4>& expectedBaseValues,
                    const std::array<uint8_t, 4>& expectedFlippedValues) {
  const Raz::ImageColorspace colorspace = recoverColorspace(channelCount);

  Raz::Image img(2, 2, colorspace);

  img.setByteValue(0, 0, 0, rawValues[0]);
  img.setByteValue(1, 0, 0, rawValues[1]);
  img.setByteValue(0, 1, 0, rawValues[2]);
  img.setByteValue(1, 1, 0, rawValues[3]);

  if (channelCount >= 3) {
    img.setByteValue(0, 0, 1, rawValues[0]);
    img.setByteValue(0, 0, 2, rawValues[0]);

    img.setByteValue(1, 0, 1, rawValues[1]);
    img.setByteValue(1, 0, 2, rawValues[1]);

    img.setByteValue(0, 1, 1, rawValues[2]);
    img.setByteValue(0, 1, 2, rawValues[2]);

    img.setByteValue(1, 1, 1, rawValues[3]);
    img.setByteValue(1, 1, 2, rawValues[3]);
  }

  if (colorspace == Raz::ImageColorspace::GRAY_ALPHA || colorspace == Raz::ImageColorspace::RGBA) {
    const uint8_t alphaChannelIndex = (channelCount == 2 ? 1 : 3);
    img.setByteValue(0, 0, alphaChannelIndex, 255);
    img.setByteValue(1, 0, alphaChannelIndex, 255);
    img.setByteValue(0, 1, alphaChannelIndex, 255);
    img.setByteValue(1, 1, alphaChannelIndex, 255);
  }

  Raz::ImageFormat::save(filePath, img);
  checkImageData(Raz::ImageFormat::load(filePath), channelCount, colorspace, expectedBaseValues);

  Raz::ImageFormat::save(filePath, img, true);
  checkImageData(Raz::ImageFormat::load(filePath), channelCount, colorspace, expectedFlippedValues);
}

void checkImageSave(const Raz::FilePath& filePath, uint8_t channelCount, const std::array<uint8_t, 4>& expectedValues) {
  checkImageSave(filePath, channelCount, expectedValues, { expectedValues[2], expectedValues[3], expectedValues[0], expectedValues[1] });
}

} // namespace

TEST_CASE("ImageFormat load BMP", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.bmp", 4, rawValues);
}

TEST_CASE("ImageFormat save BMP", "[data]") {
  checkImageSave("sàvëTêst.bmp", 4, rawValues);
}

TEST_CASE("ImageFormat load GIF", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.gif", 4, rawValues);
}

TEST_CASE("ImageFormat load JPEG", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.jpg", 3, jpegOrigValues);
}

TEST_CASE("ImageFormat save JPEG", "[data]") {
  checkImageSave("sàvëTêst.jpg", 3, jpegSavedBaseValues, jpegSavedFlippedValues);
}

TEST_CASE("ImageFormat load PGM", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.pgm", 1, rawValues);
}

TEST_CASE("ImageFormat load PNG", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png", 4, rawValues);
}

TEST_CASE("ImageFormat save PNG", "[data]") {
  checkImageSave("sàvëTêst.png", 4, rawValues);
}

TEST_CASE("ImageFormat load PPM", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.ppm", 3, rawValues);
}

TEST_CASE("ImageFormat load TGA", "[data]") {
  checkImageLoad(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.tga", 3, rawValues);
}

TEST_CASE("ImageFormat save TGA", "[data]") {
  checkImageSave("sàvëTêst.tga", 3, rawValues);
}
