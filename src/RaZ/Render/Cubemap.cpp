#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Utils/Logger.hpp"

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

    vec4 pos = uniProjectionMat * mat4(mat3(uniViewMat)) * vec4(vertPosition, 1.0);
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

} // namespace

Cubemap::Cubemap() {
  Logger::debug("[Cubemap] Creating...");

  Renderer::generateTexture(m_index);

  m_program.setVertexShader(VertexShader::loadFromSource(vertSource));
  m_program.setFragmentShader(FragmentShader::loadFromSource(fragSource));
  m_program.compileShaders();
  m_program.link();

  m_program.use();
  m_program.sendUniform("uniSkybox", 0);

  Logger::debug("[Cubemap] Created (ID: " + std::to_string(m_index) + ')');
}

Cubemap::Cubemap(Cubemap&& cubemap) noexcept
  : m_index{ std::exchange(cubemap.m_index, std::numeric_limits<unsigned int>::max()) },
    m_program{ std::move(cubemap.m_program) } {}

void Cubemap::load(const FilePath& rightTexturePath, const FilePath& leftTexturePath,
                   const FilePath& topTexturePath, const FilePath& bottomTexturePath,
                   const FilePath& frontTexturePath, const FilePath& backTexturePath) const {
  bind();

  constexpr auto mapImage = [] (const Image& img, TextureType type) {
    Renderer::sendImageData2D(type,
                              0,
                              static_cast<TextureInternalFormat>(img.getColorspace()),
                              img.getWidth(), img.getHeight(),
                              static_cast<TextureFormat>(img.getColorspace()), TextureDataType::UBYTE,
                              img.getDataPtr());
  };

  mapImage(ImageFormat::load(rightTexturePath), TextureType::CUBEMAP_POS_X);
  mapImage(ImageFormat::load(leftTexturePath), TextureType::CUBEMAP_NEG_X);
  mapImage(ImageFormat::load(topTexturePath), TextureType::CUBEMAP_POS_Y);
  mapImage(ImageFormat::load(bottomTexturePath), TextureType::CUBEMAP_NEG_Y);
  mapImage(ImageFormat::load(frontTexturePath), TextureType::CUBEMAP_POS_Z);
  mapImage(ImageFormat::load(backTexturePath), TextureType::CUBEMAP_NEG_Z);

  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MINIFY_FILTER, TextureParamValue::LINEAR);
  Renderer::setTextureParameter(TextureType::CUBEMAP, TextureParam::MAGNIFY_FILTER, TextureParamValue::LINEAR);
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

void Cubemap::draw() const {
  Renderer::setDepthFunction(DepthStencilFunction::LESS_EQUAL);
  Renderer::setFaceCulling(FaceOrientation::FRONT);

  m_program.use();

  Renderer::activateTexture(0);
  bind();

  MeshRenderer::drawUnitCube();

  Renderer::setFaceCulling(FaceOrientation::BACK);
  Renderer::setDepthFunction(DepthStencilFunction::LESS);
}

Cubemap& Cubemap::operator=(Cubemap&& cubemap) noexcept {
  std::swap(m_index, cubemap.m_index);
  m_program = std::move(cubemap.m_program);

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
