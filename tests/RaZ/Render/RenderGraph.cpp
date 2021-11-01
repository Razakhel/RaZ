#include "Catch.hpp"

#include "RaZ/Render/RenderGraph.hpp"

TEST_CASE("RenderGraph validity") {
  Raz::RenderGraph graph;
  CHECK(graph.isValid());

  const Raz::Texture& depthTexture = graph.addTextureBuffer(1, 1, Raz::ImageColorspace::DEPTH);
  CHECK(graph.isValid());

  // Adding a write texture to the geometry pass without an associated read texture: it is invalid
  graph.getGeometryPass().addWriteTexture(depthTexture);
  CHECK_FALSE(graph.isValid());

  Raz::RenderPass& nextPass = graph.addNode();
  CHECK_FALSE(graph.isValid()); // The next pass doesn't have any texture, the graph is still invalid

  nextPass.addReadTexture(depthTexture, ""); // Unnecessary uniform name
  CHECK_FALSE(graph.isValid()); // The geometry & the next pass have a matching texture buffer, but they aren't linked

  graph.getGeometryPass().addChildren(nextPass);
  CHECK(graph.isValid()); // The passes are linked & their buffers match, the graph is now valid
}
