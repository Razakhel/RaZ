#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include <numeric>

TEST_CASE("Image colorspace/data type creation") {
  const Raz::Image imgByte(Raz::ImageColorspace::RGBA);
  CHECK(imgByte.getWidth() == 0);
  CHECK(imgByte.getHeight() == 0);
  CHECK(imgByte.getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(imgByte.getDataType() == Raz::ImageDataType::BYTE);
  CHECK(imgByte.getChannelCount() == 4);
  CHECK(imgByte.isEmpty());

  const Raz::Image imgFloat(Raz::ImageColorspace::GRAY_ALPHA, Raz::ImageDataType::FLOAT);
  CHECK(imgFloat.getWidth() == 0);
  CHECK(imgFloat.getHeight() == 0);
  CHECK(imgFloat.getColorspace() == Raz::ImageColorspace::GRAY_ALPHA);
  CHECK(imgFloat.getDataType() == Raz::ImageDataType::FLOAT);
  CHECK(imgFloat.getChannelCount() == 2);
  CHECK(imgFloat.isEmpty());

  const Raz::Image imgDepth(Raz::ImageColorspace::DEPTH);
  CHECK(imgDepth.getWidth() == 0);
  CHECK(imgDepth.getHeight() == 0);
  CHECK(imgDepth.getColorspace() == Raz::ImageColorspace::DEPTH);
  CHECK(imgDepth.getDataType() == Raz::ImageDataType::FLOAT); // A depth image is always floating-point
  CHECK(imgDepth.getChannelCount() == 1);
  CHECK(imgDepth.isEmpty());
}

TEST_CASE("Image dimensions creation") {
  const Raz::Image imgEmpty(0, 0, Raz::ImageColorspace::RGBA);
  CHECK(imgEmpty.getWidth() == 0);
  CHECK(imgEmpty.getHeight() == 0);
  CHECK(imgEmpty.getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(imgEmpty.getDataType() == Raz::ImageDataType::BYTE);
  CHECK(imgEmpty.getChannelCount() == 4);
  CHECK(imgEmpty.isEmpty());

  const Raz::Image imgSmall(1, 1, Raz::ImageColorspace::DEPTH);
  CHECK(imgSmall.getWidth() == 1);
  CHECK(imgSmall.getHeight() == 1);
  CHECK(imgSmall.getColorspace() == Raz::ImageColorspace::DEPTH);
  CHECK(imgSmall.getDataType() == Raz::ImageDataType::FLOAT); // A depth image is always floating-point
  CHECK(imgSmall.getChannelCount() == 1);
  CHECK_FALSE(imgSmall.isEmpty());
}

TEST_CASE("Image copy/move") {
  Raz::Image imgBase(3, 3, Raz::ImageColorspace::RGB);

  auto* imgData = static_cast<uint8_t*>(imgBase.getDataPtr());
  std::iota(imgData, imgData + 9, 0);

  auto areEqual = [] (const Raz::Image& img1, const Raz::Image& img2) {
    return (img1.getWidth() == img2.getWidth()
         && img1.getHeight() == img2.getHeight()
         && img1.getColorspace() == img2.getColorspace()
         && img1 == img2); // Comparing image data
  };

  Raz::Image imgCopy(imgBase); // Copy constructor
  CHECK_FALSE(imgCopy.isEmpty());
  CHECK(areEqual(imgCopy, imgBase));

  imgCopy = imgBase; // Copy assignment operator
  CHECK_FALSE(imgCopy.isEmpty());
  CHECK(areEqual(imgCopy, imgBase));

  Raz::Image imgMove(std::move(imgBase)); // Move constructor
  CHECK_FALSE(imgMove.isEmpty());
  CHECK(imgBase.isEmpty());
  CHECK_FALSE(areEqual(imgMove, imgBase));

  imgMove = std::move(imgCopy); // Move assignment operator
  CHECK_FALSE(imgMove.isEmpty());
  CHECK(imgCopy.isEmpty());
  CHECK_FALSE(areEqual(imgMove, imgCopy));
}
