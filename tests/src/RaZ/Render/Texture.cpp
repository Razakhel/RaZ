#include "Catch.hpp"

#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("Texture creation") {
  Raz::Texture textureEmpty; // A texture can be created without dimensions and colorspace/data type
  CHECK(textureEmpty.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureEmpty.getWidth() == 0);
  CHECK(textureEmpty.getHeight() == 0);
  CHECK(textureEmpty.getColorspace() == Raz::TextureColorspace::INVALID);
  CHECK(textureEmpty.getDataType() == Raz::TextureDataType::BYTE);

  textureEmpty.setParameters(1, 1, Raz::TextureColorspace::RGBA);
  CHECK(textureEmpty.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureEmpty.getWidth() == 1);
  CHECK(textureEmpty.getHeight() == 1);
  CHECK(textureEmpty.getColorspace() == Raz::TextureColorspace::RGBA);
  CHECK(textureEmpty.getDataType() == Raz::TextureDataType::BYTE);

  const Raz::Texture textureSmall(1, 1, Raz::TextureColorspace::DEPTH);
  CHECK(textureSmall.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(textureSmall.getWidth() == 1);
  CHECK(textureSmall.getHeight() == 1);
  CHECK(textureSmall.getColorspace() == Raz::TextureColorspace::DEPTH);
  CHECK(textureSmall.getDataType() == Raz::TextureDataType::FLOAT); // A depth texture is always floating-point
}

TEST_CASE("Texture move") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  Raz::Texture texture(3, 3, Raz::TextureColorspace::GRAY, Raz::TextureDataType::FLOAT);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int textureIndex         = texture.getIndex();
  const unsigned int textureWidth         = texture.getWidth();
  const unsigned int textureHeight        = texture.getHeight();
  const Raz::TextureColorspace colorspace = texture.getColorspace();
  const Raz::TextureDataType dataType     = texture.getDataType();

  // Move ctor

  Raz::Texture movedTextureCtor(std::move(texture));

  // The new texture has the same values as the original one
  CHECK(movedTextureCtor.getIndex() == textureIndex);
  CHECK(movedTextureCtor.getWidth() == textureWidth);
  CHECK(movedTextureCtor.getHeight() == textureHeight);
  CHECK(movedTextureCtor.getColorspace() == colorspace);
  CHECK(movedTextureCtor.getDataType() == dataType);

  // The moved texture is now invalid
  CHECK(texture.getIndex() == std::numeric_limits<unsigned int>::max());

  // Move assignment operator

  Raz::Texture movedTextureOp;
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int movedTextureOpIndex = movedTextureOp.getIndex();

  movedTextureOp = std::move(movedTextureCtor);

  // The new texture has the same values as the previous one
  CHECK(movedTextureOp.getIndex() == textureIndex);
  CHECK(movedTextureOp.getWidth() == textureWidth);
  CHECK(movedTextureOp.getHeight() == textureHeight);
  CHECK(movedTextureOp.getColorspace() == colorspace);
  CHECK(movedTextureOp.getDataType() == dataType);

  // After being moved, the values are swapped: the moved-from texture now has the previous moved-to's values
  // The moved-from image is however always directly moved, thus invalidated
  CHECK(movedTextureCtor.getIndex() == movedTextureOpIndex);
}

TEST_CASE("Texture presets") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::TexturePtr whiteTexture = Raz::Texture::create(Raz::ColorPreset::White);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(whiteTexture->getWidth() == 1);
  CHECK(whiteTexture->getHeight() == 1);
  CHECK(whiteTexture->getColorspace() == Raz::TextureColorspace::RGB);
  CHECK(whiteTexture->getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES) // Renderer::recoverTexture*() are unavailable with OpenGL ES
  std::array<uint8_t, 3> textureData {}; // Recovering the texture's data (1 RGB pixel -> 3 values)

  whiteTexture->bind();

  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::TEXTURE_2D) == 1);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::TEXTURE_2D) == 1);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::TEXTURE_2D) == Raz::TextureInternalFormat::RGB);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  Raz::Renderer::recoverTextureData(Raz::TextureType::TEXTURE_2D, 0, Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, textureData.data());
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
