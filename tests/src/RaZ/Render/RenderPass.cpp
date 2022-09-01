#include "Catch.hpp"

#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("RenderPass validity") {
  Raz::RenderPass initialPass;
  CHECK(initialPass.isValid()); // The pass has no buffer, thus is valid

  const auto depthBuffer = Raz::Texture::create(Raz::TextureColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture::create(Raz::TextureColorspace::RGB);

  initialPass.setWriteDepthTexture(depthBuffer);
  CHECK(initialPass.isValid()); // Direct buffer dependency is not checked; even though no child exists, it is not required to have matching buffers

  Raz::RenderPass nextPass;

  initialPass.addChildren(nextPass);
  CHECK(initialPass.isValid()); // A subsequent pass exists, but doesn't change the previous result
  CHECK(nextPass.isValid());

  nextPass.addReadTexture(colorBuffer, ""); // A uniform name is unnecessary here
  CHECK(initialPass.isValid()); // Matching buffers being unrequired, the first pass is still valid
  CHECK(nextPass.isValid());

  initialPass.addReadTexture(depthBuffer, "");
  CHECK_FALSE(initialPass.isValid()); // The depth buffer is defined as both read & write in the first pass
  CHECK(nextPass.isValid());

  nextPass.addWriteColorTexture(colorBuffer, 0);
  CHECK_FALSE(initialPass.isValid());
  CHECK_FALSE(nextPass.isValid()); // The color buffer is defined as both read & write in the second pass

  initialPass.removeReadTexture(depthBuffer);
  CHECK(initialPass.isValid()); // The depth buffer is only set as write, the pass is valid again
  CHECK_FALSE(nextPass.isValid());

  nextPass.removeWriteTexture(colorBuffer);
  CHECK(initialPass.isValid());
  CHECK(nextPass.isValid()); // The color buffer is only set as read, the pass is valid again
}

TEST_CASE("RenderPass textures") {
  Raz::RenderPass initialPass;
  CHECK(initialPass.getReadTextureCount() == 0);
  CHECK_FALSE(initialPass.getFramebuffer().hasDepthBuffer());
  CHECK(initialPass.getFramebuffer().getColorBufferCount() == 0);

  const auto depthBuffer = Raz::Texture::create(Raz::TextureColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture::create(Raz::TextureColorspace::RGB);

  initialPass.setWriteDepthTexture(depthBuffer);
  CHECK(initialPass.getReadTextureCount() == 0);
  REQUIRE(initialPass.getFramebuffer().hasDepthBuffer());
  CHECK(initialPass.getFramebuffer().getDepthBuffer().getIndex() == depthBuffer->getIndex());
  CHECK(initialPass.getFramebuffer().getColorBufferCount() == 0);

  Raz::RenderPass nextPass;

  nextPass.addReadTexture(colorBuffer, "");
  REQUIRE(nextPass.getReadTextureCount() == 1);
  CHECK(nextPass.getReadTexture(0).getIndex() == colorBuffer->getIndex());
  CHECK_FALSE(nextPass.getFramebuffer().hasDepthBuffer());
  CHECK(nextPass.getFramebuffer().getColorBufferCount() == 0);

  initialPass.addChildren(nextPass);
  CHECK(initialPass.getFramebuffer().getColorBufferCount() == 0); // Setting a pass dependency does not add any buffer automatically
  CHECK(nextPass.getReadTextureCount() == 1);

  initialPass.addWriteColorTexture(colorBuffer, 0);
  REQUIRE(initialPass.getFramebuffer().getColorBufferCount() == 1);
  REQUIRE(nextPass.getReadTextureCount() == 1);
  CHECK(initialPass.getFramebuffer().getColorBuffer(0).getIndex() == nextPass.getReadTexture(0).getIndex()); // The color buffer is the same in both passes

  nextPass.removeParents(initialPass);
  CHECK(initialPass.getFramebuffer().getColorBufferCount() == 1); // Removing a pass dependency does not remove any buffer automatically
  CHECK(nextPass.getReadTextureCount() == 1);

  initialPass.clearWriteTextures();
  CHECK_FALSE(initialPass.getFramebuffer().hasDepthBuffer());
  CHECK(initialPass.getFramebuffer().getColorBufferCount() == 0);

  nextPass.clearReadTextures();
  CHECK(nextPass.getReadTextureCount() == 0);
}
