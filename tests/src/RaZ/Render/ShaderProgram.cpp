#include "Catch.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace {

constexpr std::string_view vertSource = R"(
  uniform int uniUnused;

  struct Test {
    mat3 uniMat3x3;
    samplerCube uniSamplerCube;
  };

  uniform Test uniStruct;

  void main() {
    vec3 val = texture(uniStruct.uniSamplerCube, vec3(0.0)).rgb;
    gl_Position = vec4(val * uniStruct.uniMat3x3, 1.0);
  }
)";

constexpr std::string_view tessCtrlSource = R"(
  uniform int uniInt[2];
  uniform bool uniBool;

  layout(vertices = 4) out;

  void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = float(uniBool);
    gl_TessLevelOuter[1] = float(uniBool);
    gl_TessLevelOuter[2] = float(uniBool);
    gl_TessLevelOuter[3] = float(uniBool);

    gl_TessLevelInner[0] = float(uniInt[0]);
    gl_TessLevelInner[1] = float(uniInt[1]);
  }
)";

constexpr std::string_view tessEvalSource = R"(
  layout(quads, equal_spacing, ccw) in;

  uniform uint uniUint[3];
  uniform float uniFloat;

  void main() {
    gl_Position = vec4(float(uniUint[0]), float(uniUint[1]), float(uniUint[2]), uniFloat);
  }
)";

constexpr std::string_view fragSource = R"(
  uniform vec3 uniVec3[2];
  uniform sampler2D uniSampler2D;

  layout(location = 0) out vec4 fragColor;

  void main() {
    fragColor = vec4(uniVec3[0] + uniVec3[1], 1.0) * texture(uniSampler2D, vec2(0.0, 0.0));
  }
)";

constexpr std::string_view compSource = R"(
  layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

  uniform int uniUnused;
  uniform vec3 uniVec3;
  uniform float uniFloat[3];
  uniform int uniInt[2];
  uniform uint uniUint;

  layout(rgba32f, binding = 0) uniform image2D uniImage2D;

  void main() {
    imageStore(uniImage2D, ivec2(uniInt[0] + uniInt[1], int(uniUint)), vec4(uniVec3, uniFloat[0] + uniFloat[1] + uniFloat[2]));
  }
)";

struct UniformInfo {
  Raz::UniformType uniType {};
  int uniSize {};
};

void checkUniformInfo(const std::unordered_map<std::string, UniformInfo>& correspUniInfo, const Raz::ShaderProgram& program) {
  CHECK(Raz::Renderer::recoverActiveUniformCount(program.getIndex()) == correspUniInfo.size());
  CHECK_FALSE(Raz::Renderer::hasErrors());

  for (unsigned int uniIndex = 0; uniIndex < correspUniInfo.size(); ++uniIndex) {
    UniformInfo uniInfo;
    std::string uniName;
    Raz::Renderer::recoverUniformInfo(program.getIndex(), uniIndex, uniInfo.uniType, uniName, &uniInfo.uniSize);
    CHECK_FALSE(Raz::Renderer::hasErrors());

    const auto iter = correspUniInfo.find(uniName);

    REQUIRE(iter != correspUniInfo.cend());
    CHECK(uniInfo.uniType == iter->second.uniType);
    CHECK(uniInfo.uniSize == iter->second.uniSize);
  }
}

} // namespace

TEST_CASE("ShaderProgram move") {
  {
    // Tessellation shaders are only available in OpenGL 4.0+
    const bool isTessellationSupported = Raz::Renderer::checkVersion(4, 0);

    Raz::RenderShaderProgram programBase;

    programBase.setShaders(Raz::VertexShader(), Raz::GeometryShader(), Raz::FragmentShader());
    if (isTessellationSupported) {
      programBase.setTessellationControlShader(Raz::TessellationControlShader());
      programBase.setTessellationEvaluationShader(Raz::TessellationEvaluationShader());
    }

    programBase.updateShaders();

    const unsigned int programIndex  = programBase.getIndex();
    const unsigned int vertIndex     = programBase.getVertexShader().getIndex();
    const unsigned int tessCtrlIndex = (isTessellationSupported ? programBase.getTessellationControlShader().getIndex() : 0);
    const unsigned int tessEvalIndex = (isTessellationSupported ? programBase.getTessellationEvaluationShader().getIndex() : 0);
    const unsigned int geomIndex     = programBase.getGeometryShader().getIndex();
    const unsigned int fragIndex     = programBase.getFragmentShader().getIndex();

    // Constructor
    Raz::RenderShaderProgram programMoved = std::move(programBase);
    CHECK(programMoved.getIndex() == programIndex);
    CHECK(programMoved.getVertexShader().getIndex() == vertIndex);
    CHECK(programMoved.getGeometryShader().getIndex() == geomIndex);
    CHECK(programMoved.getFragmentShader().getIndex() == fragIndex);

    if (isTessellationSupported) {
      CHECK(programMoved.getTessellationControlShader().getIndex() == tessCtrlIndex);
      CHECK(programMoved.getTessellationEvaluationShader().getIndex() == tessEvalIndex);
    }

    // Assignment operator
    programBase = std::move(programMoved);
    CHECK(programBase.getIndex() == programIndex);
    CHECK(programBase.getVertexShader().getIndex() == vertIndex);
    CHECK(programBase.getGeometryShader().getIndex() == geomIndex);
    CHECK(programBase.getFragmentShader().getIndex() == fragIndex);

    if (isTessellationSupported) {
      CHECK(programBase.getTessellationControlShader().getIndex() == tessCtrlIndex);
      CHECK(programBase.getTessellationEvaluationShader().getIndex() == tessEvalIndex);
    }
  }

  // Compute shaders are only available in OpenGL 4.3+
  if (Raz::Renderer::checkVersion(4, 3)) {
    Raz::ComputeShaderProgram programBase;
    programBase.setShader(Raz::ComputeShader());
    programBase.updateShaders();

    const unsigned int programIndex = programBase.getIndex();
    const unsigned int compIndex    = programBase.getShader().getIndex();

    // Constructor
    Raz::ComputeShaderProgram programMoved = std::move(programBase);
    CHECK(programMoved.getIndex() == programIndex);
    CHECK(programMoved.getShader().getIndex() == compIndex);

    // Assignment operator
    programBase = std::move(programMoved);
    CHECK(programBase.getIndex() == programIndex);
    CHECK(programBase.getShader().getIndex() == compIndex);
  }
}

TEST_CASE("RenderShaderProgram creation") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  {
    Raz::RenderShaderProgram program;

    // Setting the shaders one by one doesn't automatically link the program (which needs them all)
    program.setVertexShader(Raz::VertexShader::loadFromSource(vertSource));
    program.setFragmentShader(Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());

    CHECK_FALSE(program.isLinked());
    CHECK_FALSE(program.isUsed());

    program.link();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isLinked());

    program.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isUsed());
  }

  {
    Raz::RenderShaderProgram program;

    CHECK_FALSE(program.isLinked());
    CHECK_FALSE(program.isUsed());

    // Setting all of them at once effectively links the program
    program.setShaders(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isLinked());

    // But does not declare it as used
    CHECK_FALSE(program.isUsed());

    program.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isUsed());
  }

  {
    // Constructors are available to automatically do the same
    Raz::RenderShaderProgram program(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());

    CHECK(program.isLinked());
    CHECK_FALSE(program.isUsed());

    program.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isUsed());

    // When destroying only the vertex shader...
    program.destroyVertexShader();
    CHECK_FALSE(Raz::Renderer::hasErrors());

    // ... the program can still be linked
    program.link();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isLinked());

    // When destroying both shaders...
    program.destroyFragmentShader();
    CHECK_FALSE(Raz::Renderer::hasErrors());

    // The program can't be linked anymore
    program.link();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(program.isLinked());

    // Nor can it be used
    program.use();
    CHECK(Raz::Renderer::hasErrors());
  }
}

TEST_CASE("RenderShaderProgram uniforms") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  // Tessellation shaders are only available in OpenGL 4.0+
  const bool isTessellationSupported = Raz::Renderer::checkVersion(4, 0);

  Raz::RenderShaderProgram program(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));
  if (isTessellationSupported) {
    program.setTessellationControlShader(Raz::TessellationControlShader::loadFromSource(tessCtrlSource));
    program.setTessellationEvaluationShader(Raz::TessellationEvaluationShader::loadFromSource(tessEvalSource));
    program.updateShaders();
  }

  REQUIRE(program.isLinked());

  CHECK(program.recoverUniformLocation("uniStruct.uniMat3x3") != -1);
  CHECK(program.recoverUniformLocation("uniStruct.uniSamplerCube") != -1);
  CHECK(program.recoverUniformLocation("uniVec3") != -1);
  CHECK(program.recoverUniformLocation("uniSampler2D") != -1);

  if (isTessellationSupported) {
    CHECK(program.recoverUniformLocation("uniInt") != -1);
    CHECK(program.recoverUniformLocation("uniBool") != -1);
    CHECK(program.recoverUniformLocation("uniUint") != -1);
    CHECK(program.recoverUniformLocation("uniFloat") != -1);
  }

  // Any unused uniform will be optimized out, hence won't have a location
  CHECK(program.recoverUniformLocation("uniUnused") == -1);

  std::unordered_map<std::string, UniformInfo> correspUniInfo = {
    { "uniStruct.uniMat3x3", { Raz::UniformType::MAT3, 1 } },
    { "uniStruct.uniSamplerCube", { Raz::UniformType::SAMPLER_CUBE, 1 } },
    { "uniVec3[0]", { Raz::UniformType::VEC3, 2 } },
    { "uniSampler2D", { Raz::UniformType::SAMPLER_2D, 1 } }
  };

  if (isTessellationSupported) {
    correspUniInfo.emplace("uniInt[0]", UniformInfo{ Raz::UniformType::INT, 2 });
    correspUniInfo.emplace("uniBool", UniformInfo{ Raz::UniformType::BOOL, 1 });
    correspUniInfo.emplace("uniUint[0]", UniformInfo{ Raz::UniformType::UINT, 3 });
    correspUniInfo.emplace("uniFloat", UniformInfo{ Raz::UniformType::FLOAT, 1 });
  }

  checkUniformInfo(correspUniInfo, program);
}

TEST_CASE("ComputeShaderProgram creation") {
  // Compute shaders are only available in OpenGL 4.3+
  if (!Raz::Renderer::checkVersion(4, 3))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  {
    Raz::ComputeShaderProgram program;

    CHECK_FALSE(program.isLinked());
    CHECK_FALSE(program.isUsed());

    // Setting the shader on its own automatically links the program
    program.setShader(Raz::ComputeShader::loadFromSource(compSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isLinked());
    CHECK_FALSE(program.isUsed());

    program.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isUsed());
  }

  {
    // A constructor is available to automatically do the same
    Raz::ComputeShaderProgram program(Raz::ComputeShader::loadFromSource(compSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isLinked());
    CHECK_FALSE(program.isUsed());

    program.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(program.isUsed());
  }
}

TEST_CASE("ComputeShaderProgram uniforms") {
  // Compute shaders are only available in OpenGL 4.3+
  if (!Raz::Renderer::checkVersion(4, 3))
    return;

  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::ComputeShaderProgram program(Raz::ComputeShader::loadFromSource(compSource));

  REQUIRE(program.isLinked());

  CHECK(program.recoverUniformLocation("uniVec3") != -1);
  CHECK(program.recoverUniformLocation("uniFloat") != -1);
  CHECK(program.recoverUniformLocation("uniInt") != -1);
  CHECK(program.recoverUniformLocation("uniUint") != -1);
  CHECK(program.recoverUniformLocation("uniImage2D") != -1);

  // Any unused uniform will be optimized out, hence won't have a location
  CHECK(program.recoverUniformLocation("uniUnused") == -1);

  const std::unordered_map<std::string, UniformInfo> correspUniInfo = {
    { "uniVec3", { Raz::UniformType::VEC3, 1 } },
    { "uniFloat[0]", { Raz::UniformType::FLOAT, 3 } },
    { "uniInt[0]", { Raz::UniformType::INT, 2 } },
    { "uniUint", { Raz::UniformType::UINT, 1 } },
#if !defined(USE_OPENGL_ES)
    { "uniImage2D", { Raz::UniformType::IMAGE_2D, 1 } } // UniformType::IMAGE_2D is not available with OpenGL ES
#endif
  };

  checkUniformInfo(correspUniInfo, program);
}