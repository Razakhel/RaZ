#include "RaZ/RaZ.hpp"

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

    vec3 normal = texture(uniMaterial.normalMap, vertMeshInfo.vertTexcoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(vertMeshInfo.vertTBNMatrix * normal);

    // Using the emissive (which will always be 0 here) to avoid many warnings about unrecognized uniform names
    fragColor  = vec4(albedo + emissive, metallic);
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
    vec4 color       = texture(uniSceneBuffers.color, fragTexcoords).rgba;
    vec4 normalRough = texture(uniSceneBuffers.normal, fragTexcoords).rgba;
    vec3 normal      = normalRough.rgb;
    float roughness  = normalRough.a;

    if (int(gl_FragCoord.y) > 360) {
      if (int(gl_FragCoord.x) < 640)
        fragColor = vec4(vec3(depth), 1.0);
      else
        fragColor = vec4(normal, 1.0);
    } else {
      if (int(gl_FragCoord.x) < 640)
        fragColor = vec4(vec3(roughness), 1.0);
      else
        fragColor = color;
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
    render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT "shaders/common.vert"),
                                           Raz::FragmentShader::loadFromSource(geomFragSource));

    Raz::Window& window = render.getWindow();

    ///////////////////
    // Render passes //
    ///////////////////

    Raz::RenderGraph& renderGraph = render.getRenderGraph();

    // Creating the render graph's texture buffers
    const auto depthBuffer  = Raz::Texture::create(sceneWidth, sceneHeight, Raz::ImageColorspace::DEPTH);
    const auto colorBuffer  = Raz::Texture::create(sceneWidth, sceneHeight, Raz::ImageColorspace::RGBA);
    const auto normalBuffer = Raz::Texture::create(sceneWidth, sceneHeight, Raz::ImageColorspace::RGBA);

    // Setting geometry pass' shaders & defining its write buffers
    Raz::RenderPass& geomPass = renderGraph.getGeometryPass();
    geomPass.addWriteTexture(depthBuffer);
    geomPass.addWriteTexture(colorBuffer);
    geomPass.addWriteTexture(normalBuffer);

    // Adding the second pass & defining its read buffers
    Raz::RenderPass& splitPass = renderGraph.addNode(Raz::FragmentShader::loadFromSource(displayFragSource));
    splitPass.addReadTexture(depthBuffer, "uniSceneBuffers.depth");
    splitPass.addReadTexture(colorBuffer, "uniSceneBuffers.color");
    splitPass.addReadTexture(normalBuffer, "uniSceneBuffers.normal");

    geomPass.addChildren(splitPass);

    //////////
    // Mesh //
    //////////

    // Importing the mesh & transforming it so that it can be fully visible
    Raz::Entity& mesh = world.addEntity();

    auto [meshData, meshRenderData] = Raz::ObjFormat::load(RAZ_ROOT "assets/meshes/shield.obj");
    mesh.addComponent<Raz::Mesh>(std::move(meshData));
    mesh.addComponent<Raz::MeshRenderer>(std::move(meshRenderData));

    auto& meshTrans = mesh.addComponent<Raz::Transform>();
    meshTrans.scale(0.2f);

    ////////////
    // Camera //
    ////////////

    Raz::Entity& camera = world.addEntity();
    auto& cameraComp    = camera.addComponent<Raz::Camera>(sceneWidth, sceneHeight);
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));

    float cameraSpeed = 1.f;
    window.addKeyCallback(Raz::Keyboard::LEFT_SHIFT,
                          [&cameraSpeed] (float /* deltaTime */) noexcept { cameraSpeed = 2.f; },
                          Raz::Input::ONCE,
                          [&cameraSpeed] () noexcept { cameraSpeed = 1.f; });
    window.addKeyCallback(Raz::Keyboard::SPACE, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move(0.f, (10.f * deltaTime) * cameraSpeed, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::V, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move(0.f, (-10.f * deltaTime) * cameraSpeed, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::W, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move(0.f, 0.f, (-10.f * deltaTime) * cameraSpeed);
    });
    window.addKeyCallback(Raz::Keyboard::S, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move(0.f,  0.f, (10.f * deltaTime) * cameraSpeed);
    });
    window.addKeyCallback(Raz::Keyboard::A, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
    });
    window.addKeyCallback(Raz::Keyboard::D, [&cameraTrans, &cameraSpeed] (float deltaTime) {
      cameraTrans.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
    });

    window.setMouseMoveCallback([&cameraTrans, &window] (double xMove, double yMove) {
      // Dividing move by window size to scale between -1 and 1
      cameraTrans.rotate(-90_deg * static_cast<float>(yMove) / window.getHeight(),
                         -90_deg * static_cast<float>(xMove) / window.getWidth());
    });

    //////////////////////
    // Window callbacks //
    //////////////////////

    // Toggling the render pass' enabled state
    window.addKeyCallback(Raz::Keyboard::R, [&splitPass] (float /* deltaTime */) noexcept { splitPass.enable(!splitPass.isEnabled()); }, Raz::Input::ONCE);

    // Allowing to quit the application by pressing the Esc key
    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    // Allowing to quit the application when the close button is clicked
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    /////////////
    // Overlay //
    /////////////

#if !defined(RAZ_NO_OVERLAY)
    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Deferred demo", Raz::Vec2f(sceneWidth / 4, sceneHeight));

    overlay.addTexture(*depthBuffer, sceneWidth / 4, sceneHeight / 4);
    overlay.addTexture(*colorBuffer, sceneWidth / 4, sceneHeight / 4);
    overlay.addTexture(*normalBuffer, sceneWidth / 4, sceneHeight / 4);

    overlay.addSeparator();

    overlay.addFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
    overlay.addFpsCounter("FPS: %.1f");
#endif

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
