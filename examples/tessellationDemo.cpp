#include "RaZ/RaZ.hpp"

namespace {

constexpr std::string_view tessCtrlSource = R"(
  layout(vertices = 4) out;

  struct MeshInfo {
    vec3 vertPosition;
    vec2 vertTexcoords;
    mat3 vertTBNMatrix;
  };

  in MeshInfo vertMeshInfo[];

  uniform float uniTessLevelOuter = 4.0;
  uniform float uniTessLevelInner = 4.0;

  out MeshInfo tessMeshInfo[];

  void main() {
    gl_out[gl_InvocationID].gl_Position         = gl_in[gl_InvocationID].gl_Position;
    tessMeshInfo[gl_InvocationID].vertPosition  = vertMeshInfo[gl_InvocationID].vertPosition;
    tessMeshInfo[gl_InvocationID].vertTexcoords = vertMeshInfo[gl_InvocationID].vertTexcoords;
    tessMeshInfo[gl_InvocationID].vertTBNMatrix = vertMeshInfo[gl_InvocationID].vertTBNMatrix;

    // Setting levels only needs to be done once
    if (gl_InvocationID == 0) {
      gl_TessLevelOuter[0] = uniTessLevelOuter;
      gl_TessLevelOuter[1] = uniTessLevelOuter;
      gl_TessLevelOuter[2] = uniTessLevelOuter;
      gl_TessLevelOuter[3] = uniTessLevelOuter;

      gl_TessLevelInner[0] = uniTessLevelInner;
      gl_TessLevelInner[1] = uniTessLevelInner;
    }
  }
)";

constexpr std::string_view tessEvalSource = R"(
  layout(quads, fractional_odd_spacing, ccw) in;

  struct MeshInfo {
    vec3 vertPosition;
    vec2 vertTexcoords;
    mat3 vertTBNMatrix;
  };

  in MeshInfo tessMeshInfo[];

  uniform bool uniMakeSphere = true;

  out MeshInfo vertMeshInfo;

  layout(std140) uniform uboCameraInfo {
    mat4 uniViewMat;
    mat4 uniInvViewMat;
    mat4 uniProjectionMat;
    mat4 uniInvProjectionMat;
    mat4 uniViewProjectionMat;
    vec3 uniCameraPos;
  };

  void main() {
    vec3 bottomLeftPos  = tessMeshInfo[0].vertPosition;
    vec3 topLeftPos     = tessMeshInfo[1].vertPosition;
    vec3 bottomRightPos = tessMeshInfo[2].vertPosition;
    vec3 topRightPos    = tessMeshInfo[3].vertPosition;

    // Interpolating bilinearly to recover the position on the patch
    vec3 vertPos0 = mix(tessMeshInfo[0].vertPosition, tessMeshInfo[2].vertPosition, gl_TessCoord.x);
    vec3 vertPos1 = mix(tessMeshInfo[1].vertPosition, tessMeshInfo[3].vertPosition, gl_TessCoord.x);
    vec3 vertPos  = mix(vertPos0, vertPos1, gl_TessCoord.y);

    // Interpolating bilinearly to recover texcoords on the patch
    vec2 vertUV0 = mix(tessMeshInfo[0].vertTexcoords, tessMeshInfo[2].vertTexcoords, gl_TessCoord.x);
    vec2 vertUV1 = mix(tessMeshInfo[1].vertTexcoords, tessMeshInfo[3].vertTexcoords, gl_TessCoord.x);
    vec2 vertUV  = mix(vertUV0, vertUV1, gl_TessCoord.y);

    if (uniMakeSphere) {
      vertPos = normalize(vertPos);
      vertMeshInfo.vertTBNMatrix[2] = vertPos;
    } else {
      // No need to do another bilinear interpolation: we can just use any normal since all vertices in a patch have the same one
      vertMeshInfo.vertTBNMatrix[2] = tessMeshInfo[0].vertTBNMatrix[2];
    }

    vertMeshInfo.vertPosition     = vertPos;
    vertMeshInfo.vertTexcoords    = vertUV;
    vertMeshInfo.vertTBNMatrix[0] = vertMeshInfo.vertTBNMatrix[2].zxy;
    vertMeshInfo.vertTBNMatrix[1] = cross(vertMeshInfo.vertTBNMatrix[0], vertMeshInfo.vertTBNMatrix[2]);

    gl_Position = uniViewProjectionMat * vec4(vertPos, 1.0);
  }
)";

} // namespace

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

    auto& render = world.addSystem<Raz::RenderSystem>(1280u, 720u, "RaZ");

    if (!Raz::Renderer::checkVersion(4, 0) && !Raz::Renderer::isExtensionSupported("GL_ARB_tessellation_shader")) {
      throw std::runtime_error("Error: Tessellation is only available with an OpenGL 4.0 context or above, or with the 'GL_ARB_tessellation_shader' extension; "
                               "please update your graphics drivers or try on another computer");
    }

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(render.getWindow().getWidth(), render.getWindow().getHeight());

    Raz::Entity& light = world.addEntityWithComponent<Raz::Transform>();
    light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                   Raz::Axis::Forward,          // Direction
                                   3.f,                         // Energy
                                   Raz::ColorPreset::White);    // Color

    Raz::Window& window = world.getSystem<Raz::RenderSystem>().getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    // Enabling wireframe rendering by default; can be disabled from the overlay
    Raz::Renderer::setPolygonMode(Raz::FaceOrientation::FRONT_BACK, Raz::PolygonMode::LINE);

    //////////
    // Mesh //
    //////////

    Raz::Mesh mesh;
    std::vector<Raz::Vertex>& vertices = mesh.addSubmesh().getVertices();
    vertices.reserve(24);

    // Creating a cube made of 6 quad patches, in which vertices are inserted as follows:
    //
    //  1---3
    //  |   |
    //  0---2
    // TODO: this layout works differently for a terrain (see https://github.com/Razakhel/Midgard) from here: the bilinear
    //   interpolations made in the tessellation evaluation shader must be crossed for the clockwiseness to be as expected

    // Right
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(0.57735f, -0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Right, Raz::Axis::Forward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(0.57735f,  0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Right, Raz::Axis::Forward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Right, Raz::Axis::Forward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(0.57735f,  0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Right, Raz::Axis::Forward });

    // Left
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Left, Raz::Axis::Backward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f,  0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Left, Raz::Axis::Backward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Left, Raz::Axis::Backward });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f,  0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Left, Raz::Axis::Backward });

    // Top
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, 0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Up, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, 0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Up, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, 0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Up, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, 0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Up, Raz::Axis::Right });

    // Bottom
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, -0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Down, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Down, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f,  0.57735f), Raz::Vec2f(), Raz::Axis::Down, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Down, Raz::Axis::Left });

    // Front
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f, 0.57735f), Raz::Vec2f(), Raz::Axis::Backward, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f,  0.57735f, 0.57735f), Raz::Vec2f(), Raz::Axis::Backward, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, -0.57735f, 0.57735f), Raz::Vec2f(), Raz::Axis::Backward, Raz::Axis::Right });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f,  0.57735f, 0.57735f), Raz::Vec2f(), Raz::Axis::Backward, Raz::Axis::Right });

    // Back
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Forward, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f( 0.57735f,  0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Forward, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f, -0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Forward, Raz::Axis::Left });
    vertices.emplace_back(Raz::Vertex{ Raz::Vec3f(-0.57735f,  0.57735f, -0.57735f), Raz::Vec2f(), Raz::Axis::Forward, Raz::Axis::Left });

    Raz::Entity& entity = world.addEntity();
    auto& transform     = entity.addComponent<Raz::Transform>();
    auto& meshRenderer  = entity.addComponent<Raz::MeshRenderer>(mesh, Raz::RenderMode::PATCH);

    Raz::Renderer::setPatchVertexCount(4); // The vertex count is 3 by default, but we are using quads here

    ///////////////////////////
    // Tessellation material //
    ///////////////////////////

    Raz::RenderShaderProgram& materialProgram = meshRenderer.getMaterials().front().getProgram();

    materialProgram.setTessellationControlShader(Raz::TessellationControlShader::loadFromSource(tessCtrlSource));
    materialProgram.setTessellationEvaluationShader(Raz::TessellationEvaluationShader::loadFromSource(tessEvalSource));
    materialProgram.link();

    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::PROGRAM, materialProgram.getIndex(), "Tessellation shader program");
      Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, materialProgram.getVertexShader().getIndex(), "Vertex shader");
      Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, materialProgram.getTessellationControlShader().getIndex(), "Tess. control shader");
      Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, materialProgram.getTessellationEvaluationShader().getIndex(), "Tess. eval. shader");
      Raz::Renderer::setLabel(Raz::RenderObjectType::SHADER, materialProgram.getFragmentShader().getIndex(), "Fragment shader");
    }

    /////////////
    // Overlay //
    /////////////

    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Tessellation demo", Raz::Vec2f(-1.f));

    overlay.addSlider("Outer level", [&materialProgram] (float newVal) {
      materialProgram.setAttribute(newVal, "uniTessLevelOuter");
      materialProgram.sendAttributes();
    }, 1.f, 64.f, 4.f);
    overlay.addSlider("Inner level", [&materialProgram] (float newVal) {
      materialProgram.setAttribute(newVal, "uniTessLevelInner");
      materialProgram.sendAttributes();
    }, 1.f, 64.f, 4.f);

    overlay.addCheckbox("Make sphere", [&materialProgram] () {
      materialProgram.setAttribute(true, "uniMakeSphere");
      materialProgram.sendAttributes();
    }, [&materialProgram] () {
      materialProgram.setAttribute(false, "uniMakeSphere");
      materialProgram.sendAttributes();
    }, true);

    overlay.addCheckbox("Enable wireframe", [] () {
      Raz::Renderer::setPolygonMode(Raz::FaceOrientation::FRONT_BACK, Raz::PolygonMode::LINE);
    }, [] () {
      Raz::Renderer::setPolygonMode(Raz::FaceOrientation::FRONT_BACK, Raz::PolygonMode::FILL);
    }, true);

    app.run([&] (const Raz::FrameTimeInfo& timeInfo) {
      transform.rotate(-45.0_deg * timeInfo.deltaTime, Raz::Axis::Up);
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
