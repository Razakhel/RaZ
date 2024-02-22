#include "Catch.hpp"

#include "RaZ/Render/SubmeshRenderer.hpp"

TEST_CASE("SubmeshRenderer clone", "[render]") {
  Raz::SubmeshRenderer submeshRenderer;
  submeshRenderer.setMaterialIndex(42);

  const Raz::SubmeshRenderer clonedSubmeshRenderer = submeshRenderer.clone();
  CHECK(clonedSubmeshRenderer.getMaterialIndex() == 42);
}
