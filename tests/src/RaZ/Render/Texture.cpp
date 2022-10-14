#include "Catch.hpp"

#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("Texture creation") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  Raz::Texture2D texture2D; // A texture can be created without dimensions and colorspace/data type
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture2D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture2D.getWidth() == 0);
  CHECK(texture2D.getHeight() == 0);
  CHECK(texture2D.getColorspace() == Raz::TextureColorspace::INVALID); // If no colorspace is given, an invalid one is assigned
  CHECK(texture2D.getDataType() == Raz::TextureDataType::BYTE);

  texture2D.resize(1, 1);
  texture2D.setColorspace(Raz::TextureColorspace::DEPTH);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture2D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture2D.getWidth() == 1);
  CHECK(texture2D.getHeight() == 1);
  CHECK(texture2D.getColorspace() == Raz::TextureColorspace::DEPTH);
  CHECK(texture2D.getDataType() == Raz::TextureDataType::FLOAT); // A depth texture is always floating-point

  const Raz::Texture3D texture3D(2, 2, 2, Raz::TextureColorspace::RGBA);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture3D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture3D.getWidth() == 2);
  CHECK(texture3D.getHeight() == 2);
  CHECK(texture3D.getDepth() == 2);
  CHECK(texture3D.getColorspace() == Raz::TextureColorspace::RGBA);
  CHECK(texture3D.getDataType() == Raz::TextureDataType::BYTE);
}

TEST_CASE("Texture move") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  Raz::Texture2D texture2D(1, 2, Raz::TextureColorspace::GRAY, Raz::TextureDataType::BYTE);
  Raz::Texture3D texture3D(3, 4, 5, Raz::TextureColorspace::RGB, Raz::TextureDataType::FLOAT);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int texture2DIndex         = texture2D.getIndex();
  const unsigned int texture2DWidth         = texture2D.getWidth();
  const unsigned int texture2DHeight        = texture2D.getHeight();
  const Raz::TextureColorspace colorspace2D = texture2D.getColorspace();
  const Raz::TextureDataType dataType2D     = texture2D.getDataType();

  const unsigned int texture3DIndex         = texture3D.getIndex();
  const unsigned int texture3DWidth         = texture3D.getWidth();
  const unsigned int texture3DHeight        = texture3D.getHeight();
  const unsigned int texture3DDepth         = texture3D.getDepth();
  const Raz::TextureColorspace colorspace3D = texture3D.getColorspace();
  const Raz::TextureDataType dataType3D     = texture3D.getDataType();

  CHECK(texture2DIndex != std::numeric_limits<unsigned int>::max());
  CHECK(texture3DIndex != std::numeric_limits<unsigned int>::max());

  // Move ctor

  Raz::Texture2D movedTexture2DCtor(std::move(texture2D));
  Raz::Texture3D movedTexture3DCtor(std::move(texture3D));

  // The new textures have the same values as the original ones
  CHECK(movedTexture2DCtor.getIndex() == texture2DIndex);
  CHECK(movedTexture2DCtor.getWidth() == texture2DWidth);
  CHECK(movedTexture2DCtor.getHeight() == texture2DHeight);
  CHECK(movedTexture2DCtor.getColorspace() == colorspace2D);
  CHECK(movedTexture2DCtor.getDataType() == dataType2D);

  CHECK(movedTexture3DCtor.getIndex() == texture3DIndex);
  CHECK(movedTexture3DCtor.getWidth() == texture3DWidth);
  CHECK(movedTexture3DCtor.getHeight() == texture3DHeight);
  CHECK(movedTexture3DCtor.getDepth() == texture3DDepth);
  CHECK(movedTexture3DCtor.getColorspace() == colorspace3D);
  CHECK(movedTexture3DCtor.getDataType() == dataType3D);

  // The moved textures are now invalid
  CHECK(texture2D.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(texture3D.getIndex() == std::numeric_limits<unsigned int>::max());

  // Move assignment operator

  Raz::Texture2D movedTexture2DOp;
  Raz::Texture3D movedTexture3DOp;
  CHECK_FALSE(Raz::Renderer::hasErrors());

  const unsigned int movedTexture2DOpIndex = movedTexture2DOp.getIndex();
  const unsigned int movedTexture3DOpIndex = movedTexture3DOp.getIndex();

  movedTexture2DOp = std::move(movedTexture2DCtor);
  movedTexture3DOp = std::move(movedTexture3DCtor);

  // The new textures have the same values as the previous ones
  CHECK(movedTexture2DOp.getIndex() == texture2DIndex);
  CHECK(movedTexture2DOp.getWidth() == texture2DWidth);
  CHECK(movedTexture2DOp.getHeight() == texture2DHeight);
  CHECK(movedTexture2DOp.getColorspace() == colorspace2D);
  CHECK(movedTexture2DOp.getDataType() == dataType2D);

  CHECK(movedTexture3DOp.getIndex() == texture3DIndex);
  CHECK(movedTexture3DOp.getWidth() == texture3DWidth);
  CHECK(movedTexture3DOp.getHeight() == texture3DHeight);
  CHECK(movedTexture3DOp.getDepth() == texture3DDepth);
  CHECK(movedTexture3DOp.getColorspace() == colorspace3D);
  CHECK(movedTexture3DOp.getDataType() == dataType3D);

  // After being moved, the values are swapped: the moved-from textures now have the previous moved-to's values
  CHECK(movedTexture2DCtor.getIndex() == movedTexture2DOpIndex);
  CHECK(movedTexture3DCtor.getIndex() == movedTexture3DOpIndex);
}

TEST_CASE("Texture presets") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::Texture2DPtr whiteTexture = Raz::Texture2D::create(Raz::ColorPreset::White);
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
  CHECK(Raz::Renderer::recoverTextureDepth(Raz::TextureType::TEXTURE_2D) == 1);
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
  const Raz::Texture2DPtr whiteTexture2 = Raz::Texture2D::create(Raz::ColorPreset::White);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK_FALSE(whiteTexture2.get() == whiteTexture.get());
  CHECK_FALSE(whiteTexture2->getIndex() == whiteTexture->getIndex());
}
