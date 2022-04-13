#include "Catch.hpp"

#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("RenderPass validity") {
  Raz::RenderPass initialPass;
  CHECK(initialPass.isValid()); // The pass has no buffer, thus is valid

  const auto depthTexture = Raz::Texture::create(1, 1, Raz::ImageColorspace::DEPTH);
  const auto colorTexture = Raz::Texture::create(1, 1, Raz::ImageColorspace::RGB);

  initialPass.addWriteTexture(depthTexture);
  CHECK(initialPass.isValid()); // Direct buffer dependency is not checked; even though no child exists, it is not required to have matching buffers

  Raz::RenderPass nextPass;

  // Adding the second pass as a child of the first one
  initialPass.addChildren(nextPass);
  CHECK(initialPass.isValid()); // A subsequent pass exists, but doesn't change the previous result
  CHECK(nextPass.isValid());

  // Adding a read color buffer to the second pass
  nextPass.addReadTexture(colorTexture, ""); // An uniform name is unnecessary here
  CHECK(initialPass.isValid()); // Matching buffers being unrequired, the first pass is still valid
  CHECK(nextPass.isValid());

  // Adding the depth buffer as a read one in the first pass
  initialPass.addReadTexture(depthTexture, "");
  CHECK_FALSE(initialPass.isValid()); // The depth buffer is defined as both read & write in the first pass
  CHECK(nextPass.isValid());

  // Adding the color buffer as a write one in the second pass
  nextPass.addWriteTexture(colorTexture);
  CHECK_FALSE(initialPass.isValid());
  CHECK_FALSE(nextPass.isValid()); // The color buffer is defined as both read & write in the second pass
}
