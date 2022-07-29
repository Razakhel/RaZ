#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <utility>

namespace Raz {

namespace {

constexpr std::string_view vertSource = R"(
  layout(location = 0) in vec3 vertPosition;

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
    fragTexcoords = vertPosition;

    vec4 pos    = uniProjectionMat * mat4(mat3(uniViewMat)) * vec4(vertPosition, 1.0);
    gl_Position = pos.xyww;
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

inline const MeshRenderer& getDisplayCube() {
  static const MeshRenderer cube = [] () {
    MeshRenderer meshRenderer(Mesh(AABB(Vec3f(-1.f, -1.f, -1.f),
                                        Vec3f(1.f, 1.f, 1.f))), RenderMode::TRIANGLE);

    meshRenderer.setMaterial(Material());

    RenderShaderProgram& program = meshRenderer.getMaterials().front().getProgram();
    program.setShaders(VertexShader::loadFromSource(vertSource), FragmentShader::loadFromSource(fragSource));
    program.use();
    program.sendUniform("uniSkybox", 0);

#if !defined(USE_OPENGL_ES)
    if (Renderer::checkVersion(4, 3)) {
      Renderer::setLabel(RenderObjectType::PROGRAM, program.getIndex(), "Cubemap shader program");
      Renderer::setLabel(RenderObjectType::SHADER, program.getVertexShader().getIndex(), "Cubemap vertex shader");
      Renderer::setLabel(RenderObjectType::SHADER, program.getFragmentShader().getIndex(), "Cubemap fragment shader");
    }
#endif

    return meshRenderer;
  }();

  return cube;
}

TextureFormat recoverFormat(const Image& image) {
  TextureFormat colorFormat {};

  switch (image.getColorspace()) {
    case ImageColorspace::GRAY:
      colorFormat = TextureFormat::RED;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorFormat = TextureFormat::RG;
      break;

    case ImageColorspace::RGB:
    case ImageColorspace::SRGB:
    default:
      colorFormat = TextureFormat::RGB;
      break;

    case ImageColorspace::RGBA:
    case ImageColorspace::SRGBA:
      colorFormat = TextureFormat::RGBA;
      break;

    case ImageColorspace::DEPTH:
      colorFormat = TextureFormat::DEPTH;
      break;
  }

  return colorFormat;
}

TextureInternalFormat recoverInternalFormat(const Image& image) {
  // If the image is of a byte data type and not an sRGB colorspace, its internal format is the same as its format
  if (image.getDataType() == ImageDataType::BYTE && image.getColorspace() != ImageColorspace::SRGB && image.getColorspace() != ImageColorspace::SRGBA)
    return static_cast<TextureInternalFormat>(recoverFormat(image));

  TextureInternalFormat colorFormat {};

  switch (image.getColorspace()) {
    case ImageColorspace::GRAY:
      colorFormat = TextureInternalFormat::R16F;
      break;

    case ImageColorspace::GRAY_ALPHA:
      colorFormat = TextureInternalFormat::RG16F;
      break;

    case ImageColorspace::RGB:
    default:
      colorFormat = TextureInternalFormat::RGB16F;
      break;

    case ImageColorspace::RGBA:
      colorFormat = TextureInternalFormat::RGBA16F;
      break;

    case ImageColorspace::SRGB:
      colorFormat = TextureInternalFormat::SRGB8;
      break;

    case ImageColorspace::SRGBA:
      colorFormat = TextureInternalFormat::SRGBA8;
      break;

    case ImageColorspace::DEPTH:
      colorFormat = TextureInternalFormat::DEPTH32F;
      break;
  }

  return colorFormat;
}

} // namespace

Cubemap::Cubemap() {
  Logger::debug("[Cubemap] Creating...");
  Renderer::generateTexture(m_index);
  Logger::debug("[Cubemap] Created (ID: " + std::to_string(m_index) + ')');
}

const RenderShaderProgram& Cubemap::getProgram() const {
  return getDisplayCube().getMaterials().front().getProgram();
}

Cubemap::Cubemap(Cubemap&& cubemap) noexcept
  : m_index{ std::exchange(cubemap.m_index, std::numeric_limits<unsigned int>::max()) } {}

void Cubemap::load(const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) const {
  bind();

  constexpr auto mapImage = [] (const Image& img, TextureType type) {
    Renderer::sendImageData2D(type,
                              0,
                              recoverInternalFormat(img),
                              img.getWidth(),
                              img.getHeight(),
                              recoverFormat(img),
                              (img.getDataType() == ImageDataType::FLOAT ? TextureDataType::FLOAT : TextureDataType::UBYTE),
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

  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_S, TextureParamValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_T, TextureParamValue::CLAMP_TO_EDGE);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::WRAP_R, TextureParamValue::CLAMP_TO_EDGE);

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
  Renderer::setDepthFunction(DepthStencilFunction::LESS_EQUAL);
  Renderer::setFaceCulling(FaceOrientation::FRONT);

  const MeshRenderer& displayCube = getDisplayCube();

  displayCube.getMaterials().front().getProgram().use();

  Renderer::activateTexture(0);
  bind();

  displayCube.draw();

  Renderer::setFaceCulling(FaceOrientation::BACK);
  Renderer::setDepthFunction(DepthStencilFunction::LESS);
}

Cubemap& Cubemap::operator=(Cubemap&& cubemap) noexcept {
  std::swap(m_index, cubemap.m_index);

  return *this;
}

Cubemap::~Cubemap() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Logger::debug("[Cubemap] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteTexture(m_index);
  Logger::debug("[Cubemap] Destroyed");
}

} // namespace Raz
