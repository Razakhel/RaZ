#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"
#include "GL/glew.h" // Needed by TracyOpenGL.hpp
#include "tracy/TracyOpenGL.hpp"

namespace Raz {

namespace {

constexpr std::string_view vertSource = R"(
  const vec3 positions[8] = vec3[](
    vec3( 1.0,  1.0, -1.0), // Right top back
    vec3( 1.0,  1.0,  1.0), // Right top front
    vec3( 1.0, -1.0, -1.0), // Right bottom back
    vec3( 1.0, -1.0,  1.0), // Right bottom front
    vec3(-1.0,  1.0, -1.0), // Left top back
    vec3(-1.0,  1.0,  1.0), // Left top front
    vec3(-1.0, -1.0, -1.0), // Left bottom back
    vec3(-1.0, -1.0,  1.0)  // Left bottom front
  );

  // Organizing the triangles to be in a clockwise order, since we will always be inside the cube
  const int indices[36] = int[](
    0, 2, 1, 1, 2, 3, // Right
    4, 5, 7, 4, 7, 6, // Left
    4, 0, 1, 4, 1, 5, // Top
    7, 3, 2, 7, 2, 6, // Bottom
    5, 1, 3, 5, 3, 7, // Front
    0, 4, 6, 0, 6, 2  // Back
  );

  layout(std140) uniform uboCameraInfo {
    mat4 uniViewMat;
    mat4 uniInvViewMat;
    mat4 uniProjectionMat;
    mat4 uniInvProjectionMat;
    mat4 uniViewProjectionMat;
    vec3 uniCameraPos;
  };

  out vec3 fragTexcoords;

  void main() {
    vec3 worldPos = positions[indices[gl_VertexID]];

    fragTexcoords = worldPos;

    vec4 projPos = uniProjectionMat * (mat4(mat3(uniViewMat)) * vec4(worldPos, 1.0));
    gl_Position  = projPos.xyww;
  }
)";

constexpr std::string_view fragSource = R"(
  in vec3 fragTexcoords;

  uniform samplerCube uniSkybox;

  layout(location = 0) out vec4 fragColor;

  void main() {
    fragColor = texture(uniSkybox, fragTexcoords);
  }
)";

TextureFormat recoverFormat(ImageColorspace colorspace) {
  switch (colorspace) {
    case ImageColorspace::GRAY:
      return TextureFormat::RED;

    case ImageColorspace::GRAY_ALPHA:
      return TextureFormat::RG;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
      return TextureFormat::RGB;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      return TextureFormat::RGBA;

    default:
      break;
  }

  throw std::invalid_argument("Error: Invalid image colorspace");
}

TextureInternalFormat recoverInternalFormat(ImageColorspace colorspace, ImageDataType dataType) {
  if (dataType == ImageDataType::BYTE) {
    // RGB(A) images are supposed to be treated as sRGB(A) textures; this will be the case in the future

    if (/*colorspace == ImageColorspace::RGB || */colorspace == ImageColorspace::SRGB)
      return TextureInternalFormat::SRGB8;

    if (/*colorspace == ImageColorspace::RGBA || */colorspace == ImageColorspace::SRGBA)
      return TextureInternalFormat::SRGBA8;

    // If the image is of a byte data type and not an sRGB colorspace, its internal format is the same as its format
    return static_cast<TextureInternalFormat>(recoverFormat(colorspace));
  }

  // Floating-point sRGB(A) images are not treated as sRGB, which is necessarily an integer format; they are therefore interpreted as floating-point RGB(A)
  switch (colorspace) {
    case ImageColorspace::GRAY:
      return TextureInternalFormat::R16F;

    case ImageColorspace::GRAY_ALPHA:
      return TextureInternalFormat::RG16F;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
      return TextureInternalFormat::RGB16F;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      return TextureInternalFormat::RGBA16F;

    default:
      break;
  }

  throw std::invalid_argument("Error: Invalid image colorspace");
}

} // namespace

Cubemap::Cubemap() {
  ZoneScopedN("Cubemap::Cubemap");

  Logger::debug("[Cubemap] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Cubemap] Created (ID: {})", m_index.get());
}

const RenderShaderProgram& Cubemap::getProgram() const {
  static const RenderShaderProgram cubemapProgram = [] () {
    RenderShaderProgram program(VertexShader::loadFromSource(vertSource), FragmentShader::loadFromSource(fragSource));
    program.setAttribute(0, "uniSkybox");
    program.sendAttributes();

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, program.getIndex(), "Cubemap shader program");
      Renderer::setLabel(RenderObjectType::SHADER, program.getVertexShader().getIndex(), "Cubemap vertex shader");
      Renderer::setLabel(RenderObjectType::SHADER, program.getFragmentShader().getIndex(), "Cubemap fragment shader");
    }
#endif

    return program;
  }();

  return cubemapProgram;
}

void Cubemap::load(const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) const {
  ZoneScopedN("Cubemap::load");

  bind();

  constexpr auto mapImage = [] (const Image& img, TextureType type) {
    if (img.isEmpty()) {
      Logger::error("[Cubemap] Empty image given as cubemap face");
      return;
    }

    Renderer::sendImageData2D(type,
                              0,
                              recoverInternalFormat(img.getColorspace(), img.getDataType()),
                              img.getWidth(),
                              img.getHeight(),
                              recoverFormat(img.getColorspace()),
                              (img.getDataType() == ImageDataType::FLOAT ? PixelDataType::FLOAT : PixelDataType::UBYTE),
                              img.getDataPtr());
  };

  //            ______________________
  //           /|                   /|
  //          / |                  / |
  //         /  |       +Y        /  |
  //        /   |                /   |
  //       |--------------------|    |
  //       |    |        -Z     |    |
  //       | -X |               | +X |
  //       |    |               |    |
  //       |    |    +Z         |    |
  //       |   /----------------|---/
  //       |  /                 |  /
  //       | /        -Y        | /
  //       |/                   |/
  //       ----------------------

  mapImage(right, TextureType::CUBEMAP_POS_X);
  mapImage(left, TextureType::CUBEMAP_NEG_X);
  mapImage(top, TextureType::CUBEMAP_POS_Y);
  mapImage(bottom, TextureType::CUBEMAP_NEG_Y);
  mapImage(front, TextureType::CUBEMAP_POS_Z);
  mapImage(back, TextureType::CUBEMAP_NEG_Z);

  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParameter::MINIFY_FILTER, TextureParameterValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParameter::MAGNIFY_FILTER, TextureParameterValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParameter::WRAP_S, TextureParameterValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParameter::WRAP_T, TextureParameterValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParameter::WRAP_R, TextureParameterValue::CLAMP_TO_EDGE);

  unbind();

#if !defined(USE_OPENGL_ES)
  // Setting the label right after creating the texture works, but generates an OpenGL error. This is thus done here instead
  if (Renderer::checkVersion(4, 3))
    Renderer::setLabel(RenderObjectType::TEXTURE, m_index, "Cubemap texture");
#endif
}

void Cubemap::bind() const {
  Renderer::bindTexture(TextureType::CUBEMAP, m_index);
}

void Cubemap::unbind() const {
  Renderer::unbindTexture(TextureType::CUBEMAP);
}

void Cubemap::draw() const {
  ZoneScopedN("Cubemap::draw");
  TracyGpuZone("Cubemap::draw")

  static const VertexArray vao;
  vao.bind();

  getProgram().use();
  Renderer::setActiveTexture(0);
  bind();

  Renderer::setDepthFunction(DepthStencilFunction::LESS_EQUAL);
  Renderer::drawArrays(PrimitiveType::TRIANGLES, 36);
  Renderer::setDepthFunction(DepthStencilFunction::LESS);
}

Cubemap::~Cubemap() {
  ZoneScopedN("Cubemap::~Cubemap");

  if (!m_index.isValid())
    return;

  Logger::debug("[Cubemap] Destroying (ID: {})...", m_index.get());
  Renderer::deleteTexture(m_index);
  Logger::debug("[Cubemap] Destroyed");
}

} // namespace Raz
