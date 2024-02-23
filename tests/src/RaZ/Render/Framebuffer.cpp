#include "RaZ/Render/Framebuffer.hpp"
#include "RaZ/Render/Texture.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Framebuffer buffers", "[render]") {
  Raz::Framebuffer framebuffer;
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 0);
  CHECK(framebuffer.isEmpty()); // Combining the above checks

  // The buffers need to have a size for the framebuffer to be complete
  const auto depthBuffer = Raz::Texture2D::create(Raz::TextureColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture2D::create(Raz::TextureColorspace::RGB);

  // Setting a color buffer as depth or adding a depth buffer as color throws an exception
  CHECK_THROWS(framebuffer.setDepthBuffer(colorBuffer));
  CHECK_THROWS(framebuffer.addColorBuffer(depthBuffer, 0));

  framebuffer.setDepthBuffer(depthBuffer);
  REQUIRE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  CHECK(framebuffer.getColorBufferCount() == 0);

  framebuffer.addColorBuffer(colorBuffer, 0);
  REQUIRE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  REQUIRE(framebuffer.getColorBufferCount() == 1);
  CHECK(framebuffer.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  CHECK(depthBuffer->getWidth() == 0);
  CHECK(depthBuffer->getHeight() == 0);
  CHECK(colorBuffer->getWidth() == 0);
  CHECK(colorBuffer->getHeight() == 0);

  framebuffer.resizeBuffers(1, 1);
  CHECK(depthBuffer->getWidth() == 1); // Resizing the framebuffer resizes all contained buffers
  CHECK(depthBuffer->getHeight() == 1);
  CHECK(colorBuffer->getWidth() == 1);
  CHECK(colorBuffer->getHeight() == 1);

  framebuffer.removeTextureBuffer(depthBuffer);
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  REQUIRE(framebuffer.getColorBufferCount() == 1);
  CHECK(framebuffer.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  framebuffer.removeTextureBuffer(colorBuffer);
  CHECK(framebuffer.isEmpty());

  // Re-adding both buffers to check clear operations
  framebuffer.setDepthBuffer(depthBuffer);
  framebuffer.addColorBuffer(colorBuffer, 0);
  CHECK(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 1);

  framebuffer.clearTextureBuffers();
  CHECK(framebuffer.isEmpty()); // Both depth & color buffers have been removed
}

TEST_CASE("Framebuffer move", "[render]") {
  Raz::Framebuffer framebuffer;

  const unsigned int framebufferIndex = framebuffer.getIndex();
  REQUIRE(framebufferIndex != 0);

  const auto depthBuffer = Raz::Texture2D::create(Raz::TextureColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture2D::create(Raz::TextureColorspace::RGB);

  framebuffer.setDepthBuffer(depthBuffer);
  framebuffer.addColorBuffer(colorBuffer, 0);

  // Move ctor

  Raz::Framebuffer movedFramebufferCtor(std::move(framebuffer));

  // The new framebuffer has the same values as the original one
  CHECK(movedFramebufferCtor.getIndex() == framebufferIndex);
  REQUIRE(movedFramebufferCtor.hasDepthBuffer());
  CHECK(movedFramebufferCtor.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  REQUIRE(movedFramebufferCtor.getColorBufferCount() == 1);
  CHECK(movedFramebufferCtor.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  // The moved texture is now invalid
  CHECK(framebuffer.getIndex() == 0);
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 0);

  // Move assignment operator

  Raz::Framebuffer movedFramebufferOp;

  const unsigned int movedFramebufferOpIndex = movedFramebufferOp.getIndex();
  REQUIRE(movedFramebufferOpIndex != 0);

  movedFramebufferOp = std::move(movedFramebufferCtor);

  // The new framebuffer has the same values as the previous one
  CHECK(movedFramebufferOp.getIndex() == framebufferIndex);
  REQUIRE(movedFramebufferOp.hasDepthBuffer());
  CHECK(movedFramebufferOp.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  REQUIRE(movedFramebufferOp.getColorBufferCount() == 1);
  CHECK(movedFramebufferOp.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  // After being moved, the values are swapped: the moved-from framebuffer now has the previous moved-to's values
  CHECK(movedFramebufferCtor.getIndex() == movedFramebufferOpIndex);
  CHECK_FALSE(movedFramebufferCtor.hasDepthBuffer());
  CHECK(movedFramebufferCtor.getColorBufferCount() == 0);
}
