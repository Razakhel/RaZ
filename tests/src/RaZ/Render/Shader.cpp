#include "Catch.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"

namespace {

inline void checkShader(const Raz::Shader& shader, const Raz::FilePath& path = {}) {
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(shader.isValid());
  CHECK(shader.getPath() == path);

  shader.compile();
  CHECK_FALSE(Raz::Renderer::hasErrors());
  CHECK(shader.isCompiled());
}

} // namespace

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

  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (Raz::Renderer::checkVersion(4, 3) || Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
    Raz::ComputeShader compShader;
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(compShader.isValid());

    compShader.destroy();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(compShader.isValid());
  }
}

TEST_CASE("Shader clone") {
  // The content of the shaders does not need to be valid for this test
  const Raz::FilePath shaderPath = RAZ_TESTS_ROOT "assets/shaders/basic.comp";
  constexpr std::string_view shaderSource = "Shader source test";

  static constexpr auto areShadersEqual = [] (const Raz::Shader& shader1, const Raz::Shader& shader2) {
    CHECK_FALSE(shader1.getIndex() == shader2.getIndex()); // Both shaders remain two different objects: their indices must be different
    CHECK(shader1.getPath() == shader2.getPath());
    CHECK(Raz::Renderer::recoverShaderType(shader1.getIndex()) == Raz::Renderer::recoverShaderType(shader2.getIndex()));
    CHECK(Raz::Renderer::recoverShaderSource(shader1.getIndex()) == Raz::Renderer::recoverShaderSource(shader2.getIndex()));
  };

  {
    const Raz::VertexShader shaderFromPath(shaderPath);
    areShadersEqual(shaderFromPath, shaderFromPath.clone());

    const Raz::VertexShader shaderFromSource = Raz::VertexShader::loadFromSource(shaderSource);
    areShadersEqual(shaderFromSource, shaderFromSource.clone());
  }

  // Tessellation shaders are only available in OpenGL 4.0+
  if (Raz::Renderer::checkVersion(4, 0)) {
    {
      const Raz::TessellationControlShader shaderFromPath(shaderPath);
      areShadersEqual(shaderFromPath, shaderFromPath.clone());

      const Raz::TessellationControlShader shaderFromSource = Raz::TessellationControlShader::loadFromSource(shaderSource);
      areShadersEqual(shaderFromSource, shaderFromSource.clone());
    }

    {
      const Raz::TessellationEvaluationShader shaderFromPath(shaderPath);
      areShadersEqual(shaderFromPath, shaderFromPath.clone());

      const Raz::TessellationEvaluationShader shaderFromSource = Raz::TessellationEvaluationShader::loadFromSource(shaderSource);
      areShadersEqual(shaderFromSource, shaderFromSource.clone());
    }
  }

  {
    const Raz::GeometryShader shaderFromPath(shaderPath);
    areShadersEqual(shaderFromPath, shaderFromPath.clone());

    const Raz::GeometryShader shaderFromSource = Raz::GeometryShader::loadFromSource(shaderSource);
    areShadersEqual(shaderFromSource, shaderFromSource.clone());
  }

  {
    const Raz::FragmentShader shaderFromPath(shaderPath);
    areShadersEqual(shaderFromPath, shaderFromPath.clone());

    const Raz::FragmentShader shaderFromSource = Raz::FragmentShader::loadFromSource(shaderSource);
    areShadersEqual(shaderFromSource, shaderFromSource.clone());
  }

  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (Raz::Renderer::checkVersion(4, 3) || Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
    const Raz::ComputeShader shaderFromPath(shaderPath);
    areShadersEqual(shaderFromPath, shaderFromPath.clone());

    const Raz::ComputeShader shaderFromSource = Raz::ComputeShader::loadFromSource(shaderSource);
    areShadersEqual(shaderFromSource, shaderFromSource.clone());
  }
}

TEST_CASE("Vertex shader from source") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string vertSource = R"(
    void main() {
      gl_Position = vec4(1.0);
    }
  )";

  // The #version tag is automatically added if not present; if it is, nothing is changed to the source
  checkShader(Raz::VertexShader::loadFromSource(vertSource));
  checkShader(Raz::VertexShader::loadFromSource("#version 330\n" + vertSource));
}

TEST_CASE("Tessellation control shader from source") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessCtrlSource = R"(
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

  // The #version tag is automatically added if not present; if it is, nothing is changed to the source
  checkShader(Raz::TessellationControlShader::loadFromSource(tessCtrlSource));
  checkShader(Raz::TessellationControlShader::loadFromSource("#version 400\n" + tessCtrlSource));
}

TEST_CASE("Tessellation evaluation shader from source") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string tessEvalSource = R"(
    layout(triangles, equal_spacing, ccw) in;

    void main() {
      gl_Position = vec4(0.0);
    }
  )";

  // The #version tag is automatically added if not present; if it is, nothing is changed to the source
  checkShader(Raz::TessellationEvaluationShader::loadFromSource(tessEvalSource));
  checkShader(Raz::TessellationEvaluationShader::loadFromSource("#version 400\n" + tessEvalSource));
}

TEST_CASE("Fragment shader from source") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string fragSource = R"(
    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = vec4(1.0);
    }
  )";

  // The #version tag is automatically added if not present; if it is, nothing is changed to the source
  checkShader(Raz::FragmentShader::loadFromSource(fragSource));
  checkShader(Raz::FragmentShader::loadFromSource("#version 330\n" + fragSource));
}

TEST_CASE("Compute shader from source") {
  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (!Raz::Renderer::checkVersion(4, 3) && !Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader"))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const std::string compSource = R"(
    #extension GL_ARB_compute_shader : enable

    layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

    layout(rgba32f, binding = 0) uniform writeonly image2D uniOutput;

    void main() {
      imageStore(uniOutput, ivec2(0), vec4(0.0));
    }
  )";

  // The #version tag is automatically added if not present; if it is, nothing is changed to the source
  checkShader(Raz::ComputeShader::loadFromSource(compSource));
  checkShader(Raz::ComputeShader::loadFromSource("#version 420\n" + compSource)); // A minimum of OpenGL 4.2 is required for the extension to exist
}

TEST_CASE("Vertex shader imported") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::FilePath vertShaderPath = RAZ_TESTS_ROOT "../shaders/common.vert";

  const Raz::VertexShader vertShader(vertShaderPath);
  checkShader(vertShader, vertShaderPath);
}

TEST_CASE("Tessellation control shader imported") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::FilePath tessCtrlShaderPath = RAZ_TESTS_ROOT "assets/shaders/basic.tesc";

  const Raz::TessellationControlShader tessCtrlShader(tessCtrlShaderPath);
  checkShader(tessCtrlShader, tessCtrlShaderPath);
}

TEST_CASE("Tessellation evaluation shader imported") {
  // Tessellation shaders are only available in OpenGL 4.0+
  if (!Raz::Renderer::checkVersion(4, 0))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::FilePath tessEvalShaderPath = RAZ_TESTS_ROOT "assets/shaders/basic.tese";

  const Raz::TessellationEvaluationShader tessEvalShader(tessEvalShaderPath);
  checkShader(tessEvalShader, tessEvalShaderPath);
}

TEST_CASE("Fragment shader imported") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::FilePath fragShaderPath = RAZ_TESTS_ROOT "../shaders/lambert.frag";

  const Raz::FragmentShader fragShader(fragShaderPath);
  checkShader(fragShader, fragShaderPath);
}

TEST_CASE("Compute shader imported") {
  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (!Raz::Renderer::checkVersion(4, 3) && !Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader"))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::FilePath compShaderPath = RAZ_TESTS_ROOT "assets/shaders/basic.comp";

  const Raz::ComputeShader compShader(compShaderPath);
  checkShader(compShader, compShaderPath);
}
