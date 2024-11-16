#include "RaZ/RaZ.hpp"

#include "DemoUtils.hpp"

using namespace std::literals;

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(10);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    auto& renderSystem = world.addSystem<Raz::RenderSystem>(1280u, 720u, "RaZ", Raz::WindowSetting::DEFAULT, 2);
    renderSystem.setCubemap(Raz::Cubemap(Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_right.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_left.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_top.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_bottom.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_front.png"),
                                         Raz::ImageFormat::load(RAZ_ROOT "assets/skyboxes/clouds_back.png")));

    Raz::Window& window = renderSystem.getWindow();
    window.setIcon(Raz::ImageFormat::load(RAZ_ROOT "assets/icons/RaZ_logo_128.png"));

    window.addKeyCallback(Raz::Keyboard::F5, [&renderSystem] (float /* deltaTime */) { renderSystem.updateShaders(); });

    window.addKeyCallback(Raz::Keyboard::F11, [&window] (float) noexcept {
      static bool isFullScreen = false;

      if (isFullScreen)
        window.makeWindowed();
      else
        window.makeFullscreen();

      isFullScreen = !isFullScreen;
    }, Raz::Input::ONCE);

    // Allowing to quit the application by pressing the Escape key
    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float /* deltaTime */) noexcept { app.quit(); });
    // Quitting the application when the close button is clicked
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    //////////////////
    // Post effects //
    //////////////////

    Raz::RenderGraph& renderGraph = renderSystem.getRenderGraph();
    Raz::RenderPass& geometryPass = renderSystem.getGeometryPass();

    // For demonstration purposes, a buffer is created here for each process' output. However, this is far from ideal;
    //   always reuse buffers across passes whenever you can, as it may save a lot of memory
    // Note though that a buffer cannot be set as both read & write in the same pass or process
    const auto depthBuffer      = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::DEPTH);
    const auto colorBuffer      = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto gradientBuffer   = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto gradDirBuffer    = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto edgeBuffer       = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::GRAY);
    const auto edgeBlendBuffer  = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto chromAberrBuffer = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto blurredBuffer    = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto vignetteBuffer   = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);
    const auto filmGrainBuffer  = Raz::Texture2D::create(renderSystem.getSceneWidth(), renderSystem.getSceneHeight(), Raz::TextureColorspace::RGB);

#if !defined(USE_OPENGL_ES)
    if (Raz::Renderer::checkVersion(4, 3)) {
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, depthBuffer->getIndex(), "Depth buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, colorBuffer->getIndex(), "Color buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, gradientBuffer->getIndex(), "Gradient buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, gradDirBuffer->getIndex(), "Gradient direction buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, edgeBuffer->getIndex(), "Edge buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, edgeBlendBuffer->getIndex(), "Edge blend buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, chromAberrBuffer->getIndex(), "Chrom. aberr. buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, blurredBuffer->getIndex(), "Blurred buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, vignetteBuffer->getIndex(), "Vignette buffer");
      Raz::Renderer::setLabel(Raz::RenderObjectType::TEXTURE, filmGrainBuffer->getIndex(), "Film grain buffer");
    }
#endif

    geometryPass.setWriteDepthTexture(depthBuffer); // A depth buffer is always needed
    geometryPass.addWriteColorTexture(colorBuffer, 0);

    // Sobel

    auto& sobel = renderGraph.addRenderProcess<Raz::SobelFilterRenderProcess>();
    sobel.setInputBuffer(colorBuffer);
    sobel.setOutputGradientBuffer(gradientBuffer);
    sobel.setOutputGradientDirectionBuffer(gradDirBuffer);
    sobel.addParent(geometryPass);

    // Canny

    // Note that in an ordinary Canny edge detection process, the input of the gradient information computing pass (here Sobel)
    //  should be applied a gaussian blur to attempt removing high frequencies (small details). It's not done here to avoid
    //  computing another blur, as one already is further below

    auto& canny = renderGraph.addRenderProcess<Raz::CannyFilterRenderProcess>();
    canny.setInputGradientBuffer(gradientBuffer);
    canny.setInputGradientDirectionBuffer(gradDirBuffer);
    canny.setOutputBuffer(edgeBuffer);
    canny.addParent(sobel);

    // Edge blend

    auto& edgeBlend = renderGraph.addNode(Raz::FragmentShader::loadFromSource(R"(
      in vec2 fragTexcoords;

      uniform sampler2D uniColorBuffer;
      uniform sampler2D uniEdgeBuffer;
      uniform float uniBlendFactor;

      layout(location = 0) out vec4 fragColor;

      void main() {
        vec3 origColor = texture(uniColorBuffer, fragTexcoords).rgb;
        float edge     = texture(uniEdgeBuffer, fragTexcoords).r;

        vec3 blendedColor = origColor * (1.0 - edge);

        vec3 finalColor = mix(origColor, blendedColor, uniBlendFactor);
        fragColor       = vec4(finalColor, 1.0);
      }
    )"));
    edgeBlend.getProgram().setAttribute(0.f, "uniBlendFactor");
    edgeBlend.getProgram().sendAttributes();
    edgeBlend.addReadTexture(colorBuffer, "uniColorBuffer");
    edgeBlend.addReadTexture(edgeBuffer, "uniEdgeBuffer");
    edgeBlend.addWriteColorTexture(edgeBlendBuffer, 0);
    canny.addChild(edgeBlend);

    // Chromatic aberration

    auto& chromaticAberration = renderGraph.addRenderProcess<Raz::ChromaticAberrationRenderProcess>();
    chromaticAberration.setInputBuffer(edgeBlendBuffer);
    chromaticAberration.setOutputBuffer(chromAberrBuffer);
    chromaticAberration.addParent(edgeBlend);

    // Blur

    auto& boxBlur = renderGraph.addRenderProcess<Raz::BoxBlurRenderProcess>();
    boxBlur.setInputBuffer(chromAberrBuffer);
    boxBlur.setOutputBuffer(blurredBuffer);
    boxBlur.addParent(chromaticAberration);

    // Vignette

    auto& vignette = renderGraph.addRenderProcess<Raz::VignetteRenderProcess>();
    vignette.setInputBuffer(blurredBuffer);
    vignette.setOutputBuffer(vignetteBuffer);
    vignette.addParent(boxBlur);

    // Film grain

    auto& filmGrain = renderGraph.addRenderProcess<Raz::FilmGrainRenderProcess>();
    filmGrain.setInputBuffer(vignetteBuffer);
    filmGrain.setOutputBuffer(filmGrainBuffer);
    filmGrain.addParent(vignette);

    // Pixelization

    auto& pixelization = renderGraph.addRenderProcess<Raz::PixelizationRenderProcess>();
    pixelization.setInputBuffer(filmGrainBuffer);
    pixelization.addParent(filmGrain);

    ////////////
    // Camera //
    ////////////

    Raz::Entity& camera = world.addEntity();
    auto& cameraComp    = camera.addComponent<Raz::Camera>(renderSystem.getSceneWidth(), renderSystem.getSceneHeight());
    auto& cameraTrans   = camera.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));

    DemoUtils::setupCameraControls(camera, window);

    //////////
    // Mesh //
    //////////

    Raz::Entity& mesh = world.addEntity();

    auto [meshData, meshRenderData] = Raz::MeshFormat::load(RAZ_ROOT "assets/meshes/shield.obj");
    auto& meshComp       = mesh.addComponent<Raz::Mesh>(std::move(meshData)); // Useful if you need to keep the geometry
    auto& meshRenderComp = mesh.addComponent<Raz::MeshRenderer>(std::move(meshRenderData)); // Useful if you need to render the mesh

    auto& meshTrans = mesh.addComponent<Raz::Transform>();
    meshTrans.scale(0.2f);

    window.addKeyCallback(Raz::Keyboard::R, [&meshRenderComp] (float /* deltaTime */) noexcept { meshRenderComp.disable(); },
                          Raz::Input::ONCE,
                          [&meshRenderComp] () noexcept { meshRenderComp.enable(); });

    DemoUtils::setupMeshControls(mesh, window);

    ///////////
    // Light //
    ///////////

    Raz::Entity& light = world.addEntity();
    /*auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::POINT,    // Type
                                                     1.f,                      // Energy
                                                     Raz::ColorPreset::White); // Color*/
    auto& lightComp = light.addComponent<Raz::Light>(Raz::LightType::DIRECTIONAL, // Type
                                                     -Raz::Axis::Z,               // Direction
                                                     1.f,                         // Energy
                                                     Raz::ColorPreset::White);    // Color
    auto& lightTrans = light.addComponent<Raz::Transform>(Raz::Vec3f(0.f, 1.f, 0.f));

    DemoUtils::setupLightControls(light, renderSystem, window);
    DemoUtils::setupAddLight(cameraTrans, world, window);

    ///////////////
    // Scripting //
    ///////////////

#if !defined(RAZ_NO_LUA)
    constexpr std::string_view luaScriptSource = R"(
local rotAngle = Degreesf.new(20)

function setup()
    -- 'this' always represents the entity containing the script
    this:getTransform().rotation = Quaternionf.new(-rotAngle, Axis.Y)
end

function update(timeInfo)
    local angle = rotAngle * math.sin(timeInfo.globalTime) * timeInfo.deltaTime
    this:getTransform():rotate(Quaternionf.new(angle, Axis.Y))
end)";

    world.addSystem<Raz::ScriptSystem>();

    auto& luaScript = mesh.addComponent<Raz::LuaScript>(luaScriptSource.data());
#endif

    ///////////
    // Audio //
    ///////////

#if defined(RAZ_USE_AUDIO)
    auto& audio = world.addSystem<Raz::AudioSystem>();

    auto& meshSound = mesh.addComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT "assets/sounds/wave_seagulls.wav"));
    meshSound.setRepeat(true);
    meshSound.setGain(0.f);
    meshSound.play();

    // Adding a Listener component, so that the sound's volume decays over the camera's distance to the source
    // Passing its initial position and/or orientation is optional, since they will be sent every time the camera moves,
    //  but recommended so that the sound instantly takes that information into account
    camera.addComponent<Raz::Listener>(cameraTrans.getPosition(), Raz::Mat3f(cameraTrans.getRotation().computeMatrix()));

    DemoUtils::setupSoundControls(meshSound, window);
    DemoUtils::setupAddSound(cameraTrans, RAZ_ROOT "assets/sounds/wave_seagulls.wav", world, window);
#endif // RAZ_USE_AUDIO

    /////////////
    // Overlay //
    /////////////

#if !defined(RAZ_NO_OVERLAY)
    Raz::OverlayWindow& overlay = window.getOverlay().addWindow("RaZ - Full demo", Raz::Vec2f(-1.f));

    DemoUtils::insertOverlayCameraControlsHelp(overlay);
    overlay.addLabel("Press F11 to toggle fullscreen.");

    overlay.addSeparator();

    DemoUtils::insertOverlayCullingOption(window, overlay);
    DemoUtils::insertOverlayVerticalSyncOption(window, overlay);

#if !defined(RAZ_NO_LUA)
    constexpr float overlayScriptWidth        = 600.f;
    constexpr float overlayScriptSourceHeight = 225.f;
    constexpr float overlayScriptHeight       = overlayScriptSourceHeight + 60.f;

    Raz::OverlayWindow& overlayScript = window.getOverlay().addWindow("Edit script",
                                                                      Raz::Vec2f(overlayScriptWidth, overlayScriptHeight),
                                                                      Raz::Vec2f(static_cast<float>(window.getWidth()) - overlayScriptWidth, 0.f));
    overlayScript.disable();

    Raz::OverlayTextArea& scriptTextArea = overlayScript.addTextArea("Lua script",
                                                                     [] (const std::string&) noexcept {},
                                                                     luaScriptSource.data() + 1,
                                                                     overlayScriptSourceHeight);

    overlayScript.addButton("Apply", [&] () {
      try {
        // Running a dummy script, checking that it's syntactically correct
        Raz::LuaScript testScript(scriptTextArea.getText());
        testScript.registerEntity(mesh, "this");
        testScript.update({});

        luaScript.loadCode(scriptTextArea.getText());
      } catch (const std::exception& ex) {
        Raz::Logger::error("Failed to reload the Lua script:\n" + std::string(ex.what()));
      }
    });

    overlay.addButton("Edit script", [&overlayScript] () noexcept { overlayScript.enable(!overlayScript.isEnabled()); });
#endif

    overlay.addSeparator();

#if defined(RAZ_USE_AUDIO)
    overlay.addSlider("Sound volume", [&meshSound] (float value) noexcept { meshSound.setGain(value); }, 0.f, 1.f, 0.f);
#endif // RAZ_USE_AUDIO

    overlay.addSlider("Edge enhanc. strength",
                      [&edgeBlend] (float value) {
                        edgeBlend.getProgram().setAttribute(value, "uniBlendFactor");
                        edgeBlend.getProgram().sendAttributes();
                      },
                      0.f, 1.f, 0.f);

    overlay.addSlider("Canny lower bound",
                      [&canny] (float value) { canny.setLowerBound(value); },
                      0.f, 1.f, 0.1f);

    overlay.addSlider("Canny upper bound",
                      [&canny] (float value) { canny.setUpperBound(value); },
                      0.f, 1.f, 0.3f);

    overlay.addSlider("Chrom. aberr. strength",
                      [&chromaticAberration] (float value) { chromaticAberration.setStrength(value); },
                      -15.f, 15.f, 0.f);

    overlay.addSlider("Blur strength",
                      [&boxBlur] (float value) { boxBlur.setStrength(static_cast<unsigned int>(value)); },
                      1.f, 16.f, 1.f);

    overlay.addSlider("Vignette strength",
                      [&vignette] (float value) { vignette.setStrength(value); },
                      0.f, 1.f, 0.25f);

    overlay.addSlider("Vignette opacity",
                      [&vignette] (float value) { vignette.setOpacity(value); },
                      0.f, 1.f, 1.f);

    overlay.addColorPicker("Vignette color",
                           [&vignette] (const Raz::Color& color) { vignette.setColor(color); },
                           Raz::ColorPreset::Black);

    overlay.addSlider("Film grain strength",
                      [&filmGrain] (float value) { filmGrain.setStrength(value); },
                      0.f, 1.f, 0.05f);

    overlay.addSlider("Pixelization strength",
                      [&pixelization] (float value) { pixelization.setStrength(value); },
                      0.f, 1.f, 0.f);

#if !defined(USE_OPENGL_ES) // GPU timing capabilities are not available with OpenGL ES
    overlay.addSeparator();

    Raz::OverlayPlot& plot = overlay.addPlot("Profiler", 100, {}, "Time (ms)", 0.f, 100.f, false, 200.f);
    Raz::OverlayPlotEntry& geomPlot         = plot.addEntry("Geometry");
    Raz::OverlayPlotEntry& sobelPlot        = plot.addEntry("Sobel");
    Raz::OverlayPlotEntry& cannyPlot        = plot.addEntry("Canny");
    Raz::OverlayPlotEntry& chromAberrPlot   = plot.addEntry("Chrom. aberr.");
    Raz::OverlayPlotEntry& blurPlot         = plot.addEntry("Blur");
    Raz::OverlayPlotEntry& vignettePlot     = plot.addEntry("Vignette");
    Raz::OverlayPlotEntry& filmGrainPlot    = plot.addEntry("Film grain");
    Raz::OverlayPlotEntry& pixelizationPlot = plot.addEntry("Pixelization");
#endif // USE_OPENGL_ES

    overlay.addSeparator();

    DemoUtils::insertOverlayFrameSpeed(overlay);
#endif // RAZ_NO_OVERLAY

    //////////////////////////
    // Starting application //
    //////////////////////////

#if !defined(USE_OPENGL_ES)
    app.run([&] (const Raz::FrameTimeInfo&) {
      geomPlot.push(geometryPass.recoverElapsedTime());
      sobelPlot.push(sobel.recoverElapsedTime());
      cannyPlot.push(canny.recoverElapsedTime());
      chromAberrPlot.push(chromaticAberration.recoverElapsedTime());
      blurPlot.push(boxBlur.recoverElapsedTime());
      vignettePlot.push(vignette.recoverElapsedTime());
      filmGrainPlot.push(filmGrain.recoverElapsedTime());
      pixelizationPlot.push(pixelization.recoverElapsedTime());
    });
#else
    app.run();
#endif
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
