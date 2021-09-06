#include "RaZ/RaZ.hpp"

using namespace std::literals;

constexpr unsigned int sceneWidth  = 1280;
constexpr unsigned int sceneHeight = 720;

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
constexpr std::string_view geomFragSource = R"(
  #version 330 core

  struct Material {
    vec3 baseColor;
    float metallicFactor;
    float roughnessFactor;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D ambientOcclusionMap;
  };

  in MeshInfo {
    vec3 vertPosition;
    vec2 vertTexcoords;
    mat3 vertTBNMatrix;
  } fragMeshInfo;

  layout(std140) uniform uboCameraMatrices {
    mat4 viewMat;
    mat4 invViewMat;
    mat4 projectionMat;
    mat4 invProjectionMat;
    mat4 viewProjectionMat;
    vec3 cameraPos;
  };

  uniform Material uniMaterial;

  layout(location = 0) out vec4 fragColor;
  layout(location = 1) out vec4 fragNormal;

  void main() {
    vec3 albedo     = pow(texture(uniMaterial.albedoMap, fragMeshInfo.vertTexcoords).rgb, vec3(2.2)) * uniMaterial.baseColor;
    float metallic  = texture(uniMaterial.metallicMap, fragMeshInfo.vertTexcoords).r * uniMaterial.metallicFactor;
    float roughness = texture(uniMaterial.roughnessMap, fragMeshInfo.vertTexcoords).r * uniMaterial.roughnessFactor;

    vec3 normal = texture(uniMaterial.normalMap, fragMeshInfo.vertTexcoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(fragMeshInfo.vertTBNMatrix * normal);

    fragColor  = vec4(albedo, metallic);
    fragNormal = vec4(normal, roughness);
  }
)";

constexpr std::string_view displayFragSource = R"(
  #version 330 core

  struct Buffers {
    sampler2D depth;
    sampler2D color;
    sampler2D normal;
  };

  in vec2 fragTexcoords;

  uniform Buffers uniSceneBuffers;

  layout(location = 0) out vec4 fragColor;

  void main() {
    float depth     = texture(uniSceneBuffers.depth, fragTexcoords).r;
    vec4 color      = texture(uniSceneBuffers.color, fragTexcoords).rgba;
    vec3 normal     = texture(uniSceneBuffers.normal, fragTexcoords).rgb;
    float roughness = texture(uniSceneBuffers.normal, fragTexcoords).a;

    if (gl_FragCoord.y > 360) {
      if (gl_FragCoord.x < 640)
        fragColor = vec4(vec3(depth), 1.0);
      else
        fragColor = vec4(normal, 1.0);
    } else {
      if (gl_FragCoord.x < 640)
        fragColor = vec4(vec3(roughness), 1.0);
      else
        fragColor = color;
    }
  }
)";
#else // Emscripten/OpenGL ES
// The version must be on the first line
constexpr std::string_view geomFragSource = R"(#version 300 es

  precision highp float;
  precision highp int;

  struct Material {
    vec3 baseColor;
    float metallicFactor;
    float roughnessFactor;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D ambientOcclusionMap;
  };

  in struct MeshInfo {
    vec3 vertPosition;
    vec2 vertTexcoords;
    mat3 vertTBNMatrix;
  } fragMeshInfo;

  layout(std140) uniform uboCameraMatrices {
    mat4 viewMat;
    mat4 invViewMat;
    mat4 projectionMat;
    mat4 invProjectionMat;
    mat4 viewProjectionMat;
    vec3 cameraPos;
  };

  uniform Material uniMaterial;

  layout(location = 0) out vec4 fragColor;
  layout(location = 1) out vec4 fragNormal;

  void main() {
    vec3 albedo     = pow(texture(uniMaterial.albedoMap, fragMeshInfo.vertTexcoords).rgb, vec3(2.2)) * uniMaterial.baseColor;
    float metallic  = texture(uniMaterial.metallicMap, fragMeshInfo.vertTexcoords).r * uniMaterial.metallicFactor;
    float roughness = texture(uniMaterial.roughnessMap, fragMeshInfo.vertTexcoords).r * uniMaterial.roughnessFactor;

    vec3 normal = texture(uniMaterial.normalMap, fragMeshInfo.vertTexcoords).rgb;
    normal      = normalize(normal * 2.0 - 1.0);
    normal      = normalize(fragMeshInfo.vertTBNMatrix * normal);

    fragColor  = vec4(albedo, metallic);
    fragNormal = vec4(normal, roughness);
  }
)";

constexpr std::string_view displayFragSource = R"(#version 300 es

  precision highp float;
  precision highp int;

  struct Buffers {
    sampler2D depth;
    sampler2D color;
    sampler2D normal;
  };

  in vec2 fragTexcoords;

  uniform Buffers uniSceneBuffers;

  layout(location = 0) out vec4 fragColor;

  void main() {
    float depth     = texture(uniSceneBuffers.depth, fragTexcoords).r;
    vec4 color      = texture(uniSceneBuffers.color, fragTexcoords).rgba;
    vec3 normal     = texture(uniSceneBuffers.normal, fragTexcoords).rgb;
    float roughness = texture(uniSceneBuffers.normal, fragTexcoords).a;

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
#endif

int main() {
  Raz::Application app;
  Raz::World& world = app.addWorld(2);

  auto& render = world.addSystem<Raz::RenderSystem>(sceneWidth, sceneHeight, "RaZ");
  render.getGeometryProgram().setShaders(Raz::VertexShader(RAZ_ROOT + "shaders/common.vert"s),
                                         Raz::FragmentShader::loadFromSource(geomFragSource));

  Raz::Window& window = render.getWindow();

  ///////////////////
  // Render passes //
  ///////////////////

  // Creating the render graph's texture buffers
  const Raz::Texture& geomDepthBuffer  = render.getRenderGraph().addTextureBuffer(sceneWidth, sceneHeight, 0, Raz::ImageColorspace::DEPTH);
  const Raz::Texture& geomColorBuffer  = render.getRenderGraph().addTextureBuffer(sceneWidth, sceneHeight, 1, Raz::ImageColorspace::RGBA);
  const Raz::Texture& geomNormalBuffer = render.getRenderGraph().addTextureBuffer(sceneWidth, sceneHeight, 2, Raz::ImageColorspace::RGBA);

  // Setting geometry pass' shaders & defining its write buffers
  Raz::RenderPass& geomPass = render.getGeometryPass();
  geomPass.addWriteTexture(geomDepthBuffer);
  geomPass.addWriteTexture(geomColorBuffer);
  geomPass.addWriteTexture(geomNormalBuffer);

  // Adding the second pass & defining its read buffers
  Raz::RenderPass& renderPass = render.addRenderPass(Raz::FragmentShader::loadFromSource(displayFragSource));
  renderPass.addReadTexture(geomDepthBuffer, "uniSceneBuffers.depth");
  renderPass.addReadTexture(geomColorBuffer, "uniSceneBuffers.color");
  renderPass.addReadTexture(geomNormalBuffer, "uniSceneBuffers.normal");

  geomPass.addChildren(renderPass);

  // Importing the mesh & transforming it so that it can be fully visible
  Raz::Entity& mesh = world.addEntity();
  mesh.addComponent<Raz::Mesh>(RAZ_ROOT + "assets/meshes/shield.obj"s);

  auto& meshTrans = mesh.addComponent<Raz::Transform>();
  meshTrans.scale(0.2f);
  meshTrans.rotate(180_deg, Raz::Axis::Y);

  ////////////
  // Camera //
  ////////////

  Raz::Entity& camera = world.addEntity();
  auto& cameraComp    = camera.addComponent<Raz::Camera>(sceneWidth, sceneHeight);
  auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, -5.f));

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
    cameraTrans.move(0.f, 0.f, (10.f * deltaTime) * cameraSpeed);
  });
  window.addKeyCallback(Raz::Keyboard::S, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move(0.f,  0.f, (-10.f * deltaTime) * cameraSpeed);
  });
  window.addKeyCallback(Raz::Keyboard::A, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((-10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });
  window.addKeyCallback(Raz::Keyboard::D, [&cameraTrans, &cameraSpeed] (float deltaTime) {
    cameraTrans.move((10.f * deltaTime) * cameraSpeed, 0.f, 0.f);
  });

  window.addMouseMoveCallback([&cameraTrans, &window] (double xMove, double yMove) {
    // Dividing move by window size to scale between -1 and 1
    cameraTrans.rotate(-90_deg * static_cast<float>(yMove) / window.getHeight(),
                       -90_deg * static_cast<float>(xMove) / window.getWidth());
  });

  //////////////////////
  // Window callbacks //
  //////////////////////

#if !defined(USE_OPENGL_ES)
  // Toggling the wireframe rendering's state
  bool isWireframe = false;
  window.addKeyCallback(Raz::Keyboard::Z, [&isWireframe] (float /* deltaTime */) {
    isWireframe = !isWireframe;
    Raz::Renderer::setPolygonMode(Raz::FaceOrientation::FRONT_BACK, (isWireframe ? Raz::PolygonMode::LINE : Raz::PolygonMode::FILL));
  }, Raz::Input::ONCE);
#endif

  // Toggling the render pass' enabled state
  window.addKeyCallback(Raz::Keyboard::R, [&renderPass] (float /* deltaTime */) noexcept { renderPass.enable(!renderPass.isEnabled()); }, Raz::Input::ONCE);

  // Allowing to quit the application by pressing the Esc key
  window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
  // Allowing to quit the application when the close button is clicked
  window.setCloseCallback([&app] () noexcept { app.quit(); });

  /////////////
  // Overlay //
  /////////////

#if !defined(RAZ_NO_OVERLAY)
  window.enableOverlay();

  window.addOverlayLabel("RaZ - Deferred demo");

  window.addOverlaySeparator();

  window.addOverlayTexture(geomDepthBuffer, sceneWidth / 3, sceneHeight / 3);
  window.addOverlayTexture(geomColorBuffer, sceneWidth / 3, sceneHeight / 3);
  window.addOverlayTexture(geomNormalBuffer, sceneWidth / 3, sceneHeight / 3);

  window.addOverlaySeparator();

  window.addOverlayFrameTime("Frame time: %.3f ms/frame"); // Frame time's & FPS counter's texts must be formatted
  window.addOverlayFpsCounter("FPS: %.1f");
#endif

  // Running the application
  app.run();

  return EXIT_SUCCESS;
}
