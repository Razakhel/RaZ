#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

Cubemap::Cubemap() {
  Renderer::generateTexture(m_index);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  const std::string vertSource = R"(
    #version 330 core

    layout(location = 0) in vec3 vertPosition;

    layout(std140) uniform uboCubemapMatrix {
      mat4 viewProjMat;
    };

    out vec3 fragTexcoords;

    void main() {
      fragTexcoords = vertPosition;

      vec4 pos = viewProjMat * vec4(vertPosition, 1.0);
      gl_Position = pos.xyww;
    }
  )";

  const std::string fragSource = R"(
    #version 330 core

    in vec3 fragTexcoords;

    uniform samplerCube uniSkybox;

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = texture(uniSkybox, fragTexcoords);
    }
  )";
#else
  // The version must be on first line
  const std::string vertSource = R"(#version 300 es

    precision highp float;
    precision highp int;

    layout(location = 0) in vec3 vertPosition;

    layout(std140) uniform uboCubemapMatrix {
      mat4 viewProjMat;
    };

    out vec3 fragTexcoords;

    void main() {
      fragTexcoords = vertPosition;

      vec4 pos = viewProjMat * vec4(vertPosition, 1.0);
      gl_Position = pos.xyww;
    }
  )";

  const std::string fragSource = R"(#version 300 es

    precision highp float;
    precision highp int;

    in vec3 fragTexcoords;

    uniform samplerCube uniSkybox;

    layout(location = 0) out vec4 fragColor;

    void main() {
      fragColor = texture(uniSkybox, fragTexcoords);
    }
  )";
#endif

  m_program.setVertexShader(VertexShader::loadFromSource(vertSource));
  m_program.setFragmentShader(FragmentShader::loadFromSource(fragSource));
  m_program.compileShaders();
  m_program.link();

  m_program.use();
  m_program.sendUniform("uniSkybox", 0);

  m_viewProjUbo.bindUniformBlock(m_program, "uboCubemapMatrix", 1);
  m_viewProjUbo.bindBufferBase(1);
}

Cubemap::Cubemap(Cubemap&& cubemap) noexcept
  : m_index{ std::exchange(cubemap.m_index, std::numeric_limits<unsigned int>::max()) },
    m_program{ std::move(cubemap.m_program) },
    m_viewProjUbo{ std::move(cubemap.m_viewProjUbo) } {}

void Cubemap::load(const FilePath& rightTexturePath, const FilePath& leftTexturePath,
                   const FilePath& topTexturePath, const FilePath& bottomTexturePath,
                   const FilePath& frontTexturePath, const FilePath& backTexturePath) const {
  bind();

  Image img(rightTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_POS_X,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  img.read(leftTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_NEG_X,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  img.read(topTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_POS_Y,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  img.read(bottomTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_NEG_Y,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  img.read(frontTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_POS_Z,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  img.read(backTexturePath);
  Renderer::sendImageData2D(TextureType::CUBEMAP_NEG_Z,
                            0,
                            static_cast<TextureInternalFormat>(img.getColorspace()),
                            img.getWidth(),
                            img.getHeight(),
                            static_cast<TextureFormat>(img.getColorspace()),
                            TextureDataType::UBYTE,
                            img.getDataPtr());

  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_S, TextureParamValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_T, TextureParamValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_R, TextureParamValue::CLAMP_TO_EDGE);

  unbind();
}

void Cubemap::bind() const {
  Renderer::bindTexture(TextureType::CUBEMAP, m_index);
}

void Cubemap::unbind() const {
  Renderer::unbindTexture(TextureType::CUBEMAP);
}

void Cubemap::draw(const Camera& camera) const {
  Renderer::setDepthFunction(DepthFunction::LESS_EQUAL);
  Renderer::setFaceCulling(FaceOrientation::FRONT);

  m_program.use();

  Renderer::activateTexture(0);
  bind();

  m_viewProjUbo.bind();
  sendViewProjectionMatrix(Mat4f(Mat3f(camera.getViewMatrix())) * camera.getProjectionMatrix());

  Mesh::drawUnitCube();

  Renderer::setFaceCulling(FaceOrientation::BACK);
  Renderer::setDepthFunction(DepthFunction::LESS);
}

Cubemap& Cubemap::operator=(Cubemap&& cubemap) noexcept {
  std::swap(m_index, cubemap.m_index);
  m_program = std::move(cubemap.m_program);
  m_viewProjUbo = std::move(cubemap.m_viewProjUbo);

  return *this;
}

Cubemap::~Cubemap() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Renderer::deleteTexture(m_index);
}

} // namespace Raz
