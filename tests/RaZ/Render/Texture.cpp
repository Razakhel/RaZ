#include "Catch.hpp"

#include "RaZ/Render/Texture.hpp"

TEST_CASE("Texture move") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::Image refImg(RAZ_TESTS_ROOT + "assets/textures/RGBRA.png"s);

  Raz::Texture texture(RAZ_TESTS_ROOT + "assets/textures/RGBRA.png"s, 42);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int textureIndex = texture.getIndex();

  // Move ctor

  Raz::Texture movedTextureCtor(std::move(texture));

  // The new texture has the same values as the original one
  CHECK(movedTextureCtor.getIndex() == textureIndex);
  CHECK(movedTextureCtor.getBindingIndex() == 42);
  CHECK_FALSE(movedTextureCtor.getImage().isEmpty());
  CHECK(movedTextureCtor.getImage() == refImg);

  // The moved texture is now invalid
  CHECK(texture.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(texture.getBindingIndex() == std::numeric_limits<int>::max());
  CHECK(texture.getImage().isEmpty());

  // Move assignment operator

  Raz::Texture movedTextureOp;
  CHECK_FALSE(Raz::Renderer::hasErrors());
  movedTextureOp.setBindingIndex(255);

  const unsigned int movedTextureOpIndex = movedTextureOp.getIndex();

  movedTextureOp = std::move(movedTextureCtor);

  // The new texture has the same values as the previous one
  CHECK(movedTextureOp.getIndex() == textureIndex);
  CHECK(movedTextureOp.getBindingIndex() == 42);
  CHECK_FALSE(movedTextureOp.getImage().isEmpty());
  CHECK(movedTextureOp.getImage() == refImg);

  // After being moved, the values are swapped: the moved-from texture now has the previous moved-to's values
  // The moved-from image is however always directly moved, thus invalidated
  CHECK(movedTextureCtor.getIndex() == movedTextureOpIndex);
  CHECK(movedTextureCtor.getBindingIndex() == 255);
  CHECK(movedTextureCtor.getImage().isEmpty());
}

TEST_CASE("Texture presets") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::TexturePtr whiteTexture = Raz::Texture::create(Raz::ColorPreset::WHITE, 42);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(whiteTexture->getBindingIndex() == 42);
  CHECK(whiteTexture->getImage().isEmpty()); // The image's data is untouched, no allocation is made

  // Recovering the texture's data (1 RGB pixel -> 3 values)
  std::array<uint8_t, 3> textureData {};

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

  // Creating another texture from the same preset gives a different one; both aren't linked
  const Raz::TexturePtr whiteTexture2 = Raz::Texture::create(Raz::ColorPreset::WHITE);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK_FALSE(whiteTexture2.get() == whiteTexture.get());
  CHECK_FALSE(whiteTexture2->getIndex() == whiteTexture->getIndex());
  CHECK(whiteTexture2->getBindingIndex() == std::numeric_limits<int>::max());
}
