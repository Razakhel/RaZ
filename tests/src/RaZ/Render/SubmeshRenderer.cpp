#include "RaZ/Render/SubmeshRenderer.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubmeshRenderer clone", "[render]") {
  Raz::SubmeshRenderer submeshRenderer;
  submeshRenderer.setMaterialIndex(42);

  const Raz::SubmeshRenderer clonedSubmeshRenderer = submeshRenderer.clone();
  CHECK(clonedSubmeshRenderer.getMaterialIndex() == 42);
}
