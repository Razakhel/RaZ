#include "Catch.hpp"

#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("RenderPass validity") {
  Raz::RenderPass initialPass;
  CHECK(initialPass.isValid()); // The pass has no buffer, thus is valid

  const auto depthBuffer = Raz::Texture::create(Raz::ImageColorspace::DEPTH);
  const auto colorBuffer = Raz::Texture::create(Raz::ImageColorspace::RGB);

  initialPass.addWriteTexture(depthBuffer);
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

  nextPass.addWriteTexture(colorBuffer);
  CHECK_FALSE(initialPass.isValid());
  CHECK_FALSE(nextPass.isValid()); // The color buffer is defined as both read & write in the second pass

  initialPass.removeReadTexture(depthBuffer);
  CHECK(initialPass.isValid()); // The depth buffer is only set as write, the pass is valid again
  CHECK_FALSE(nextPass.isValid());

  nextPass.removeWriteTexture(colorBuffer);
  CHECK(initialPass.isValid());
  CHECK(nextPass.isValid()); // The color buffer is only set as read, the pass is valid again
}
