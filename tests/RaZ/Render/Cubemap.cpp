#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/FilePath.hpp"

TEST_CASE("Cubemap move") {
  Raz::Cubemap cubemap;

  const unsigned int cubemapIndex = cubemap.getIndex();

  // Move ctor

  Raz::Cubemap movedCubemapCtor(std::move(cubemap));

  // The new cubemap has the same index as the original one
  CHECK(movedCubemapCtor.getIndex() == cubemapIndex);

  // The moved cubemap is now invalid
  CHECK(cubemap.getIndex() == std::numeric_limits<unsigned int>::max());

  // Move assignment operator

  Raz::Cubemap movedCubemapOp;

  const unsigned int movedCubemapOpIndex = movedCubemapOp.getIndex();

  movedCubemapOp = std::move(movedCubemapCtor);

  // The new cubemap has the same index as the previous one
  CHECK(movedCubemapOp.getIndex() == cubemapIndex);

  // After being moved, the indices are swapped: the moved-from cubemap now has the previous moved-to's index
  CHECK(movedCubemapCtor.getIndex() == movedCubemapOpIndex);
}

TEST_CASE("Cubemap textures") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::Image refImgPosX = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/₀₀₀₀.png");
  const Raz::Image refImgNegX = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/₁₀₀₁.png");

  const Raz::Image refImgPosY = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/₁₁₁₁.png");
  const Raz::Image refImgNegY = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔŖȒȐ.png");

  const Raz::Image refImgPosZ = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/BƁḂɃ.png");
  const Raz::Image refImgNegZ = Raz::ImageFormat::load(RAZ_TESTS_ROOT "assets/textures/ŔĜBŖĀ.png");

  Raz::Cubemap cubemap(refImgPosX, refImgNegX, refImgPosY, refImgNegY, refImgPosZ, refImgNegZ);
  CHECK_FALSE(Raz::Renderer::hasErrors());

#if !defined(USE_OPENGL_ES) // Renderer::recoverTexture*() functions are unavailable with OpenGL ES
  std::vector<uint8_t> textureData {};

  cubemap.bind();

  // Texture +X
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_X) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_X) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_X) == Raz::TextureInternalFormat::RED);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  textureData.resize(4); // 4 black & white pixels

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_X, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosX.getDataPtr())));

  // Texture -X
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_X) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_X) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_X) == Raz::TextureInternalFormat::RED);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_X, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegX.getDataPtr())));

  // Texture +Y
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_Y) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_Y) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_Y) == Raz::TextureInternalFormat::RED);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_Y, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosY.getDataPtr())));

  // Texture -Y
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_Y) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_Y) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_Y) == Raz::TextureInternalFormat::RGB);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  textureData.resize(12); // 4 RGB pixels

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_Y, 0, Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegY.getDataPtr())));

  // Texture +Z
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_Z) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_Z) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_Z) == Raz::TextureInternalFormat::RGB);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_Z, 0, Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosZ.getDataPtr())));

  // Texture -Z
  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_Z) == 2);
  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_Z) == 2);
  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_Z) == Raz::TextureInternalFormat::RGBA);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  textureData.resize(16); // 4 RGBA pixels

  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_Z, 0, Raz::TextureFormat::RGBA, Raz::TextureDataType::UBYTE, textureData.data());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegZ.getDataPtr())));

  cubemap.unbind();
#endif
}
