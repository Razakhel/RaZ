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
}

TEST_CASE("Image dimensions creation") {
  const Raz::Image imgEmpty(0, 0, Raz::ImageColorspace::RGBA);
  CHECK(imgEmpty.getWidth() == 0);
  CHECK(imgEmpty.getHeight() == 0);
  CHECK(imgEmpty.getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(imgEmpty.getDataType() == Raz::ImageDataType::BYTE);
  CHECK(imgEmpty.getChannelCount() == 4);
  CHECK(imgEmpty.isEmpty());

  const Raz::Image imgSmall(1, 1, Raz::ImageColorspace::GRAY, Raz::ImageDataType::FLOAT);
  CHECK(imgSmall.getWidth() == 1);
  CHECK(imgSmall.getHeight() == 1);
  CHECK(imgSmall.getColorspace() == Raz::ImageColorspace::GRAY);
  CHECK(imgSmall.getDataType() == Raz::ImageDataType::FLOAT);
  CHECK(imgSmall.getChannelCount() == 1);
  CHECK_FALSE(imgSmall.isEmpty());
}

TEST_CASE("Image equality") {
  Raz::Image imgB1(1, 1, Raz::ImageColorspace::GRAY, Raz::ImageDataType::BYTE);
  Raz::Image imgB2(2, 2, Raz::ImageColorspace::GRAY, Raz::ImageDataType::BYTE);
  Raz::Image imgB3(2, 2, Raz::ImageColorspace::GRAY_ALPHA, Raz::ImageDataType::BYTE);

  CHECK(imgB1 == imgB1);
  CHECK_THAT(imgB1, IsNearlyEqualToImage(imgB1)); // The images are already strictly equal to each other
  CHECK(imgB1 != imgB2); // Their dimensions are different
  CHECK_THROWS(IsNearlyEqualToImage(imgB1).match(imgB2)); // The matcher throws on different dimensions
  CHECK(imgB2 != imgB3); // Their colorspace and channel count are different
  CHECK_THROWS(IsNearlyEqualToImage(imgB2).match(imgB3)); // The matcher throws on different colorspaces/channel counts

  Raz::Image imgF1(1, 1, Raz::ImageColorspace::GRAY, Raz::ImageDataType::FLOAT);
  Raz::Image imgF2(2, 2, Raz::ImageColorspace::GRAY, Raz::ImageDataType::FLOAT);
  Raz::Image imgF3(2, 2, Raz::ImageColorspace::GRAY_ALPHA, Raz::ImageDataType::FLOAT);

  CHECK(imgF1 == imgF1);
  CHECK_THAT(imgF1, IsNearlyEqualToImage(imgF1));
  CHECK(imgF1 != imgF2);
  CHECK_THROWS(IsNearlyEqualToImage(imgF1).match(imgF2));
  CHECK(imgF2 != imgF3);
  CHECK_THROWS(IsNearlyEqualToImage(imgF2).match(imgF3));

  CHECK(imgB1 != imgF1); // Same dimensions, but different data types
  CHECK_THROWS(IsNearlyEqualToImage(imgB1).match(imgF1)); // The matcher throws on different types

  {
    Raz::Image imgB2Copy = imgB2;
    CHECK(imgB2Copy == imgB2);

    static_cast<uint8_t*>(imgB2Copy.getDataPtr())[0] = 1;

    CHECK(imgB2Copy != imgB2); // Same dimensions, same data type, but different content
    CHECK_THAT(imgB2Copy, IsNearlyEqualToImage(imgB2)); // The near-equality check has a tolerance high enough to deem them nearly equal
  }

  {
    Raz::Image imgF2Copy = imgF2;
    CHECK(imgF2Copy == imgF2);

    static_cast<float*>(imgF2Copy.getDataPtr())[0] = 0.01f;

    CHECK(imgF2Copy != imgF2); // Same as above, their content are not strictly equal
    CHECK_THAT(imgF2Copy, IsNearlyEqualToImage(imgF2)); // But they are nearly equal to each other
  }
}

TEST_CASE("Image copy/move") {
  Raz::Image imgBase(3, 3, Raz::ImageColorspace::RGB);

  auto* imgData = static_cast<uint8_t*>(imgBase.getDataPtr());
  std::iota(imgData, imgData + 9, static_cast<uint8_t>(0));

  Raz::Image imgCopy(imgBase); // Copy constructor
  CHECK_FALSE(imgCopy.isEmpty());
  CHECK(imgCopy == imgBase);

  imgCopy = imgBase; // Copy assignment operator
  CHECK_FALSE(imgCopy.isEmpty());
  CHECK(imgCopy == imgBase);

  Raz::Image imgMove(std::move(imgBase)); // Move constructor
  CHECK_FALSE(imgMove.isEmpty());
  CHECK(imgBase.isEmpty());
  CHECK(imgMove != imgBase);

  imgMove = std::move(imgCopy); // Move assignment operator
  CHECK_FALSE(imgMove.isEmpty());
  CHECK(imgCopy.isEmpty());
  CHECK(imgMove != imgCopy);
}
