#include "Catch.hpp"

#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/SubmeshRenderer.hpp"
#include "RaZ/Utils/Shape.hpp"

TEST_CASE("SubmeshRenderer clone") {
  Raz::SubmeshRenderer submeshRenderer;
  submeshRenderer.setMaterialIndex(42);

  Raz::SubmeshRenderer clonedSubmeshRenderer = submeshRenderer.clone();
  CHECK(clonedSubmeshRenderer.getMaterialIndex() == 42);
}
