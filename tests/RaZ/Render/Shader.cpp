#include "Catch.hpp"

#include "RaZ/Render/Shader.hpp"

TEST_CASE("Vertex shader basic") {
  const std::string vertSource = R"(
    #version 330 core

    void main() {
      gl_Position = vec4(1.0);
    }
  )";

  const Raz::VertexShader vertShader = Raz::VertexShader::loadFromSource(vertSource);

  vertShader.compile();
  CHECK(vertShader.isCompiled());
}

TEST_CASE("Fragment shader basic") {
  const std::string fragSource = R"(
    #version 330 core

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = vec4(1.0);
    }
  )";

  const Raz::FragmentShader fragShader = Raz::FragmentShader::loadFromSource(fragSource);

  fragShader.compile();
  CHECK(fragShader.isCompiled());
}

TEST_CASE("Vertex shader imported") {
  const Raz::VertexShader vertShader(RAZ_TESTS_ROOT + "../shaders/vert.glsl"s);

  vertShader.compile();
  CHECK(vertShader.isCompiled());
}

TEST_CASE("Fragment shader imported") {
  const Raz::FragmentShader fragShader(RAZ_TESTS_ROOT + "../shaders/lambert.glsl"s);

  fragShader.compile();
  CHECK(fragShader.isCompiled());
}
