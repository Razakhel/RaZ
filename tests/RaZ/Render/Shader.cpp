#include "Catch.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"

TEST_CASE("Shader validity") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  {
    Raz::VertexShader vertShader;
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(vertShader.isValid());

    vertShader.destroy();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(vertShader.isValid());
  }

  {
    Raz::GeometryShader geomShader;
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(geomShader.isValid());

    geomShader.destroy();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(geomShader.isValid());
  }

  {
    Raz::FragmentShader fragShader;
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(fragShader.isValid());

    fragShader.destroy();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(fragShader.isValid());
  }

  {
    Raz::ComputeShader compShader;
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(compShader.isValid());

    compShader.destroy();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(compShader.isValid());
  }
}

TEST_CASE("Vertex shader from source") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string vertSource = R"(
    #version 330 core

    void main() {
      gl_Position = vec4(1.0);
    }
  )";

  const Raz::VertexShader vertShader = Raz::VertexShader::loadFromSource(vertSource);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(vertShader.isValid());

  vertShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(vertShader.isCompiled());
}

TEST_CASE("Fragment shader from source") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string fragSource = R"(
    #version 330 core

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = vec4(1.0);
    }
  )";

  const Raz::FragmentShader fragShader = Raz::FragmentShader::loadFromSource(fragSource);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(fragShader.isValid());

  fragShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(fragShader.isCompiled());
}

TEST_CASE("Compute shader from source") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string compSource = R"(
    #version 430

    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

    layout(rgba32f, binding = 0) uniform image2D uniOutput;

    void main() {
      imageStore(uniOutput, ivec2(0), vec4(0.0));
    }
  )";

  const Raz::ComputeShader compShader = Raz::ComputeShader::loadFromSource(compSource);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isValid());

  compShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isCompiled());
}

TEST_CASE("Vertex shader imported") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string vertShaderPath = RAZ_TESTS_ROOT + "../shaders/common.vert"s;

  const Raz::VertexShader vertShader(vertShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(vertShader.isValid());
  CHECK(vertShader.getPath() == vertShaderPath);

  vertShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(vertShader.isCompiled());
}

TEST_CASE("Fragment shader imported") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string fragShaderPath = RAZ_TESTS_ROOT + "../shaders/lambert.frag"s;

  const Raz::FragmentShader fragShader(fragShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(fragShader.isValid());
  CHECK(fragShader.getPath() == fragShaderPath);

  fragShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(fragShader.isCompiled());
}

TEST_CASE("Compute shader imported") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string compShaderPath = RAZ_TESTS_ROOT + "assets/shaders/test.comp"s;

  const Raz::ComputeShader compShader(compShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isValid());
  CHECK(compShader.getPath() == compShaderPath);

  compShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isCompiled());
}
