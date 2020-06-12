#include "Catch.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace {

const std::string vertSource = R"(
    #version 330 core

    uniform int uniUnused;
    uniform float uniFloat;

    struct Test {
      mat3 uniMat3x3;
      uint uniUint;
      samplerCube uniSamplerCube;
    };

    uniform Test uniStruct;

    void main() {
      vec3 val = vec3(uniFloat) * texture(uniStruct.uniSamplerCube, vec3(0.0, 0.0, 0.0)).rgb;
      gl_Position = vec4(val * uniStruct.uniMat3x3, uniStruct.uniUint);
    }
)";

const std::string fragSource = R"(
    #version 330 core

    uniform int uniInt;
    uniform vec3 uniVec3;
    uniform sampler2D uniSampler2D;

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = vec4(uniVec3, uniInt) * texture(uniSampler2D, vec2(0.0, 0.0));
    }
)";

} // namespace

TEST_CASE("ShaderProgram creation") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  {
    Raz::ShaderProgram defaultProgram;

    // Setting the shaders one by one doesn't automatically link the program (which needs them all)
    defaultProgram.setVertexShader(Raz::VertexShader::loadFromSource(vertSource));
    defaultProgram.setFragmentShader(Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());

    CHECK_FALSE(defaultProgram.isLinked());
    CHECK_FALSE(defaultProgram.isUsed());

    defaultProgram.link();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(defaultProgram.isLinked());

    defaultProgram.use();
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK(defaultProgram.isUsed());
  }

  {
    Raz::ShaderProgram defaultProgram;

    CHECK_FALSE(defaultProgram.isLinked());
    CHECK_FALSE(defaultProgram.isUsed());

    // Setting all of them at once effectively links the program
    defaultProgram.setShaders(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());

    CHECK(defaultProgram.isLinked());
    CHECK(defaultProgram.isUsed());
  }

  {
    // Constructors are available to automatically do the same
    Raz::ShaderProgram program(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));
    CHECK_FALSE(Raz::Renderer::hasErrors());

    CHECK(program.isLinked());
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
    // The direct Renderer call is used instead of program.link(), since it would trigger assertions
    Raz::Renderer::linkProgram(program.getIndex());
    CHECK_FALSE(Raz::Renderer::hasErrors());
    CHECK_FALSE(program.isLinked());

    // Nor can it be used
    // Likewise, program.use() isn't called due to assertions
    Raz::Renderer::useProgram(program.getIndex());
    CHECK(Raz::Renderer::hasErrors());
  }
}

TEST_CASE("ShaderProgram uniforms") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  const Raz::ShaderProgram program(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));

  CHECK(program.recoverUniformLocation("uniFloat") != -1);
  CHECK(program.recoverUniformLocation("uniStruct.uniMat3x3") != -1);
  CHECK(program.recoverUniformLocation("uniStruct.uniUint") != -1);
  CHECK(program.recoverUniformLocation("uniStruct.uniSamplerCube") != -1);
  CHECK(program.recoverUniformLocation("uniInt") != -1);
  CHECK(program.recoverUniformLocation("uniVec3") != -1);
  CHECK(program.recoverUniformLocation("uniSampler2D") != -1);

  // Any unused uniform will be optimized out, hence won't have a location
  CHECK(program.recoverUniformLocation("uniUnused") == -1);

  CHECK(Raz::Renderer::recoverActiveUniformCount(program.getIndex()) == 7);
  CHECK_FALSE(Raz::Renderer::hasErrors());

  struct UniformInfo {
    Raz::UniformType uniType {};
    int uniSize {};
  };

  const std::unordered_map<std::string, UniformInfo> correspUniInfo = {
    { "uniFloat", { Raz::UniformType::FLOAT, 1 } },
    { "uniStruct.uniMat3x3", { Raz::UniformType::MAT3, 1 } },
    { "uniStruct.uniUint", { Raz::UniformType::UINT, 1 } },
    { "uniStruct.uniSamplerCube", { Raz::UniformType::SAMPLER_CUBE, 1 } },
    { "uniInt", { Raz::UniformType::INT, 1 } },
    { "uniVec3", { Raz::UniformType::VEC3, 1 } },
    { "uniSampler2D", { Raz::UniformType::SAMPLER_2D, 1 } }
  };

  for (unsigned int uniIndex = 0; uniIndex < 7; ++uniIndex) {
    UniformInfo uniInfo;
    std::string uniName;
    Raz::Renderer::recoverUniformInfo(program.getIndex(), uniIndex, uniInfo.uniType, uniName, &uniInfo.uniSize);
    CHECK_FALSE(Raz::Renderer::hasErrors());

    auto iter = correspUniInfo.find(uniName);

    REQUIRE(iter != correspUniInfo.cend());
    CHECK(uniInfo.uniType == iter->second.uniType);
    CHECK(uniInfo.uniSize == iter->second.uniSize);
  }
}
