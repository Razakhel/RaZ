#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

constexpr unsigned int sceneWidth  = 1280;
constexpr unsigned int sceneHeight = 720;

constexpr std::string_view geomFragSource = R"(
  struct Material {
    vec3 baseColor;
    vec3 emissive;
    float metallicFactor;
    float roughnessFactor;

    sampler2D baseColorMap;
    sampler2D emissiveMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D ambientMap;
  };

  in struct MeshInfo {
    vec3 vertPosition;
    vec2 vertTexcoords;
    mat3 vertTBNMatrix;
  } vertMeshInfo;

  layout(std140) uniform uboCameraInfo {
    mat4 uniViewMat;
    mat4 uniInvViewMat;
    mat4 uniProjectionMat;
    mat4 uniInvProjectionMat;
    mat4 uniViewProjectionMat;
    vec3 uniCameraPos;
  };

  uniform Material uniMaterial;

  layout(location = 0) out vec4 fragColor;
  layout(location = 1) out vec4 fragNormal;

  void main() {
    vec3 albedo     = pow(texture(uniMaterial.baseColorMap, vertMeshInfo.vertTexcoords).rgb, vec3(2.2)) * uniMaterial.baseColor;
    vec3 emissive   = texture(uniMaterial.emissiveMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;
    float metallic  = texture(uniMaterial.metallicMap, vertMeshInfo.vertTexcoords).r * uniMaterial.metallicFactor;
    float roughness = texture(uniMaterial.roughnessMap, vertMeshInfo.vertTexcoords).r * uniMaterial.roughnessFactor;
    float ambOcc    = texture(uniMaterial.ambientMap, vertMeshInfo.vertTexcoords).r;

    vec3 normal = texture(uniMaterial.normalMap, vertMeshInfo.vertTexcoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(vertMeshInfo.vertTBNMatrix * normal);

    // Using the emissive (which will always be 0 here) to avoid many warnings about unrecognized uniform names
    fragColor  = vec4(albedo + (ambOcc - 1.0) + emissive, metallic);
    fragNormal = vec4(normal, roughness);
  }
)";

constexpr std::string_view displayFragSource = R"(
  struct Buffers {
    sampler2D depth;
    sampler2D color;
    sampler2D normal;
  };

  in vec2 fragTexcoords;

  uniform Buffers uniSceneBuffers;

  layout(location = 0) out vec4 fragColor;

  void main() {
    float depth      = texture(uniSceneBuffers.depth, fragTexcoords).r;
    vec4 colorMetal  = texture(uniSceneBuffers.color, fragTexcoords).rgba;
    vec3 color       = colorMetal.rgb;
    float metalness  = colorMetal.a;
    vec4 normalRough = texture(uniSceneBuffers.normal, fragTexcoords).rgba;
    vec3 normal      = normalRough.rgb;
    float roughness  = normalRough.a;

    if (fragTexcoords.y > 0.5) {
      if (fragTexcoords.x < 0.5)
        fragColor = vec4(vec3(depth), 1.0);
      else
        fragColor = vec4(normal, 1.0);
    } else {
      if (fragTexcoords.x < 0.5)
        fragColor = vec4(1.0, metalness, roughness, 1.0);
      else
        fragColor = vec4(color, 1.0);
    }
  }
)";

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(2);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& render = world.addSystem<Raz::RenderSystem>(sceneWidth, sceneHeight, "RaZ");

    Raz::Window& window = render.getWindow();

    // Allowing to quit the application by pressing the Escape key
    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    // Quitting the application when the close button is clicked
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    ///////////////////
    // Render passes //
    ///////////////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();

    // Creating the render graph's texture buffers
    const auto depthBuffer  = Raz::Texture2D::create(sceneWidth, sceneHeight, Raz::TextureColorspace::DEPTH);
    const auto colorBuffer  = Raz::Texture2D::create(sceneWidth, sceneHeight, Raz::TextureColorspace::RGBA);
    const auto normalBuffer = Raz::Texture2D::create(sceneWidth, sceneHeight, Raz::TextureColorspace::RGBA);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "Depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "Color buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, normalBuffer->getIndex(), "Normal buffer");
    }
#endif

    // Setting the geometry pass' write buffers
    Raz::RenderPass& geomPass = renderGraph.getGeometryPass();
    geomPass.setWriteDepthTexture(depthBuffer);
    geomPass.addWriteColorTexture(colorBuffer, 0);
    geomPass.addWriteColorTexture(normalBuffer, 1);

    // Adding the second pass & defining its read buffers
    Raz::RenderPass& splitPass = renderGraph.addNode(Raz::FragmentShader::loadFromSource(displayFragSource));
    splitPass.addReadTexture(depthBuffer, "uniSceneBuffers.depth");
    splitPass.addReadTexture(colorBuffer, "uniSceneBuffers.color");
    splitPass.addReadTexture(normalBuffer, "uniSceneBuffers.normal");

    geomPass.addChildren(splitPass);

    // Toggling the render pass' enabled state
    window.addKeyCallback(Raz::Keyboard::R, [&splitPass] (float /* deltaTime */) noexcept { splitPass.enable(!splitPass.isEnabled()); }, Raz::Input::ONCE);

    ////////////
    // Camera //
    ////////////

    Raz::Entity& camera = world.addEntity();
    auto& cameraComp    = camera.addComponent<Raz::Camera>(sceneWidth, sceneHeight);
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));

    DemoUtils::setupCameraControls(camera, window);

    //////////
    // Mesh //
    //////////

    Raz::Entity& mesh  = world.addEntity();
    auto& meshRenderer = mesh.addComponent<Raz::MeshRenderer>(Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/shield.obj").second);

    Raz::RenderShaderProgram& shaderProgram = meshRenderer.getMaterials().front().getProgram();
    shaderProgram.setFragmentShader(Raz::FragmentShader::loadFromSource(geomFragSource));
    shaderProgram.link();

    auto& meshTrans = mesh.addComponent<Raz::Transform>();
    meshTrans.scale(0.2f);

    /////////////
    // Overlay //
    /////////////

#if !defined(RAZ_NO_OVERLAY)
    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Deferred demo", Raz::Vec2f(sceneWidth / 4, sceneHeight));

    DemoUtils::insertOverlayCameraControlsHelp(overlay);

    overlay.addSeparator();

    overlay.addTexture(*depthBuffer, sceneWidth / 4, sceneHeight / 4);
    overlay.addTexture(*colorBuffer, sceneWidth / 4, sceneHeight / 4);
    overlay.addTexture(*normalBuffer, sceneWidth / 4, sceneHeight / 4);
#endif

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
