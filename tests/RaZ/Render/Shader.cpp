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

  // Tessellation shaders are only available in OpenGL 4.0+
  if (Raz::Renderer::checkVersion(4, 0)) {
    {
      Raz::TessellationControlShader tessCtrlShader;
      CHECK_FALSE(Raz::Renderer::hasErrors());
      CHECK(tessCtrlShader.isValid());

      tessCtrlShader.destroy();
      CHECK_FALSE(Raz::Renderer::hasErrors());
      CHECK_FALSE(tessCtrlShader.isValid());
    }

    {
      Raz::TessellationEvaluationShader tessEvalShader;
      CHECK_FALSE(Raz::Renderer::hasErrors());
      CHECK(tessEvalShader.isValid());

      tessEvalShader.destroy();
      CHECK_FALSE(Raz::Renderer::hasErrors());
      CHECK_FALSE(tessEvalShader.isValid());
    }
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

  // Compute shaders are only available in OpenGL 4.3+
  if (Raz::Renderer::checkVersion(4, 3)) {
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

TEST_CASE("Tessellation control shader from source") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessCtrlSource = R"(
    #version 400 core

    layout(vertices = 3) out;

    void main() {
      gl_TessLevelOuter[0] = 1.0;
      gl_TessLevelOuter[1] = 2.0;
      gl_TessLevelOuter[2] = 3.0;
      gl_TessLevelOuter[3] = 4.0;

      gl_TessLevelInner[0] = 1.0;
      gl_TessLevelInner[1] = 2.0;
    }
  )";

  const Raz::TessellationControlShader tessCtrlShader = Raz::TessellationControlShader::loadFromSource(tessCtrlSource);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessCtrlShader.isValid());

  tessCtrlShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessCtrlShader.isCompiled());
}

TEST_CASE("Tessellation evaluation shader from source") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessEvalSource = R"(
    #version 400 core

    layout(triangles, equal_spacing, ccw) in;

    void main() {
      gl_Position = vec4(0.0);
    }
  )";

  const Raz::TessellationEvaluationShader tessEvalShader = Raz::TessellationEvaluationShader::loadFromSource(tessEvalSource);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessEvalShader.isValid());

  tessEvalShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessEvalShader.isCompiled());
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
  // Compute shaders are only available in OpenGL 4.3+
  if (!Raz::Renderer::checkVersion(4, 3))
    return;

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

TEST_CASE("Tessellation control shader imported") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessCtrlShaderPath = RAZ_TESTS_ROOT + "assets/shaders/basic.tesc"s;

  const Raz::TessellationControlShader tessCtrlShader(tessCtrlShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessCtrlShader.isValid());
  CHECK(tessCtrlShader.getPath() == tessCtrlShaderPath);

  tessCtrlShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessCtrlShader.isCompiled());
}

TEST_CASE("Tessellation evaluation shader imported") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessEvalShaderPath = RAZ_TESTS_ROOT + "assets/shaders/basic.tese"s;

  const Raz::TessellationEvaluationShader tessEvalShader(tessEvalShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessEvalShader.isValid());
  CHECK(tessEvalShader.getPath() == tessEvalShaderPath);

  tessEvalShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(tessEvalShader.isCompiled());
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
  // Compute shaders are only available in OpenGL 4.3+
  if (!Raz::Renderer::checkVersion(4, 3))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string compShaderPath = RAZ_TESTS_ROOT + "assets/shaders/basic.comp"s;

  const Raz::ComputeShader compShader(compShaderPath);
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isValid());
  CHECK(compShader.getPath() == compShaderPath);

  compShader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(compShader.isCompiled());
}
