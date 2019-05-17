#include "catch/catch.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/Window.hpp"

TEST_CASE("Vertex shader basic") {
// Window created to setup the OpenGL context, which Raz::Shader needs to be instantiated
// TODO: this window should not be created anymore when renderer will be externalized
  const Raz::Window window(1, 1);

  const std::string vertSource = R"(
    #version 330 core

    void main() {
      gl_Position = vec4(1.0);
    }
  )";

  const Raz::VertexShader vertShader = Raz::VertexShader::loadFromSource(vertSource);

  vertShader.compile();
  REQUIRE(vertShader.isCompiled());
}

TEST_CASE("Fragment shader basic") {
// Window created to setup the OpenGL context, which Raz::Shader needs to be instantiated
// TODO: this window should not be created anymore when renderer will be externalized
  const Raz::Window window(1, 1);

  const std::string fragSource = R"(
    #version 330 core

    layout (location = 0) out vec4 fragColor;

    void main() {
      fragColor = vec4(1.0);
    }
  )";

  const Raz::FragmentShader fragShader = Raz::FragmentShader::loadFromSource(fragSource);

  fragShader.compile();
  REQUIRE(fragShader.isCompiled());
}

TEST_CASE("Vertex shader imported") {
// Window created to setup the OpenGL context, which Raz::Shader needs to be instantiated
// TODO: this window should not be created anymore when renderer will be externalized
  const Raz::Window window(1, 1);

  const Raz::VertexShader vertShader("../../shaders/vert.glsl");

  vertShader.compile();
  REQUIRE(vertShader.isCompiled());
}

TEST_CASE("Fragment shader imported") {
// Window created to setup the OpenGL context, which Raz::Shader needs to be instantiated
// TODO: this window should not be created anymore when renderer will be externalized
  const Raz::Window window(1, 1);

  const Raz::FragmentShader fragShader("../../shaders/lambert.glsl");

  fragShader.compile();
  REQUIRE(fragShader.isCompiled());
}
