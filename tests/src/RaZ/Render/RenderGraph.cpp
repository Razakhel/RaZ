#include "Catch.hpp"

#include "RaZ/Render/RenderGraph.hpp"

TEST_CASE("RenderGraph validity") {
  Raz::RenderGraph graph;
  CHECK(graph.isValid());

  const auto depthTexture = Raz::Texture::create(1, 1, Raz::ImageColorspace::DEPTH);
  CHECK(graph.isValid()); // Adding a buffer does not change the graph's validity

  Raz::RenderPass& firstPass = graph.addNode();
  CHECK(graph.isValid()); // An empty pass is always valid

  firstPass.addWriteTexture(depthTexture);
  CHECK(graph.isValid()); // A pass does not need matching buffers with its children

  Raz::RenderPass& secondPass = graph.addNode();
  firstPass.addChildren(secondPass);
  CHECK(graph.isValid()); // Since matching buffers between passes is not required, the graph remains valid

  secondPass.addReadTexture(depthTexture, ""); // Unnecessary uniform name
  CHECK(graph.isValid()); // Same as the above

  firstPass.addReadTexture(depthTexture, "");
  CHECK_FALSE(graph.isValid()); // The depth texture is set as both read & write usages in the geometry buffer; at least one pass is invalid, thus the graph is
}
