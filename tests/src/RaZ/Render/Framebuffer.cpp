#include "Catch.hpp"

#include "RaZ/Render/Framebuffer.hpp"

TEST_CASE("Framebuffer buffers") {
  Raz::Framebuffer framebuffer;
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 0);
  CHECK(framebuffer.isEmpty()); // Combining the above checks

  // The buffers need to have a size for the framebuffer to be complete
  const auto depthBuffer = Raz::Texture::create(Raz::ImageColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture::create(Raz::ImageColorspace::RGB);

  framebuffer.addTextureBuffer(depthBuffer);
  REQUIRE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  CHECK(framebuffer.getColorBufferCount() == 0);

  framebuffer.addTextureBuffer(colorBuffer);
  REQUIRE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  REQUIRE(framebuffer.getColorBufferCount() == 1);
  CHECK(framebuffer.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  CHECK(depthBuffer->getImage().getWidth() == 0);
  CHECK(depthBuffer->getImage().getHeight() == 0);
  CHECK(colorBuffer->getImage().getWidth() == 0);
  CHECK(colorBuffer->getImage().getHeight() == 0);

  framebuffer.resizeBuffers(1, 1);
  CHECK(depthBuffer->getImage().getWidth() == 1); // Resizing the framebuffer resizes all contained buffers
  CHECK(depthBuffer->getImage().getHeight() == 1);
  CHECK(colorBuffer->getImage().getWidth() == 1);
  CHECK(colorBuffer->getImage().getHeight() == 1);

  framebuffer.removeTextureBuffer(depthBuffer);
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  REQUIRE(framebuffer.getColorBufferCount() == 1);
  CHECK(framebuffer.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  framebuffer.removeTextureBuffer(colorBuffer);
  CHECK(framebuffer.isEmpty());

  // Re-adding both buffers to check clear operations
  framebuffer.addTextureBuffer(depthBuffer);
  framebuffer.addTextureBuffer(colorBuffer);
  CHECK(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 1);

  framebuffer.clearTextureBuffers();
  CHECK(framebuffer.isEmpty()); // Both depth & color buffers have been removed
}

TEST_CASE("Framebuffer move") {
  Raz::Framebuffer framebuffer;

  const unsigned int framebufferIndex = framebuffer.getIndex();

  const auto depthBuffer = Raz::Texture::create(Raz::ImageColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture::create(Raz::ImageColorspace::RGB);

  framebuffer.addTextureBuffer(depthBuffer);
  framebuffer.addTextureBuffer(colorBuffer);

  // Move ctor

  Raz::Framebuffer movedFramebufferCtor(std::move(framebuffer));

  // The new framebuffer has the same values as the original one
  CHECK(movedFramebufferCtor.getIndex() == framebufferIndex);
  REQUIRE(movedFramebufferCtor.hasDepthBuffer());
  CHECK(movedFramebufferCtor.getDepthBuffer().getIndex() == depthBuffer->getIndex());
  REQUIRE(movedFramebufferCtor.getColorBufferCount() == 1);
  CHECK(movedFramebufferCtor.getColorBuffer(0).getIndex() == colorBuffer->getIndex());

  // The moved texture is now invalid
  CHECK(framebuffer.getIndex() == std::numeric_limits<unsigned int>::max());
  CHECK_FALSE(framebuffer.hasDepthBuffer());
  CHECK(framebuffer.getColorBufferCount() == 0);

  // Move assignment operator

  Raz::Framebuffer movedFramebufferOp;

  const unsigned int movedFramebufferOpIndex = movedFramebufferOp.getIndex();

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
