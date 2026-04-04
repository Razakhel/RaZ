#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/ImageUtils.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/FilePath.hpp"

#include "CatchCustomMatchers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

constexpr unsigned int equirectangularSize = 32;

Raz::Image createEquirectangularImage(Raz::ImageDataType dataType) {
  Raz::Image img(equirectangularSize * 4, equirectangularSize * 2, Raz::ImageColorspace::RGB, dataType);

  for (unsigned int heightIndex = 0; heightIndex < img.getHeight(); ++heightIndex) {
    const Raz::Color vertColor(0.f, static_cast<float>(heightIndex) / static_cast<float>(img.getHeight()), 0.f);

    for (unsigned int widthIndex = 0; widthIndex < img.getWidth(); ++widthIndex) {
      const Raz::Color horizColor(static_cast<float>(widthIndex) / static_cast<float>(img.getWidth()), 0.f, 0.f);
      const Raz::Vec3f pixelVal(std::max(horizColor.red(), vertColor.red()),
                                std::max(horizColor.green(), vertColor.green()),
                                std::max(horizColor.blue(), vertColor.blue()));
      if (dataType == Raz::ImageDataType::FLOAT)
        img.setPixel(widthIndex, heightIndex, pixelVal);
      else
        img.setPixel(widthIndex, heightIndex, Raz::Vec3b(pixelVal * 255.f));
    }
  }

  return img;
}

} // namespace

TEST_CASE("ImageUtils equirectangular to cubemap", "[data]") {
  constexpr std::array<std::string_view, 6> cubemapFacesSuffixes = { "right", "left", "top", "bottom", "front", "back" };

  SECTION("From byte image") {
    const std::array<Raz::Image, 6> cubemapFaces = Raz::ImageUtils::convertEquirectangularToCubemap(createEquirectangularImage(Raz::ImageDataType::BYTE));

    for (std::size_t faceIndex = 0; faceIndex < cubemapFaces.size(); ++faceIndex) {
      const Raz::Image& faceImg = cubemapFaces[faceIndex];
      CHECK(faceImg.getWidth() == equirectangularSize);
      CHECK(faceImg.getHeight() == equirectangularSize);
      CHECK(faceImg.getDataType() == Raz::ImageDataType::BYTE);
      CHECK(faceImg == Raz::ImageFormat::load(std::format(RAZ_TESTS_ROOT "assets/textures/cubemap_{}.png", cubemapFacesSuffixes[faceIndex])));
    }
  }

  SECTION("From float image") {
    const std::array<Raz::Image, 6> cubemapFaces = Raz::ImageUtils::convertEquirectangularToCubemap(createEquirectangularImage(Raz::ImageDataType::FLOAT));

    for (std::size_t faceIndex = 0; faceIndex < cubemapFaces.size(); ++faceIndex) {
      const Raz::Image& faceImg = cubemapFaces[faceIndex];
      CHECK(faceImg.getWidth() == equirectangularSize);
      CHECK(faceImg.getHeight() == equirectangularSize);
      CHECK(faceImg.getDataType() == Raz::ImageDataType::FLOAT);

      const Raz::Image compImg = Raz::ImageFormat::load(std::format(RAZ_TESTS_ROOT "assets/textures/cubemap_{}.png", cubemapFacesSuffixes[faceIndex]));

      // Comparing pixel by pixel since the values of both images aren't in the same range
      for (std::size_t heightIndex = 0; heightIndex < faceImg.getHeight(); ++heightIndex) {
        for (std::size_t widthIndex = 0; widthIndex < faceImg.getWidth(); ++widthIndex) {
          const Raz::Vec3f convertedPixel = faceImg.recoverPixel<float, 3>(widthIndex, heightIndex);
          const Raz::Vec3f compFloatPixel = Raz::Vec3f(compImg.recoverPixel<uint8_t, 3>(widthIndex, heightIndex)) / 255.f;
          CHECK_THAT(convertedPixel, IsNearlyEqualToVector(compFloatPixel, 0.006f));
        }
      }
    }
  }
}
