#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/PngFormat.hpp"

TEST_CASE("PngFormat load") {
  const Raz::Image img = Raz::PngFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png");

  CHECK(img.getWidth() == 2);
  CHECK(img.getHeight() == 2);
  CHECK(img.getChannelCount() == 4);
  CHECK(img.getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(img.getDataType() == Raz::ImageDataType::BYTE);
  CHECK_FALSE(img.isEmpty());

  CHECK(img.recoverByteValue(0, 0, 0) == 191);
  CHECK(img.recoverByteValue(0, 0, 1) == 191);
  CHECK(img.recoverByteValue(0, 0, 2) == 191);
  CHECK(img.recoverByteValue(0, 0, 3) == 255);

  CHECK(img.recoverByteValue(1, 0, 0) == 239);
  CHECK(img.recoverByteValue(1, 0, 1) == 239);
  CHECK(img.recoverByteValue(1, 0, 2) == 239);
  CHECK(img.recoverByteValue(1, 0, 3) == 255);

  CHECK(img.recoverByteValue(0, 1, 0) == 239);
  CHECK(img.recoverByteValue(0, 1, 1) == 239);
  CHECK(img.recoverByteValue(0, 1, 2) == 239);
  CHECK(img.recoverByteValue(0, 1, 3) == 255);

  CHECK(img.recoverByteValue(1, 1, 0) == 191);
  CHECK(img.recoverByteValue(1, 1, 1) == 191);
  CHECK(img.recoverByteValue(1, 1, 2) == 191);
  CHECK(img.recoverByteValue(1, 1, 3) == 255);

  const Raz::Image imgFlipped = Raz::PngFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png", true);

  CHECK(imgFlipped != img);

  CHECK(imgFlipped.recoverByteValue(0, 0, 0) == 239);
  CHECK(imgFlipped.recoverByteValue(0, 0, 1) == 239);
  CHECK(imgFlipped.recoverByteValue(0, 0, 2) == 239);
  CHECK(imgFlipped.recoverByteValue(0, 0, 3) == 255);

  CHECK(imgFlipped.recoverByteValue(1, 0, 0) == 191);
  CHECK(imgFlipped.recoverByteValue(1, 0, 1) == 191);
  CHECK(imgFlipped.recoverByteValue(1, 0, 2) == 191);
  CHECK(imgFlipped.recoverByteValue(1, 0, 3) == 255);

  CHECK(imgFlipped.recoverByteValue(0, 1, 0) == 191);
  CHECK(imgFlipped.recoverByteValue(0, 1, 1) == 191);
  CHECK(imgFlipped.recoverByteValue(0, 1, 2) == 191);
  CHECK(imgFlipped.recoverByteValue(0, 1, 3) == 255);

  CHECK(imgFlipped.recoverByteValue(1, 1, 0) == 239);
  CHECK(imgFlipped.recoverByteValue(1, 1, 1) == 239);
  CHECK(imgFlipped.recoverByteValue(1, 1, 2) == 239);
  CHECK(imgFlipped.recoverByteValue(1, 1, 3) == 255);
}

TEST_CASE("PngFormat save") {
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
  img.setByteValue(0, 0, 0, 255);
  img.setByteValue(0, 0, 1, 255);
  img.setByteValue(0, 0, 2, 255);
  img.setByteValue(0, 0, 3, 255);

  // Top-center pixel
  img.setByteValue(1, 0, 0, 255);
  img.setByteValue(1, 0, 1, 255);
  img.setByteValue(1, 0, 2, 255);
  img.setByteValue(1, 0, 3, 255);

  // Center pixel
  img.setByteValue(1, 1, 0, 255);
  img.setByteValue(1, 1, 1, 255);
  img.setByteValue(1, 1, 2, 255);
  img.setByteValue(1, 1, 3, 255);

  Raz::PngFormat::save("téstÊxpørt.png", img);

  // Checking that the reimported image is the same as the original one
  CHECK(img == Raz::PngFormat::load("téstÊxpørt.png"));

  Raz::PngFormat::save("téstÊxpørtFlipped.png", img, true);

  const Raz::Image imgFlipped = Raz::PngFormat::load("téstÊxpørtFlipped.png");

  // Image imported with a vertical flip, now should be as follows:
  //
  // -------------
  // |   |   |   |
  // -------------
  // |   | x |   |
  // -------------
  // | x | x |   |
  // -------------

  CHECK(img != imgFlipped);

  // Center pixel
  CHECK(imgFlipped.recoverByteValue(1, 1, 0) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 1, 1) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 1, 2) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 1, 3) == 255);

  // Bottom-left pixel
  CHECK(imgFlipped.recoverByteValue(0, 2, 0) == 255);
  CHECK(imgFlipped.recoverByteValue(0, 2, 1) == 255);
  CHECK(imgFlipped.recoverByteValue(0, 2, 2) == 255);
  CHECK(imgFlipped.recoverByteValue(0, 2, 3) == 255);

  // Bottom-center pixel
  CHECK(imgFlipped.recoverByteValue(1, 2, 0) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 2, 1) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 2, 2) == 255);
  CHECK(imgFlipped.recoverByteValue(1, 2, 3) == 255);

  // Re-flipping the image when saving
  Raz::PngFormat::save("téstÊxpørt.png", imgFlipped, true);

  // Checking that the re-flipped image is now equal to the original one
  CHECK(img == Raz::PngFormat::load("téstÊxpørt.png"));
}
