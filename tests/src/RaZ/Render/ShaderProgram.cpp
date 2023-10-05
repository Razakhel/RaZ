#include "Catch.hpp"

#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace {

class TestShaderProgram : public Raz::ShaderProgram {
public:
  void loadShaders() const override {}
  void compileShaders() const override {}
};

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
  #extension GL_ARB_tessellation_shader : enable

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
  #extension GL_ARB_tessellation_shader : enable

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
  #extension GL_ARB_compute_shader : enable

  layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

  uniform int uniUnused;
  uniform vec3 uniVec3;
  uniform float uniFloat[3];
  uniform int uniInt[2];
  uniform uint uniUint;

  layout(rgba32f, binding = 0) uniform writeonly restrict image2D uniImage2D;

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
    Raz::RenderShaderProgram programBase;

    programBase.setShaders(Raz::VertexShader(), Raz::FragmentShader());

#if !defined(USE_OPENGL_ES)
    programBase.setGeometryShader(Raz::GeometryShader());

    // Tessellation shaders are only available in OpenGL 4.0+ or with the 'GL_ARB_tessellation_shader' extension
    const bool isTessellationSupported = Raz::Renderer::checkVersion(4, 0) || Raz::Renderer::isExtensionSupported("GL_ARB_tessellation_shader");

    if (isTessellationSupported) {
      programBase.setTessellationControlShader(Raz::TessellationControlShader());
      programBase.setTessellationEvaluationShader(Raz::TessellationEvaluationShader());
    }
#endif

    programBase.updateShaders();

    const unsigned int programIndex  = programBase.getIndex();
    const unsigned int vertIndex     = programBase.getVertexShader().getIndex();
#if !defined(USE_OPENGL_ES)
    const unsigned int tessCtrlIndex = (isTessellationSupported ? programBase.getTessellationControlShader().getIndex() : 0);
    const unsigned int tessEvalIndex = (isTessellationSupported ? programBase.getTessellationEvaluationShader().getIndex() : 0);
    const unsigned int geomIndex     = programBase.getGeometryShader().getIndex();
#endif
    const unsigned int fragIndex     = programBase.getFragmentShader().getIndex();

    // Constructor
    Raz::RenderShaderProgram programMoved = std::move(programBase);
    CHECK(programMoved.getIndex() == programIndex);
    CHECK(programMoved.getVertexShader().getIndex() == vertIndex);
    CHECK(programMoved.getFragmentShader().getIndex() == fragIndex);

#if !defined(USE_OPENGL_ES)
    CHECK(programMoved.getGeometryShader().getIndex() == geomIndex);

    if (isTessellationSupported) {
      CHECK(programMoved.getTessellationControlShader().getIndex() == tessCtrlIndex);
      CHECK(programMoved.getTessellationEvaluationShader().getIndex() == tessEvalIndex);
    }
#endif

    // Assignment operator
    programBase = std::move(programMoved);
    CHECK(programBase.getIndex() == programIndex);
    CHECK(programBase.getVertexShader().getIndex() == vertIndex);
    CHECK(programBase.getFragmentShader().getIndex() == fragIndex);

#if !defined(USE_OPENGL_ES)
    CHECK(programBase.getGeometryShader().getIndex() == geomIndex);

    if (isTessellationSupported) {
      CHECK(programBase.getTessellationControlShader().getIndex() == tessCtrlIndex);
      CHECK(programBase.getTessellationEvaluationShader().getIndex() == tessEvalIndex);
    }
#endif
  }

#if !defined(USE_WEBGL)
  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (Raz::Renderer::checkVersion(4, 3) || Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
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
#endif
}

TEST_CASE("ShaderProgram attributes") {
  TestShaderProgram program;

  CHECK(program.getAttributeCount() == 0);

  program.setAttribute(3, "attrib1");
  CHECK(program.getAttributeCount() == 1);
  REQUIRE(program.hasAttribute("attrib1")); // An attribute can be checked with its name only...
  REQUIRE(program.hasAttribute<int>("attrib1")); // ... or including its type
  CHECK_FALSE(program.hasAttribute<float>("attrib1")); // The given type must be the same as the one held for this check to be true
  CHECK(program.getAttribute<int>("attrib1") == 3);

  program.setAttribute(6.f, "attrib2");
  CHECK(program.getAttributeCount() == 2);
  REQUIRE(program.hasAttribute("attrib2"));
  CHECK(program.getAttribute<float>("attrib2") == 6.f);

  program.setAttribute(42, "attrib1");
  CHECK(program.getAttributeCount() == 2); // The attribute already exists, none has been added
  REQUIRE(program.hasAttribute("attrib1"));
  CHECK(program.getAttribute<int>("attrib1") == 42); // But its value has been reassigned

  program.removeAttribute("attrib1");
  CHECK(program.getAttributeCount() == 1);
  CHECK_FALSE(program.hasAttribute("attrib1"));

  program.clearAttributes();
  CHECK(program.getAttributeCount() == 0);
  CHECK_FALSE(program.hasAttribute("attrib2"));
}

TEST_CASE("ShaderProgram textures") {
  TestShaderProgram program;

  CHECK(program.getTextureCount() == 0);

  const auto tex2D = Raz::Texture2D::create();
  const auto tex3D = Raz::Texture3D::create();

  program.setTexture(tex2D, "tex2D");
  CHECK(program.getTextureCount() == 1);
  REQUIRE(program.hasTexture("tex2D")); // A texture's existence can be checked with either its uniform name...
  CHECK(program.hasTexture(*tex2D)); // ... or the texture itself

  CHECK(program.getTexture("tex2D").getIndex() == tex2D->getIndex()); // Textures can be recovered with either their uniform name...
  CHECK(program.getTexture(0).getIndex() == tex2D->getIndex()); // ... or their index in the list

  // The same texture can be paired with different uniform names
  program.setTexture(tex2D, "tex2D2");
  CHECK(program.getTextureCount() == 2);
  REQUIRE(program.hasTexture("tex2D"));
  REQUIRE(program.hasTexture("tex2D2"));
  CHECK(program.hasTexture(*tex2D));
  // Getting a texture with either of the uniform names returns the same
  CHECK(program.getTexture("tex2D").getIndex() == program.getTexture("tex2D2").getIndex());

  program.setTexture(tex3D, "tex3D");
  CHECK(program.getTextureCount() == 3);
  REQUIRE(program.hasTexture("tex3D"));
  CHECK(program.hasTexture(program.getTexture("tex3D")));
  CHECK(program.getTexture(2).getIndex() == program.getTexture("tex3D").getIndex());

  program.setTexture(tex3D, "tex3D2");
  CHECK(program.getTextureCount() == 4);
  REQUIRE(program.hasTexture("tex3D2"));
  CHECK(program.getTexture(2).getIndex() == program.getTexture(3).getIndex());

  // Removing a specific texture removes all entries corresponding to it
  program.removeTexture(*tex3D);
  CHECK(program.getTextureCount() == 2);
  CHECK_FALSE(program.hasTexture("tex3D"));
  CHECK_FALSE(program.hasTexture("tex3D2"));

  program.setTexture(Raz::Texture2D::create(), "tex2D");
  CHECK(program.getTextureCount() == 2); // The texture already exists, none has been added
  REQUIRE(program.hasTexture("tex2D"));
  CHECK_FALSE(program.getTexture("tex2D").getIndex() == tex2D->getIndex()); // But its value has been reassigned

  program.removeTexture("tex2D");
  CHECK(program.getTextureCount() == 1);
  CHECK_FALSE(program.hasTexture("tex2D"));
  CHECK(program.hasTexture("tex2D2")); // The other entry remains

  program.clearTextures();
  CHECK(program.getTextureCount() == 0);
  CHECK_FALSE(program.hasTexture("tex2D2"));
}

#if !defined(USE_WEBGL)
TEST_CASE("ShaderProgram image textures") {
  TestShaderProgram program;

  CHECK(program.getImageTextureCount() == 0);

  // Adding an image texture without a valid colorspace throws an exception
  CHECK_THROWS(program.setImageTexture(Raz::Texture2D::create(), "noColorspace"));

  const auto tex2D = Raz::Texture2D::create(Raz::TextureColorspace::RGB, Raz::TextureDataType::BYTE);
  const auto tex3D = Raz::Texture3D::create(Raz::TextureColorspace::GRAY, Raz::TextureDataType::FLOAT16);

  program.setImageTexture(tex2D, "tex2D", Raz::ImageTextureUsage::READ);
  CHECK(program.getImageTextureCount() == 1);
  REQUIRE(program.hasImageTexture("tex2D")); // An image texture's existence can be checked with either its uniform name...
  CHECK(program.hasImageTexture(*tex2D)); // ... or the texture itself

  CHECK(program.getImageTexture("tex2D").getIndex() == tex2D->getIndex()); // Image textures can be recovered with either their uniform name...
  CHECK(program.getImageTexture(0).getIndex() == tex2D->getIndex()); // ... or their index in the list

  // The same image texture can be paired with different uniform names
  program.setImageTexture(tex2D, "tex2D2", Raz::ImageTextureUsage::WRITE);
  CHECK(program.getImageTextureCount() == 2);
  REQUIRE(program.hasImageTexture("tex2D"));
  REQUIRE(program.hasImageTexture("tex2D2"));
  CHECK(program.hasImageTexture(*tex2D));
  // Getting an image texture with either of the uniform names returns the same
  CHECK(program.getImageTexture("tex2D").getIndex() == program.getImageTexture("tex2D2").getIndex());

  program.setImageTexture(tex3D, "tex3D", Raz::ImageTextureUsage::READ_WRITE);
  CHECK(program.getImageTextureCount() == 3);
  REQUIRE(program.hasImageTexture("tex3D"));
  CHECK(program.hasImageTexture(program.getImageTexture("tex3D")));
  CHECK(program.getImageTexture(2).getIndex() == program.getImageTexture("tex3D").getIndex());

  program.setImageTexture(tex3D, "tex3D2");
  CHECK(program.getImageTextureCount() == 4);
  REQUIRE(program.hasImageTexture("tex3D2"));
  CHECK(program.getImageTexture(2).getIndex() == program.getImageTexture(3).getIndex());

  // Removing a specific image texture removes all entries corresponding to it
  program.removeImageTexture(*tex3D);
  CHECK(program.getImageTextureCount() == 2);
  CHECK_FALSE(program.hasImageTexture("tex3D"));
  CHECK_FALSE(program.hasImageTexture("tex3D2"));

  program.setImageTexture(Raz::Texture2D::create(Raz::TextureColorspace::RG), "tex2D");
  CHECK(program.getImageTextureCount() == 2); // The image texture already exists, none has been added
  REQUIRE(program.hasImageTexture("tex2D"));
  CHECK_FALSE(program.getImageTexture("tex2D").getIndex() == tex2D->getIndex()); // But its value has been reassigned

  program.removeImageTexture("tex2D");
  CHECK(program.getImageTextureCount() == 1);
  CHECK_FALSE(program.hasImageTexture("tex2D"));
  CHECK(program.hasImageTexture("tex2D2")); // The other entry remains

  program.clearImageTextures();
  CHECK(program.getImageTextureCount() == 0);
  CHECK_FALSE(program.hasImageTexture("tex2D2"));
}
#endif

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
  }
}

TEST_CASE("RenderShaderProgram uniforms") {
  Raz::Renderer::recoverErrors(); // Flushing errors

  Raz::RenderShaderProgram program(Raz::VertexShader::loadFromSource(vertSource), Raz::FragmentShader::loadFromSource(fragSource));

#if !defined(USE_OPENGL_ES)
  // Tessellation shaders are only available in OpenGL 4.0+ or with the 'GL_ARB_tessellation_shader' extension
  const bool isTessellationSupported = Raz::Renderer::checkVersion(4, 0) || Raz::Renderer::isExtensionSupported("GL_ARB_tessellation_shader");

  if (isTessellationSupported) {
    program.setTessellationControlShader(Raz::TessellationControlShader::loadFromSource(tessCtrlSource));
    program.setTessellationEvaluationShader(Raz::TessellationEvaluationShader::loadFromSource(tessEvalSource));
    program.updateShaders();
  }
#endif

  REQUIRE(program.isLinked());

  CHECK(program.recoverUniformLocation("uniStruct.uniMat3x3") != -1);
  CHECK(program.recoverUniformLocation("uniStruct.uniSamplerCube") != -1);
  CHECK(program.recoverUniformLocation("uniVec3") != -1);
  CHECK(program.recoverUniformLocation("uniSampler2D") != -1);

#if !defined(USE_OPENGL_ES)
  if (isTessellationSupported) {
    CHECK(program.recoverUniformLocation("uniInt") != -1);
    CHECK(program.recoverUniformLocation("uniBool") != -1);
    CHECK(program.recoverUniformLocation("uniUint") != -1);
    CHECK(program.recoverUniformLocation("uniFloat") != -1);
  }
#endif

  // Any unused uniform will be optimized out, hence won't have a location
  CHECK(program.recoverUniformLocation("uniUnused") == -1);

  std::unordered_map<std::string, UniformInfo> correspUniInfo = {
    { "uniStruct.uniMat3x3", { Raz::UniformType::MAT3, 1 } },
    { "uniStruct.uniSamplerCube", { Raz::UniformType::SAMPLER_CUBE, 1 } },
    { "uniVec3[0]", { Raz::UniformType::VEC3, 2 } },
    { "uniSampler2D", { Raz::UniformType::SAMPLER_2D, 1 } }
  };

#if !defined(USE_OPENGL_ES)
  if (isTessellationSupported) {
    correspUniInfo.emplace("uniInt[0]", UniformInfo{ Raz::UniformType::INT, 2 });
    correspUniInfo.emplace("uniBool", UniformInfo{ Raz::UniformType::BOOL, 1 });
    correspUniInfo.emplace("uniUint[0]", UniformInfo{ Raz::UniformType::UINT, 3 });
    correspUniInfo.emplace("uniFloat", UniformInfo{ Raz::UniformType::FLOAT, 1 });
  }
#endif

  checkUniformInfo(correspUniInfo, program);
}

#if !defined(USE_WEBGL)
TEST_CASE("ComputeShaderProgram creation") {
  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (!Raz::Renderer::checkVersion(4, 3) && !Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader"))
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
  // Compute shaders are only available in OpenGL 4.3+ or with the relevant extension
  if (!Raz::Renderer::checkVersion(4, 3) && !Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader"))
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
#endif
