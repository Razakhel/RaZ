#include "Catch.hpp"

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

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 8)  == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 9)  == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 10) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 11) == 255);

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 12) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 13) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 14) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 15) == 255);

  const Raz::Image imgFlipped("../../tests/assets/images/defaultTest.png", true);

  REQUIRE(imgFlipped != img);

  REQUIRE(*static_cast<const uint8_t*>(imgFlipped.getDataPtr())       == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 1) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 2) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 3) == 255);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 4) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 5) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 6) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 7) == 255);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 8)  == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 9)  == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 10) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 11) == 255);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 12) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 13) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 14) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 15) == 255);
}

TEST_CASE("Image imported TGA") {
  const Raz::Image img("../../tests/assets/images/defaultTest.tga");

  REQUIRE(img.getWidth() == 2);
  REQUIRE(img.getHeight() == 2);
  REQUIRE(img.getColorspace() == Raz::ImageColorspace::RGB);
  REQUIRE(img.getDataType() == Raz::ImageDataType::BYTE);
  REQUIRE_FALSE(img.isEmpty());

  REQUIRE(*static_cast<const uint8_t*>(img.getDataPtr())       == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 1) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 2) == 191);

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 3) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 4) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 5) == 239);

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 6) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 7) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 8) == 239);

  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 9)  == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 10) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(img.getDataPtr()) + 11) == 191);

  // Checking that the same image with a vertical flip has correct values
  const Raz::Image imgFlipped("../../tests/assets/images/defaultTest.tga", true);

  REQUIRE(imgFlipped != img);

  REQUIRE(*static_cast<const uint8_t*>(imgFlipped.getDataPtr())       == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 1) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 2) == 239);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 3) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 4) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 5) == 191);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 6) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 7) == 191);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 8) == 191);

  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 9)  == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 10) == 239);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 11) == 239);
}

TEST_CASE("Image exported PNG") {
  Raz::Image img(3, 3, Raz::ImageColorspace::RGBA);

  // Creating a 3x3 image, configured as such (x are white pixels):
  //
  // -------------
  // | x | x |   |
  // -------------
  // |   | x |   |
  // -------------
  // |   |   |   |
  // -------------

  // Indices are as follows (stride of 4 every pixel since it is RGBA, thus 4 channels):
  //
  // ----------------
  // |  0 |  4 |  8 |
  // ----------------
  // | 12 | 16 | 20 |
  // ----------------
  // | 24 | 28 | 32 |
  // ----------------

  // Top-left pixel
  *static_cast<uint8_t*>(img.getDataPtr())       = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 1) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 2) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 3) = 255;

  // Top-center pixel
  *(static_cast<uint8_t*>(img.getDataPtr()) + 4) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 5) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 6) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 7) = 255;

  // Center pixel
  *(static_cast<uint8_t*>(img.getDataPtr()) + 16) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 17) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 18) = 255;
  *(static_cast<uint8_t*>(img.getDataPtr()) + 19) = 255;

  img.save("testExport.png");

  // Checking that the reimported image is the same as the original one
  REQUIRE(img == Raz::Image("testExport.png"));

  img.save("testExportFlipped.png", true);

  const Raz::Image imgFlipped("testExportFlipped.png");

  // Image imported with a vertical flip, now should be as follows:
  //
  // -------------
  // |   |   |   |
  // -------------
  // |   | x |   |
  // -------------
  // | x | x |   |
  // -------------

  REQUIRE(img != imgFlipped);

  // Center pixel
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 16) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 17) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 18) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 19) == 255);

  // Bottom-left pixel
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 24) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 25) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 26) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 27) == 255);

  // Bottom-center pixel
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 28) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 29) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 30) == 255);
  REQUIRE(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 31) == 255);

  // Re-flipping the image when saving
  imgFlipped.save("testExport.png", true);

  // Checking that the re-flipped image is now equal to the original one
  REQUIRE(img == Raz::Image("testExport.png"));
}
