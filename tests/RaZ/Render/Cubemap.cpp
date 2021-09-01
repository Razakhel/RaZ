//#include "Catch.hpp"
//
//#include "RaZ/Render/Cubemap.hpp"
//#include "RaZ/Utils/FilePath.hpp"
//#include "RaZ/Utils/Image.hpp"
//
//TEST_CASE("Cubemap move") {
//  Raz::Cubemap cubemap;
//
//  const unsigned int cubemapIndex = cubemap.getIndex();
//  const unsigned int cubemapProgIndex = cubemap.getProgram().getIndex();
//
//  // Move ctor
//
//  Raz::Cubemap movedCubemapCtor(std::move(cubemap));
//
//  // The new cubemap has the same values as the original one
//  CHECK(movedCubemapCtor.getIndex() == cubemapIndex);
//  CHECK(movedCubemapCtor.getProgram().getIndex() == cubemapProgIndex);
//
//  // The moved cubemap is now invalid
//  CHECK(cubemap.getIndex() == std::numeric_limits<unsigned int>::max());
//  CHECK(cubemap.getProgram().getIndex() == std::numeric_limits<unsigned int>::max());
//
//  // Move assignment operator
//
//  Raz::Cubemap movedCubemapOp;
//
//  const unsigned int movedCubemapOpIndex = movedCubemapOp.getIndex();
//  const unsigned int movedCubemapOpProgramIndex = movedCubemapOp.getProgram().getIndex();
//
//  movedCubemapOp = std::move(movedCubemapCtor);
//
//  // The new cubemap has the same values as the previous one
//  CHECK(movedCubemapOp.getIndex() == cubemapIndex);
//  CHECK(movedCubemapOp.getProgram().getIndex() == cubemapProgIndex);
//
//  // After being moved, the values are swapped: the moved-from cubemap now has the previous moved-to's values
//  CHECK(movedCubemapCtor.getIndex() == movedCubemapOpIndex);
//  CHECK(movedCubemapCtor.getProgram().getIndex() == movedCubemapOpProgramIndex);
//}
//
//TEST_CASE("Cubemap textures") {
//  Raz::Renderer::recoverErrors(); // Flushing errors
//
//  const Raz::Image refImgPosX(RAZ_TESTS_ROOT + "assets/textures/₀₀₀₀.png"s);
//  const Raz::Image refImgNegX(RAZ_TESTS_ROOT + "assets/textures/₁₀₀₁.png"s);
//
//  const Raz::Image refImgPosY(RAZ_TESTS_ROOT + "assets/textures/₁₁₁₁.png"s);
//  const Raz::Image refImgNegY(RAZ_TESTS_ROOT + "assets/textures/ŔŖȒȐ.png"s);
//
//  const Raz::Image refImgPosZ(RAZ_TESTS_ROOT + "assets/textures/BƁḂɃ.png"s);
//  const Raz::Image refImgNegZ(RAZ_TESTS_ROOT + "assets/textures/ŔĜBŖĀ.png"s);
//
//  Raz::Cubemap cubemap(RAZ_TESTS_ROOT + "assets/textures/₀₀₀₀.png"s, RAZ_TESTS_ROOT + "assets/textures/₁₀₀₁.png"s,
//                       RAZ_TESTS_ROOT + "assets/textures/₁₁₁₁.png"s, RAZ_TESTS_ROOT + "assets/textures/ŔŖȒȐ.png"s,
//                       RAZ_TESTS_ROOT + "assets/textures/BƁḂɃ.png"s, RAZ_TESTS_ROOT + "assets/textures/ŔĜBŖĀ.png"s);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//#if !defined(USE_OPENGL_ES) // Renderer::recoverTexture*() are unavailable with OpenGL ES
//  std::vector<uint8_t> textureData {};
//
//  cubemap.bind();
//
//  // Texture +X
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_X) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_X) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_X) == Raz::TextureInternalFormat::RED);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  textureData.resize(4); // 4 black & white pixels
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_X, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosX.getDataPtr())));
//
//  // Texture -X
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_X) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_X) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_X) == Raz::TextureInternalFormat::RED);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_X, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegX.getDataPtr())));
//
//  // Texture +Y
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_Y) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_Y) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_Y) == Raz::TextureInternalFormat::RED);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_Y, 0, Raz::TextureFormat::RED, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosY.getDataPtr())));
//
//  // Texture -Y
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_Y) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_Y) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_Y) == Raz::TextureInternalFormat::RGB);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  textureData.resize(12); // 4 RGB pixels
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_Y, 0, Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegY.getDataPtr())));
//
//  // Texture +Z
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_POS_Z) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_POS_Z) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_POS_Z) == Raz::TextureInternalFormat::RGB);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_POS_Z, 0, Raz::TextureFormat::RGB, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgPosZ.getDataPtr())));
//
//  // Texture -Z
//  CHECK(Raz::Renderer::recoverTextureWidth(Raz::TextureType::CUBEMAP_NEG_Z) == 2);
//  CHECK(Raz::Renderer::recoverTextureHeight(Raz::TextureType::CUBEMAP_NEG_Z) == 2);
//  CHECK(Raz::Renderer::recoverTextureInternalFormat(Raz::TextureType::CUBEMAP_NEG_Z) == Raz::TextureInternalFormat::RGBA);
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  textureData.resize(16); // 4 RGBA pixels
//
//  Raz::Renderer::recoverTextureData(Raz::TextureType::CUBEMAP_NEG_Z, 0, Raz::TextureFormat::RGBA, Raz::TextureDataType::UBYTE, textureData.data());
//  CHECK_FALSE(Raz::Renderer::hasErrors());
//
//  CHECK(std::equal(textureData.cbegin(), textureData.cend(), static_cast<const uint8_t*>(refImgNegZ.getDataPtr())));
//
//  cubemap.unbind();
//#endif
//}
