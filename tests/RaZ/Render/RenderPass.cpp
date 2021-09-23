#include "Catch.hpp"

#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"

TEST_CASE("RenderPass validity") {
  Raz::RenderPass initialPass;
  CHECK(initialPass.isValid()); // The pass is independent (no subsequent pass)

  // Initial pass | Next pass
  //    depth     |

  const Raz::Texture depthTexture(1, 1, 0, Raz::ImageColorspace::DEPTH);
  initialPass.addWriteTexture(depthTexture);
  CHECK_FALSE(initialPass.isValid()); // The initial pass has a write buffer, which doesn't match any read buffer since there is no child pass

  Raz::RenderPass nextPass;
  initialPass.addChildren(nextPass);
  CHECK_FALSE(initialPass.isValid()); // A subsequent pass exists, but doesn't have a matching depth buffer
  CHECK(nextPass.isValid()); // The following pass is "independent" (the precedency isn't checked)

  // Initial pass | Next pass
  //    depth     |   color

  const Raz::Texture colorTexture(1, 1, 1, Raz::ImageColorspace::RGB);
  nextPass.addReadTexture(colorTexture, ""); // An uniform name is unnecessary here
  CHECK_FALSE(initialPass.isValid()); // The added next read texture isn't the same as the initial write one
  CHECK(nextPass.isValid());

  // Initial pass | Next pass
  //    depth     |   color
  //    color     |

  initialPass.addWriteTexture(colorTexture);
  CHECK_FALSE(initialPass.isValid()); // The color buffer exists in both passes, but the depth buffer still doesn't match
  CHECK(nextPass.isValid());

  // Initial pass | Next pass
  //    depth     |   color
  //    color     |   depth

  nextPass.addReadTexture(depthTexture, "");
  CHECK(initialPass.isValid()); // Every buffer texture has a match, the pass is finally valid
  CHECK(nextPass.isValid()); // The subsequent pass remains so

  // When a pass has a same texture in both read & write usages, the pass is invalid
  initialPass.addReadTexture(colorTexture, "");
  CHECK_FALSE(initialPass.isValid());
  CHECK(nextPass.isValid());

  nextPass.addWriteTexture(depthTexture);
  CHECK_FALSE(initialPass.isValid());
  CHECK_FALSE(nextPass.isValid()); // Since the pass has write textures but no child pass, it is considered invalid anyway

  Raz::RenderPass finalPass;
  finalPass.addReadTexture(depthTexture, "");
  nextPass.addChildren(finalPass);
  CHECK_FALSE(initialPass.isValid());
  CHECK_FALSE(nextPass.isValid()); // Even when having a subsequent pass with a matching depth buffer, it remains invalid
}
