#include "Catch.hpp"

#include "RaZ/Data/PngFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Image.hpp"

TEST_CASE("PngFormat load") {
  const Raz::Image img = Raz::PngFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png");

  CHECK(img.getWidth() == 2);
  CHECK(img.getHeight() == 2);
  CHECK(img.getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(img.getDataType() == Raz::ImageDataType::BYTE);
  CHECK_FALSE(img.isEmpty());

  CHECK(*static_cast<const uint8_t*>(img.getDataPtr())       == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 1) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 2) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 3) == 255);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 4) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 5) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 6) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 7) == 255);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 8)  == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 9)  == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 10) == 239);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 11) == 255);

  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 12) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 13) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 14) == 191);
  CHECK(*(static_cast<const uint8_t*>(img.getDataPtr()) + 15) == 255);

  const Raz::Image imgFlipped = Raz::PngFormat::load(RAZ_TESTS_ROOT "assets/images/dëfàùltTêst.png", true);

  CHECK(imgFlipped != img);

  CHECK(*static_cast<const uint8_t*>(imgFlipped.getDataPtr())       == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 1) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 2) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 3) == 255);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 4) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 5) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 6) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 7) == 255);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 8)  == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 9)  == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 10) == 191);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 11) == 255);

  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 12) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 13) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 14) == 239);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 15) == 255);
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
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 16) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 17) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 18) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 19) == 255);

  // Bottom-left pixel
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 24) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 25) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 26) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 27) == 255);

  // Bottom-center pixel
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 28) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 29) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 30) == 255);
  CHECK(*(static_cast<const uint8_t*>(imgFlipped.getDataPtr()) + 31) == 255);

  // Re-flipping the image when saving
  Raz::PngFormat::save("téstÊxpørt.png", imgFlipped, true);

  // Checking that the re-flipped image is now equal to the original one
  CHECK(img == Raz::PngFormat::load("téstÊxpørt.png"));
}
