#include "Catch.hpp"

#include "RaZ/Data/Color.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("Texture creation") {
  Raz::Renderer::recoverErrors(); // Flushing errors

#if !defined(USE_OPENGL_ES) // 1D textures are unavailable with OpenGL ES
  const Raz::Texture1D texture1D; // A texture can be created without dimensions and colorspace/data type
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture1D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture1D.getWidth() == 0);
  CHECK(texture1D.getColorspace() == Raz::TextureColorspace::INVALID); // If no colorspace is given, an invalid one is assigned
  CHECK(texture1D.getDataType() == Raz::TextureDataType::BYTE);
#endif

  Raz::Texture2D texture2D(Raz::TextureColorspace::GRAY); // A texture can be created with colorspace/data type, but without dimensions
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture2D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture2D.getWidth() == 0);
  CHECK(texture2D.getHeight() == 0);
  CHECK(texture2D.getColorspace() == Raz::TextureColorspace::GRAY);
  CHECK(texture2D.getDataType() == Raz::TextureDataType::BYTE);

  texture2D.resize(1, 2);
  texture2D.setColorspace(Raz::TextureColorspace::DEPTH);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture2D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture2D.getWidth() == 1);
  CHECK(texture2D.getHeight() == 2);
  CHECK(texture2D.getColorspace() == Raz::TextureColorspace::DEPTH);
  CHECK(texture2D.getDataType() == Raz::TextureDataType::FLOAT32); // A depth texture is always 32-bit floating-point

  const Raz::Texture3D texture3D(3, 4, 5, Raz::TextureColorspace::RGBA);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(texture3D.getIndex() != std::numeric_limits<unsigned int>::max());
  CHECK(texture3D.getWidth() == 3);
  CHECK(texture3D.getHeight() == 4);
  CHECK(texture3D.getDepth() == 5);
  CHECK(texture3D.getColorspace() == Raz::TextureColorspace::RGBA);
  CHECK(texture3D.getDataType() == Raz::TextureDataType::BYTE);
}

TEST_CASE("Texture move") {
  Raz::Renderer::recoverErrors(); // Flushing errors

#if !defined(USE_OPENGL_ES) // 1D textures are unavailable with OpenGL ES
  Raz::Texture1D texture1D(1, Raz::TextureColorspace::GRAY, Raz::TextureDataType::BYTE);
#endif
  Raz::Texture2D texture2D(2, 3, Raz::TextureColorspace::RGB, Raz::TextureDataType::FLOAT16);
  Raz::Texture3D texture3D(4, 5, 6, Raz::TextureColorspace::RGBA, Raz::TextureDataType::FLOAT32);
  CHECK_FALSE(Raz::Renderer::hasErrors());

#if !defined(USE_OPENGL_ES)
  const unsigned int texture1DIndex         = texture1D.getIndex();
  const unsigned int texture1DWidth         = texture1D.getWidth();
  const Raz::TextureColorspace colorspace1D = texture1D.getColorspace();
  const Raz::TextureDataType dataType1D     = texture1D.getDataType();
#endif

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

#if !defined(USE_OPENGL_ES)
  CHECK(texture1DIndex != std::numeric_limits<unsigned int>::max());
#endif
  CHECK(texture2DIndex != std::numeric_limits<unsigned int>::max());
  CHECK(texture3DIndex != std::numeric_limits<unsigned int>::max());

  // Move ctor

#if !defined(USE_OPENGL_ES)
  Raz::Texture1D movedTexture1DCtor(std::move(texture1D));
#endif
  Raz::Texture2D movedTexture2DCtor(std::move(texture2D));
  Raz::Texture3D movedTexture3DCtor(std::move(texture3D));

  // The new textures have the same values as the original ones
#if !defined(USE_OPENGL_ES)
  CHECK(movedTexture1DCtor.getIndex() == texture1DIndex);
  CHECK(movedTexture1DCtor.getWidth() == texture1DWidth);
  CHECK(movedTexture1DCtor.getColorspace() == colorspace1D);
  CHECK(movedTexture1DCtor.getDataType() == dataType1D);
#endif

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
#if !defined(USE_OPENGL_ES)
  CHECK(texture1D.getIndex() == std::numeric_limits<unsigned int>::max());
#endif
  CHECK(texture2D.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK(texture3D.getIndex() == std::numeric_limits<unsigned int>::max());

  // Move assignment operator

#if !defined(USE_OPENGL_ES)
  Raz::Texture1D movedTexture1DOp;
#endif
  Raz::Texture2D movedTexture2DOp;
  Raz::Texture3D movedTexture3DOp;
  CHECK_FALSE(Raz::Renderer::hasErrors());

#if !defined(USE_OPENGL_ES)
  const unsigned int movedTexture1DOpIndex = movedTexture1DOp.getIndex();
#endif
  const unsigned int movedTexture2DOpIndex = movedTexture2DOp.getIndex();
  const unsigned int movedTexture3DOpIndex = movedTexture3DOp.getIndex();

#if !defined(USE_OPENGL_ES)
  movedTexture1DOp = std::move(movedTexture1DCtor);
#endif
  movedTexture2DOp = std::move(movedTexture2DCtor);
  movedTexture3DOp = std::move(movedTexture3DCtor);

  // The new textures have the same values as the previous ones
#if !defined(USE_OPENGL_ES)
  CHECK(movedTexture1DOp.getIndex() == texture1DIndex);
  CHECK(movedTexture1DOp.getWidth() == texture1DWidth);
  CHECK(movedTexture1DOp.getColorspace() == colorspace1D);
  CHECK(movedTexture1DOp.getDataType() == dataType1D);
#endif

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
#if !defined(USE_OPENGL_ES)
  CHECK(movedTexture1DCtor.getIndex() == movedTexture1DOpIndex);
#endif
  CHECK(movedTexture2DCtor.getIndex() == movedTexture2DOpIndex);
  CHECK(movedTexture3DCtor.getIndex() == movedTexture3DOpIndex);
}

TEST_CASE("Texture fill") {
  Raz::Renderer::recoverErrors(); // Flushing errors

#if !defined(USE_OPENGL_ES)
  const Raz::Texture1DPtr texture1D = Raz::Texture1D::create(Raz::ColorPreset::Red, 3);
#endif
  const Raz::Texture2DPtr texture2D = Raz::Texture2D::create(Raz::ColorPreset::Green, 3, 3);
  const Raz::Texture3DPtr texture3D = Raz::Texture3D::create(Raz::ColorPreset::Blue, 3, 3, 3);
  CHECK_FALSE(Raz::Renderer::hasErrors());

#if !defined(USE_OPENGL_ES)
  CHECK(texture1D->getWidth() == 3);
  CHECK(texture1D->getColorspace() == Raz::TextureColorspace::RGB);
  CHECK(texture1D->getDataType() == Raz::TextureDataType::BYTE);
#endif

  CHECK(texture2D->getWidth() == 3);
  CHECK(texture2D->getHeight() == 3);
  CHECK(texture2D->getColorspace() == Raz::TextureColorspace::RGB);
  CHECK(texture2D->getDataType() == Raz::TextureDataType::BYTE);

  CHECK(texture3D->getWidth() == 3);
  CHECK(texture3D->getHeight() == 3);
  CHECK(texture3D->getDepth() == 3);
  CHECK(texture3D->getColorspace() == Raz::TextureColorspace::RGB);
  CHECK(texture3D->getDataType() == Raz::TextureDataType::BYTE);

#if !defined(USE_OPENGL_ES) // Renderer::recoverTexture*() are unavailable with OpenGL ES
  {
    texture1D->bind();

    REQUIRE(Raz::Renderer::recoverTextureWidth(Raz::TextureType::TEXTURE_1D) == 3);
    REQUIRE(Raz::Renderer::recoverTextureHeight(Raz::TextureType::TEXTURE_1D) == 1);
    REQUIRE(Raz::Renderer::recoverTextureDepth(Raz::TextureType::TEXTURE_1D) == 1);
    CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::TEXTURE_1D) == Raz::TextureInternalFormat::RGB);
    CHECK_FALSE(Raz::Renderer::hasErrors());

    std::array<uint8_t, 9> textureData {};

    Raz::Renderer::recoverTextureData(Raz::TextureType::TEXTURE_1D, 0, Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, textureData.data());
    CHECK_FALSE(Raz::Renderer::hasErrors());

    texture1D->unbind();

    for (std::size_t i = 0; i < textureData.size(); i += 3) {
      CHECK(textureData[i]     == 255);
      CHECK(textureData[i + 1] == 0);
      CHECK(textureData[i + 2] == 0);
    }
  }

  {
    texture2D->bind();

    REQUIRE(Raz::Renderer::recoverTextureWidth(Raz::TextureType::TEXTURE_2D) == 3);
    REQUIRE(Raz::Renderer::recoverTextureHeight(Raz::TextureType::TEXTURE_2D) == 3);
    REQUIRE(Raz::Renderer::recoverTextureDepth(Raz::TextureType::TEXTURE_2D) == 1);
    CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::TEXTURE_2D) == Raz::TextureInternalFormat::RGB);
    CHECK_FALSE(Raz::Renderer::hasErrors());

    std::array<uint8_t, 27> textureData {};

    Raz::Renderer::recoverTextureData(Raz::TextureType::TEXTURE_2D, 0, Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, textureData.data());
    CHECK_FALSE(Raz::Renderer::hasErrors());

    texture2D->unbind();

    for (std::size_t i = 0; i < textureData.size(); i += 3) {
      CHECK(textureData[i]     == 0);
      CHECK(textureData[i + 1] == 255);
      CHECK(textureData[i + 2] == 0);
    }
  }

  {
    texture3D->bind();

    REQUIRE(Raz::Renderer::recoverTextureWidth(Raz::TextureType::TEXTURE_3D) == 3);
    REQUIRE(Raz::Renderer::recoverTextureHeight(Raz::TextureType::TEXTURE_3D) == 3);
    REQUIRE(Raz::Renderer::recoverTextureDepth(Raz::TextureType::TEXTURE_3D) == 3);
    CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::TEXTURE_3D) == Raz::TextureInternalFormat::RGB);
    CHECK_FALSE(Raz::Renderer::hasErrors());

    std::array<uint8_t, 81> textureData {};

    Raz::Renderer::recoverTextureData(Raz::TextureType::TEXTURE_3D, 0, Raz::TextureFormat::RGB, Raz::PixelDataType::UBYTE, textureData.data());
    CHECK_FALSE(Raz::Renderer::hasErrors());

    texture3D->unbind();

    for (std::size_t i = 0; i < textureData.size(); i += 3) {
      CHECK(textureData[i]     == 0);
      CHECK(textureData[i + 1] == 0);
      CHECK(textureData[i + 2] == 255);
    }
  }
#endif
}
