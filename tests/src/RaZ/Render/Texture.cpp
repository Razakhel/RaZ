#include "Catch.hpp"

#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("Texture creation") {
  Raz::Texture textureEmpty; // A texture can be created without dimensions and colorspace/data type
  CHECK(textureEmpty.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureEmpty.getImage().getWidth() == 0);
  CHECK(textureEmpty.getImage().getHeight() == 0);
  CHECK(textureEmpty.getImage().getColorspace() == Raz::ImageColorspace::GRAY); // TODO: there should be an invalid colorspace, not a default one
  CHECK(textureEmpty.getImage().getDataType() == Raz::ImageDataType::BYTE);
  CHECK(textureEmpty.getImage().getChannelCount() == 0);
  CHECK(textureEmpty.getImage().isEmpty());

  textureEmpty.setParameters(1, 1, Raz::ImageColorspace::RGBA);
  CHECK(textureEmpty.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureEmpty.getImage().getWidth() == 1); // Resizing a texture sets the dimensions to the image
  CHECK(textureEmpty.getImage().getHeight() == 1);
  CHECK(textureEmpty.getImage().getColorspace() == Raz::ImageColorspace::RGBA);
  CHECK(textureEmpty.getImage().getDataType() == Raz::ImageDataType::BYTE);
  CHECK(textureEmpty.getImage().getChannelCount() == 4);
  CHECK(textureEmpty.getImage().isEmpty()); // Resizing a texture does not modify the contained image

  const Raz::Texture textureSmall(1, 1, Raz::ImageColorspace::DEPTH);
  CHECK(textureSmall.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureSmall.getImage().getWidth() == 1);
  CHECK(textureSmall.getImage().getHeight() == 1);
  CHECK(textureSmall.getImage().getColorspace() == Raz::ImageColorspace::DEPTH);
  CHECK(textureSmall.getImage().getDataType() == Raz::ImageDataType::FLOAT); // A depth texture is always floating-point
  CHECK(textureSmall.getImage().getChannelCount() == 1);
  CHECK(textureSmall.getImage().isEmpty());
}

TEST_CASE("Texture move") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::Image refImg = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔĜBŖĀ.png");

  Raz::Texture texture(Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔĜBŖĀ.png"), false);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int textureIndex = texture.getIndex();

  // Move ctor

  Raz::Texture movedTextureCtor(std::move(texture));

  // The new texture has the same values as the original one
  CHECK(movedTextureCtor.getIndex() == textureIndex);
  CHECK_FALSE(movedTextureCtor.getImage().isEmpty());
  CHECK(movedTextureCtor.getImage() == refImg);

  // The moved texture is now invalid
  CHECK(texture.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(texture.getImage().isEmpty());

  // Move assignment operator

  Raz::Texture movedTextureOp;
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int movedTextureOpIndex = movedTextureOp.getIndex();

  movedTextureOp = std::move(movedTextureCtor);

  // The new texture has the same values as the previous one
  CHECK(movedTextureOp.getIndex() == textureIndex);
  CHECK_FALSE(movedTextureOp.getImage().isEmpty());
  CHECK(movedTextureOp.getImage() == refImg);

  // After being moved, the values are swapped: the moved-from texture now has the previous moved-to's values
  // The moved-from image is however always directly moved, thus invalidated
  CHECK(movedTextureCtor.getIndex() == movedTextureOpIndex);
  CHECK(movedTextureCtor.getImage().isEmpty());
}

TEST_CASE("Texture presets") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::TexturePtr whiteTexture = Raz::Texture::create(Raz::ColorPreset::White);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(whiteTexture->getImage().getWidth() == 1); // The width & height are set to 1
  CHECK(whiteTexture->getImage().getHeight() == 1);
  CHECK(whiteTexture->getImage().isEmpty()); // However, the image's data is untouched; no allocation is made

#if !defined(USE_OPENGL_ES) // Renderer::recoverTexture*() are unavailable with OpenGL ES
  std::array<uint8_t, 3> textureData {}; // Recovering the texture's data (1 RGB pixel -> 3 values)

  whiteTexture->bind();

  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::TEXTURE_2D) == 1);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::TEXTURE_2D) == 1);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::TEXTURE_2D) == Raz::TextureInternalFormat::RGB);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  Raz::Renderer::recoverTextureData(Raz::TextureType::TEXTURE_2D, 0, Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  whiteTexture->unbind();

  CHECK(textureData[0] == 255);
  CHECK(textureData[1] == 255);
  CHECK(textureData[2] == 255);
#endif

  // Creating another texture from the same preset gives a different one; both aren't linked
  const Raz::TexturePtr whiteTexture2 = Raz::Texture::create(Raz::ColorPreset::White);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK_FALSE(whiteTexture2.get() == whiteTexture.get());
  CHECK_FALSE(whiteTexture2->getIndex() == whiteTexture->getIndex());
}
