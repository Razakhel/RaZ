#include "RaZ/Render/Renderer.hpp"

#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaRender Camera", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local camera = Camera.new()
    camera       = Camera.new(1, 1)
    camera       = Camera.new(1, 1, Radiansf.new(Degreesf.new(45)))
    camera       = Camera.new(1, 1, Radiansf.new(Degreesf.new(45)), 0.1)
    camera       = Camera.new(1, 1, Radiansf.new(Degreesf.new(45)), 0.1, 1000)
    camera       = Camera.new(1, 1, Radiansf.new(Degreesf.new(45)), 0.1, 1000, ProjectionType.PERSPECTIVE)

    camera.fieldOfView = Radiansf.new(1)
    camera.orthographicBound = 2
    camera.cameraType = CameraType.LOOK_AT
    assert(camera.fieldOfView == Radiansf.new(1))
    assert(camera.orthographicBound == 2)
    assert(camera.cameraType == CameraType.LOOK_AT)

    camera:resizeViewport(1, 1)

    camera:setTarget(Vec3f.new(1))
    assert(camera:computeViewMatrix(Transform.new()) == camera:getViewMatrix())
    assert(camera:computeLookAt(Vec3f.new()) == camera:getViewMatrix())
    assert(camera:computeInverseViewMatrix() == camera:getInverseViewMatrix())

    camera:setProjectionType(ProjectionType.ORTHOGRAPHIC)
    assert(camera:computeProjectionMatrix() == camera:computeOrthographicMatrix())
    assert(camera:computePerspectiveMatrix() == camera:getProjectionMatrix())
    assert(camera:computeInverseProjectionMatrix() == camera:getInverseProjectionMatrix())

    assert(camera:unproject(Vec2f.new(0.5)) == Vec3f.new(0.0545324, 0.1600594, 0.1317835))
    assert(camera:unproject(Vec3f.new(0.5)) == Vec3f.new(0.1090431, 0.3200549, 0.2635144))
    assert(camera:unproject(Vec4f.new(0.5)) == Vec3f.new(-31.973266, 1023.4787, 740.6711))
  )"));
}

TEST_CASE("LuaRender Cubemap", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local cubemap = Cubemap.new()
    cubemap       = Cubemap.new(Image.new(1, 1, ImageColorspace.GRAY), Image.new(1, 1, ImageColorspace.GRAY_ALPHA),
                                Image.new(1, 1, ImageColorspace.RGB), Image.new(1, 1, ImageColorspace.RGBA),
                                Image.new(1, 1, ImageColorspace.SRGB), Image.new(1, 1, ImageColorspace.SRGBA))

    assert(cubemap:getIndex() >= 0)
    assert(cubemap:getProgram() ~= nil)

    cubemap:load(Image.new(1, 1, ImageColorspace.GRAY), Image.new(1, 1, ImageColorspace.GRAY_ALPHA),
                 Image.new(1, 1, ImageColorspace.RGB), Image.new(1, 1, ImageColorspace.RGBA),
                 Image.new(1, 1, ImageColorspace.SRGB), Image.new(1, 1, ImageColorspace.SRGBA))

    cubemap:bind()
    cubemap:draw()
    cubemap:unbind()
  )"));
}

TEST_CASE("LuaRender Framebuffer", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local framebuffer = Framebuffer.new()

    assert(framebuffer:getIndex() ~= 0)
    framebuffer:setDepthBuffer(Texture2D.create(TextureColorspace.DEPTH))
    assert(framebuffer:hasDepthBuffer())
    assert(framebuffer:getDepthBuffer() ~= nil)
    framebuffer:addColorBuffer(Texture2D.create(TextureColorspace.RGB), 0)
    assert(framebuffer:getColorBufferCount() == 1)
    assert(framebuffer:getColorBuffer(0) ~= nil)
    assert(not framebuffer:isEmpty())
    framebuffer:resizeBuffers(1, 1)
    framebuffer:mapBuffers()
    framebuffer:bind()
    framebuffer:unbind()
    framebuffer:display()
    framebuffer:removeTextureBuffer(Texture2D.create(TextureColorspace.RGB))
    framebuffer:clearDepthBuffer()
    framebuffer:clearColorBuffers()
    framebuffer:clearTextureBuffers()
    assert(framebuffer.recoverVertexShader() ~= nil)
  )"));
}

TEST_CASE("LuaRender GraphicObjects", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local indexBuffer = IndexBuffer.new()
    assert(indexBuffer:getIndex() ~= 0)
    indexBuffer:bind()
    indexBuffer:unbind()
    assert(indexBuffer.lineIndexCount == 0)
    assert(indexBuffer.triangleIndexCount == 0)

    local vertexArray = VertexArray.new()
    assert(vertexArray:getIndex() ~= 0)
    vertexArray:bind()
    vertexArray:unbind()

    local vertexBuffer = VertexBuffer.new()
    assert(vertexBuffer:getIndex() ~= 0)
    vertexBuffer:bind()
    vertexBuffer:unbind()
    assert(vertexBuffer.vertexCount == 0)
  )"));
}

TEST_CASE("LuaRender Light", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local light = Light.new(LightType.POINT, 1)
    light       = Light.new(LightType.POINT, 1, ColorPreset.White)
    light       = Light.new(LightType.DIRECTIONAL, Axis.Z, 1)
    light       = Light.new(LightType.DIRECTIONAL, Axis.Z, 1, ColorPreset.White)
    light       = Light.new(LightType.SPOT, Axis.Z, 1, Radiansf.new(0))
    light       = Light.new(LightType.SPOT, Axis.Z, 1, Radiansf.new(0), ColorPreset.White)

    light.type      = LightType.POINT
    light.direction = Axis.X
    light.energy    = 10
    light.color     = ColorPreset.Black
    light.angle     = Radiansf.new(Constant.Pi)
    assert(light.type == LightType.POINT)
    assert(light.direction == Axis.X)
    assert(light.energy == 10)
    assert(light.color == ColorPreset.Black)
    assert(light.angle == Radiansf.new(Constant.Pi))
  )"));
}

TEST_CASE("LuaRender Material", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local material = Material.new()
    material       = Material.new(MaterialType.BLINN_PHONG)

    assert(material:getProgram() ~= nil)
    assert(not material:isEmpty())
    assert(material:clone() ~= material)
    material:loadType(MaterialType.COOK_TORRANCE)
  )"));
}

TEST_CASE("LuaRender MeshRenderer", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local meshRenderer = MeshRenderer.new()
    meshRenderer       = MeshRenderer.new(Mesh.new())
    meshRenderer       = MeshRenderer.new(Mesh.new(), RenderMode.TRIANGLE)

    assert(meshRenderer:isEnabled())
    meshRenderer:enable()
    meshRenderer:enable(true)
    meshRenderer:disable()
    meshRenderer:setRenderMode(RenderMode.TRIANGLE, Mesh.new())
    assert(meshRenderer:setMaterial(Material.new()) ~= nil)
    assert(meshRenderer:addMaterial() ~= nil)
    assert(meshRenderer:addMaterial(Material.new()) ~= nil)
    meshRenderer:removeMaterial(0)
    assert(#meshRenderer:getMaterials() == 2)
    meshRenderer:draw() -- Adding an empty SubmeshRenderer doesn't set its draw function; drawing before adding one
    assert(meshRenderer:addSubmeshRenderer() ~= nil)
    assert(meshRenderer:addSubmeshRenderer(Submesh.new()) ~= nil)
    assert(meshRenderer:addSubmeshRenderer(Submesh.new(), RenderMode.TRIANGLE) ~= nil)
    assert(#meshRenderer:getSubmeshRenderers() == 3)
    assert(meshRenderer:clone() ~= meshRenderer)
    meshRenderer:load(Mesh.new())
    meshRenderer:load(Mesh.new(), RenderMode.POINT)
    meshRenderer:loadMaterials()
  )"));
}

#if !defined(RAZ_NO_OVERLAY)
TEST_CASE("LuaRender Overlay", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local overlay = Overlay.new()
    overlay:addWindow("Test")
    overlay:addWindow("Test", Vec2f.new(-1))
    overlay:addWindow("Test", Vec2f.new(-1), Vec2f.new(-1))
    assert(not overlay:isEmpty())
    assert(not overlay:hasKeyboardFocus())
    assert(not overlay:hasMouseFocus())
    overlay:render()

    local overlayWindow = OverlayWindow.new("Test")
    overlayWindow       = OverlayWindow.new("Test", Vec2f.new(-1))
    overlayWindow       = OverlayWindow.new("Test", Vec2f.new(-1), Vec2f.new(-1))
    overlayWindow:enable()
    overlayWindow:enable(true)
    overlayWindow:disable()
    assert(not overlayWindow:isEnabled())
    overlayWindow:render()

    local overlayButton = OverlayButton.new("", function () end)
    overlayButton       = overlayWindow:addButton("", function () end)
    assert(overlayButton:getType() == OverlayElementType.BUTTON)
    overlayButton:enable()
    overlayButton:enable(true)
    overlayButton:disable()
    assert(not overlayButton:isEnabled())

    local overlayCheckbox = OverlayCheckbox.new("", function () end, function () end, true)
    overlayCheckbox       = overlayWindow:addCheckbox("", function () end, function () end, false)
    assert(overlayCheckbox:getType() == OverlayElementType.CHECKBOX)
    overlayCheckbox:enable()
    overlayCheckbox:enable(true)
    overlayCheckbox:disable()
    assert(not overlayCheckbox:isEnabled())

    local overlayColoredLabel = OverlayColoredLabel.new("", ColorPreset.White)
    overlayColoredLabel       = OverlayColoredLabel.new("", ColorPreset.White, 1)
    overlayColoredLabel       = overlayWindow:addColoredLabel("", ColorPreset.Black)
    overlayColoredLabel       = overlayWindow:addColoredLabel("", ColorPreset.Black, 1)
    assert(overlayColoredLabel:getType() == OverlayElementType.COLORED_LABEL)
    overlayColoredLabel:enable()
    overlayColoredLabel:enable(true)
    overlayColoredLabel:disable()
    assert(not overlayColoredLabel:isEnabled())
    overlayColoredLabel.text  = "label"
    overlayColoredLabel.color = ColorPreset.Gray
    overlayColoredLabel.alpha = 0
    assert(overlayColoredLabel.text == "label")
    assert(overlayColoredLabel.color == ColorPreset.Gray)
    assert(overlayColoredLabel.alpha == 0)

    local overlayColorPicker = OverlayColorPicker.new("", function () end, ColorPreset.Blue)
    overlayColorPicker       = overlayWindow:addColorPicker("", function () end, ColorPreset.Red)
    assert(overlayColorPicker:getType() == OverlayElementType.COLOR_PICKER)
    overlayColorPicker:enable()
    overlayColorPicker:enable(true)
    overlayColorPicker:disable()
    assert(not overlayColorPicker:isEnabled())

    local overlayDropdown = OverlayDropdown.new("", { "1", "2", "3" }, function () end)
    overlayDropdown       = OverlayDropdown.new("", { "1", "2", "3" }, function () end, 0)
    overlayDropdown       = overlayWindow:addDropdown("", { "1", "2", "3" }, function () end)
    overlayDropdown       = overlayWindow:addDropdown("", { "1", "2", "3" }, function () end, 0)
    assert(overlayDropdown:getType() == OverlayElementType.DROPDOWN)
    overlayDropdown:enable()
    overlayDropdown:enable(true)
    overlayDropdown:disable()
    assert(not overlayDropdown:isEnabled())

    local overlayFpsCounter = OverlayFpsCounter.new("")
    overlayFpsCounter       = overlayWindow:addFpsCounter("")
    assert(overlayFpsCounter:getType() == OverlayElementType.FPS_COUNTER)
    overlayFpsCounter:enable()
    overlayFpsCounter:enable(true)
    overlayFpsCounter:disable()
    assert(not overlayFpsCounter:isEnabled())

    local overlayFrameTime = OverlayFrameTime.new("")
    overlayFrameTime       = overlayWindow:addFrameTime("")
    assert(overlayFrameTime:getType() == OverlayElementType.FRAME_TIME)
    overlayFrameTime:enable()
    overlayFrameTime:enable(true)
    overlayFrameTime:disable()
    assert(not overlayFrameTime:isEnabled())

    local overlayLabel = OverlayLabel.new("")
    overlayLabel       = overlayWindow:addLabel("")
    assert(overlayLabel:getType() == OverlayElementType.LABEL)
    overlayLabel:enable()
    overlayLabel:enable(true)
    overlayLabel:disable()
    assert(not overlayLabel:isEnabled())
    overlayLabel.text = "label"
    assert(overlayLabel.text == "label")

    local overlayListBox = OverlayListBox.new("", { "1", "2", "3" }, function () end)
    overlayListBox       = OverlayListBox.new("", { "1", "2", "3" }, function () end, 0)
    overlayListBox       = overlayWindow:addListBox("", { "1", "2", "3" }, function () end)
    overlayListBox       = overlayWindow:addListBox("", { "1", "2", "3" }, function () end, 0)
    assert(overlayListBox:getType() == OverlayElementType.LIST_BOX)
    overlayListBox:enable()
    overlayListBox:enable(true)
    overlayListBox:disable()
    assert(not overlayListBox:isEnabled())

    local overlayPlot = OverlayPlot.new("", 10)
    overlayPlot       = OverlayPlot.new("", 10, "")
    overlayPlot       = OverlayPlot.new("", 10, "", "")
    overlayPlot       = OverlayPlot.new("", 10, "", "", -1)
    overlayPlot       = OverlayPlot.new("", 10, "", "", -1, 1)
    overlayPlot       = OverlayPlot.new("", 10, "", "", -1, 1, true)
    overlayPlot       = OverlayPlot.new("", 10, "", "", -1, 1, true, -1)
    overlayPlot       = overlayWindow:addPlot("", 10)
    overlayPlot       = overlayWindow:addPlot("", 10, "")
    overlayPlot       = overlayWindow:addPlot("", 10, "", "")
    overlayPlot       = overlayWindow:addPlot("", 10, "", "", -1)
    overlayPlot       = overlayWindow:addPlot("", 10, "", "", -1, 1)
    overlayPlot       = overlayWindow:addPlot("", 10, "", "", -1, 1, true)
    overlayPlot       = overlayWindow:addPlot("", 10, "", "", -1, 1, true, -1)
    assert(overlayPlot:getType() == OverlayElementType.PLOT)
    overlayPlot:enable()
    overlayPlot:enable(true)
    overlayPlot:disable()
    assert(not overlayPlot:isEnabled())

    local overlayPlotEntry = overlayPlot:addEntry("")
    overlayPlotEntry       = overlayPlot:addEntry("", OverlayPlotType.SHADED)
    overlayPlotEntry:push(1)

    local overlayProgressBar = OverlayProgressBar.new(0, 1)
    overlayProgressBar       = OverlayProgressBar.new(0, 1, true)
    overlayProgressBar       = overlayWindow:addProgressBar(0, 1)
    overlayProgressBar       = overlayWindow:addProgressBar(0, 1, false)
    assert(overlayProgressBar:getType() == OverlayElementType.PROGRESS_BAR)
    overlayProgressBar:enable()
    overlayProgressBar:enable(true)
    overlayProgressBar:disable()
    assert(not overlayProgressBar:isEnabled())
    overlayProgressBar.currentValue = 1
    overlayProgressBar:add(4)
    assert(overlayProgressBar.currentValue == 5)
    assert(overlayProgressBar:hasStarted())
    assert(overlayProgressBar:hasFinished())

    local overlaySeparator = OverlaySeparator.new()
    overlaySeparator       = overlayWindow:addSeparator()
    assert(overlaySeparator:getType() == OverlayElementType.SEPARATOR)
    overlaySeparator:enable()
    overlaySeparator:enable(true)
    overlaySeparator:disable()
    assert(not overlaySeparator:isEnabled())

    local overlaySlider = OverlaySlider.new("", function () end, 0, 1, 0.5)
    overlaySlider       = overlayWindow:addSlider("", function () end, 0, 1, 0.5)
    assert(overlaySlider:getType() == OverlayElementType.SLIDER)
    overlaySlider:enable()
    overlaySlider:enable(true)
    overlaySlider:disable()
    assert(not overlaySlider:isEnabled())

    local overlayTextArea = OverlayTextArea.new("", function () end)
    overlayTextArea       = OverlayTextArea.new("", function () end, "init")
    overlayTextArea       = OverlayTextArea.new("", function () end, "init", -1.0)
    overlayTextArea       = overlayWindow:addTextArea("", function () end)
    overlayTextArea       = overlayWindow:addTextArea("", function () end, "init")
    overlayTextArea       = overlayWindow:addTextArea("", function () end, "init", -1.0)
    assert(overlayTextArea:getType() == OverlayElementType.TEXT_AREA)
    overlayTextArea:enable()
    overlayTextArea:enable(true)
    overlayTextArea:disable()
    assert(not overlayTextArea:isEnabled())
    overlayTextArea.text = "text"
    overlayTextArea:append(" test")
    assert(overlayTextArea.text == "text test")
    overlayTextArea:clear()

    local overlayTextbox = OverlayTextbox.new("", function () end)
    overlayTextbox       = OverlayTextbox.new("", function () end, "init")
    overlayTextbox       = overlayWindow:addTextbox("", function () end)
    overlayTextbox       = overlayWindow:addTextbox("", function () end, "init")
    assert(overlayTextbox:getType() == OverlayElementType.TEXTBOX)
    overlayTextbox:enable()
    overlayTextbox:enable(true)
    overlayTextbox:disable()
    assert(not overlayTextbox:isEnabled())
    overlayTextbox.text = "text"
    overlayTextbox:append(" test")
    assert(overlayTextbox.text == "text test")
    overlayTextbox:clear()

    local overlayTexture = OverlayTexture.new(Texture2D.new(TextureColorspace.GRAY), 1, 1)
    overlayTexture       = OverlayTexture.new(Texture2D.new(TextureColorspace.GRAY))
    overlayTexture       = overlayWindow:addTexture(Texture2D.new(TextureColorspace.GRAY), 1, 1)
    overlayTexture       = overlayWindow:addTexture(Texture2D.new(TextureColorspace.GRAY))
    assert(overlayTexture:getType() == OverlayElementType.TEXTURE)
    overlayTexture:enable()
    overlayTexture:enable(true)
    overlayTexture:disable()
    assert(not overlayTexture:isEnabled())
    overlayTexture:setTexture(Texture2D.new(TextureColorspace.GRAY), 1, 1)
    overlayTexture:setTexture(Texture2D.new(TextureColorspace.GRAY))
  )"));
}
#endif

TEST_CASE("LuaRender RenderGraph", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local renderGraph = RenderGraph.new()

    renderGraph:addNode()
    renderGraph:addNode(FragmentShader.new())
    renderGraph:addNode(FragmentShader.new(), "")
    renderGraph:removeNode(renderGraph:getNode(0))
    assert(renderGraph:getNodeCount() == 2)

    assert(renderGraph:isValid())
    assert(renderGraph:getGeometryPass() ~= nil)
    assert(renderGraph:addBloomRenderProcess() ~= nil)
    assert(renderGraph:addBoxBlurRenderProcess() ~= nil)
    assert(renderGraph:addChromaticAberrationRenderProcess() ~= nil)
    assert(renderGraph:addConvolutionRenderProcess(Mat3f.identity()) ~= nil)
    assert(renderGraph:addConvolutionRenderProcess(Mat3f.identity(), "") ~= nil)
    assert(renderGraph:addFilmGrainRenderProcess() ~= nil)
    assert(renderGraph:addGaussianBlurRenderProcess() ~= nil)
    assert(renderGraph:addPixelizationRenderProcess() ~= nil)
    assert(renderGraph:addScreenSpaceReflectionsRenderProcess() ~= nil)
    assert(renderGraph:addSobelFilterRenderProcess() ~= nil)
    assert(renderGraph:addVignetteRenderProcess() ~= nil)
    renderGraph:resizeViewport(1, 1)
    renderGraph:updateShaders()
  )"));
}

TEST_CASE("LuaRender RenderPass", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local renderPass  = RenderPass.new()
    local renderPass2 = RenderPass.new()
    local renderPass3 = RenderPass.new()

    renderPass3:addParents(renderPass, renderPass2)
    renderPass:addChildren(renderPass2, renderPass3)
    assert(renderPass3:getParentCount() == 2)
    assert(renderPass3:getParent(0) == renderPass)
    assert(renderPass:getChildCount() == 2)
    assert(renderPass:getChild(0) == renderPass3)
    assert(renderPass:isRoot())
    assert(renderPass3:isLeaf())
    renderPass2:removeParents(renderPass, renderPass3)
    renderPass2:removeChildren(renderPass, renderPass3)
    assert(renderPass2:isIsolated())

    renderPass:setName("pass")
    assert(renderPass:getName() == "pass")
    renderPass:setProgram(RenderShaderProgram.new())
    assert(renderPass:getProgram() ~= nil)
    renderPass:enable()
    renderPass:enable(true)
    renderPass:disable()
    assert(not renderPass:isEnabled())
    assert(renderPass:isValid())
    renderPass:addReadTexture(Texture2D.create(TextureColorspace.GRAY), "tex2D")
    renderPass:addReadTexture(Texture3D.create(TextureColorspace.RGBA), "tex3D")
    assert(renderPass:hasReadTexture("tex2D"))
    assert(renderPass:getReadTexture("tex2D") ~= nil)
    renderPass:removeReadTexture(renderPass:getReadTexture(0))
    assert(renderPass:getReadTextureCount() == 1)
    renderPass:clearReadTextures()
    assert(renderPass:getFramebuffer() ~= nil)
    renderPass:setWriteDepthTexture(Texture2D.create(TextureColorspace.DEPTH))
    renderPass:addWriteColorTexture(Texture2D.create(TextureColorspace.RGB), 0)
    renderPass:removeWriteTexture(Texture2D.create(TextureColorspace.GRAY))
    renderPass:clearWriteTextures()
    renderPass:resizeWriteBuffers(1, 1)
    renderPass:execute()
    assert(renderPass:recoverElapsedTime() >= 0)
  )"));

#if !defined(USE_OPENGL_ES)
  CHECK(TestUtils::executeLuaScript(R"(
    local renderPass = RenderPass.new()
    renderPass:addReadTexture(Texture1D.create(TextureColorspace.RG), "tex1D")
    assert(renderPass:hasReadTexture("tex1D"))
  )"));
#endif
}

TEST_CASE("LuaRender RenderProcess", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local renderGraph = RenderGraph.new()
    local renderPass  = RenderPass.new()

    local bloomRenderProcess = BloomRenderProcess.new(renderGraph)
    bloomRenderProcess:setState(true)
    bloomRenderProcess:enable()
    bloomRenderProcess:disable()
    assert(not bloomRenderProcess:isEnabled())
    bloomRenderProcess:addParent(renderPass)
    bloomRenderProcess:addParent(BloomRenderProcess.new(renderGraph))
    bloomRenderProcess:addChild(renderPass)
    bloomRenderProcess:addChild(BloomRenderProcess.new(renderGraph))
    bloomRenderProcess:resizeBuffers(1, 1)
    assert(bloomRenderProcess:recoverElapsedTime() >= 0)
    assert(bloomRenderProcess:getThresholdPass() ~= nil)
    assert(bloomRenderProcess:getDownscalePassCount() == bloomRenderProcess:getDownscaleBufferCount())
    assert(bloomRenderProcess:getDownscalePass(0) ~= nil)
    assert(bloomRenderProcess:getDownscaleBuffer(0) ~= nil)
    assert(bloomRenderProcess:getUpscalePassCount() == bloomRenderProcess:getUpscaleBufferCount())
    assert(bloomRenderProcess:getUpscalePass(0) ~= nil)
    assert(bloomRenderProcess:getUpscaleBuffer(0) ~= nil)
    bloomRenderProcess:setInputColorBuffer(Texture2D.create(TextureColorspace.RGB))
    bloomRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.RGB))
    bloomRenderProcess:setThresholdValue(0)

    local boxBlurRenderProcess = BoxBlurRenderProcess.new(renderGraph)
    boxBlurRenderProcess:setState(true)
    boxBlurRenderProcess:enable()
    boxBlurRenderProcess:disable()
    assert(not boxBlurRenderProcess:isEnabled())
    boxBlurRenderProcess:addParent(renderPass)
    boxBlurRenderProcess:addParent(BoxBlurRenderProcess.new(renderGraph))
    boxBlurRenderProcess:addChild(renderPass)
    boxBlurRenderProcess:addChild(BoxBlurRenderProcess.new(renderGraph))
    boxBlurRenderProcess:resizeBuffers(1, 1)
    assert(boxBlurRenderProcess:recoverElapsedTime() >= 0)
    boxBlurRenderProcess:setInputBuffer(Texture2D.create(TextureColorspace.GRAY))
    boxBlurRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.GRAY))
    boxBlurRenderProcess:setStrength(0)

    local chromAberrRenderProcess = ChromaticAberrationRenderProcess.new(renderGraph)
    chromAberrRenderProcess:setState(true)
    chromAberrRenderProcess:enable()
    chromAberrRenderProcess:disable()
    assert(not chromAberrRenderProcess:isEnabled())
    chromAberrRenderProcess:addParent(renderPass)
    chromAberrRenderProcess:addParent(ChromaticAberrationRenderProcess.new(renderGraph))
    chromAberrRenderProcess:addChild(renderPass)
    chromAberrRenderProcess:addChild(ChromaticAberrationRenderProcess.new(renderGraph))
    chromAberrRenderProcess:resizeBuffers(1, 1)
    assert(chromAberrRenderProcess:recoverElapsedTime() >= 0)
    chromAberrRenderProcess:setInputBuffer(Texture2D.create(TextureColorspace.RGB))
    chromAberrRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.RGB))
    chromAberrRenderProcess:setStrength(0)
    chromAberrRenderProcess:setDirection(Vec2f.new(1))
    chromAberrRenderProcess:setMaskTexture(Texture2D.create(TextureColorspace.GRAY))

    local convolutionRenderProcess = ConvolutionRenderProcess.new(renderGraph, Mat3f.identity())
    convolutionRenderProcess       = ConvolutionRenderProcess.new(renderGraph, Mat3f.identity(), "")
    convolutionRenderProcess:setState(true)
    convolutionRenderProcess:enable()
    convolutionRenderProcess:disable()
    assert(not convolutionRenderProcess:isEnabled())
    convolutionRenderProcess:addParent(renderPass)
    convolutionRenderProcess:addParent(ConvolutionRenderProcess.new(renderGraph, Mat3f.identity()))
    convolutionRenderProcess:addChild(renderPass)
    convolutionRenderProcess:addChild(ConvolutionRenderProcess.new(renderGraph, Mat3f.identity()))
    convolutionRenderProcess:resizeBuffers(1, 1)
    assert(convolutionRenderProcess:recoverElapsedTime() >= 0)
    convolutionRenderProcess:setInputBuffer(Texture2D.create(TextureColorspace.RGB))
    convolutionRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.RGB))
    convolutionRenderProcess:setKernel(Mat3f.identity())

    local gaussianBlurRenderProcess = GaussianBlurRenderProcess.new(renderGraph)
    gaussianBlurRenderProcess:setState(true)
    gaussianBlurRenderProcess:enable()
    gaussianBlurRenderProcess:disable()
    assert(not gaussianBlurRenderProcess:isEnabled())
    gaussianBlurRenderProcess:addParent(renderPass)
    gaussianBlurRenderProcess:addParent(GaussianBlurRenderProcess.new(renderGraph))
    gaussianBlurRenderProcess:addChild(renderPass)
    gaussianBlurRenderProcess:addChild(GaussianBlurRenderProcess.new(renderGraph))
    gaussianBlurRenderProcess:resizeBuffers(1, 1)
    assert(gaussianBlurRenderProcess:recoverElapsedTime() >= 0)
    assert(gaussianBlurRenderProcess:getHorizontalPass() ~= nil)
    assert(gaussianBlurRenderProcess:getVerticalPass() ~= nil)
    gaussianBlurRenderProcess:setInputBuffer(Texture2D.create(TextureColorspace.GRAY))
    gaussianBlurRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.GRAY))

    local ssrRenderProcess = ScreenSpaceReflectionsRenderProcess.new(renderGraph)
    ssrRenderProcess:setState(true)
    ssrRenderProcess:enable()
    ssrRenderProcess:disable()
    assert(not ssrRenderProcess:isEnabled())
    ssrRenderProcess:addParent(renderPass)
    ssrRenderProcess:addParent(ScreenSpaceReflectionsRenderProcess.new(renderGraph))
    ssrRenderProcess:addChild(renderPass)
    ssrRenderProcess:addChild(ScreenSpaceReflectionsRenderProcess.new(renderGraph))
    ssrRenderProcess:resizeBuffers(1, 1)
    assert(ssrRenderProcess:recoverElapsedTime() >= 0)
    ssrRenderProcess:setInputDepthBuffer(Texture2D.create(TextureColorspace.DEPTH))
    ssrRenderProcess:setInputColorBuffer(Texture2D.create(TextureColorspace.RGB))
    ssrRenderProcess:setInputBlurredColorBuffer(Texture2D.create(TextureColorspace.RGB))
    ssrRenderProcess:setInputNormalBuffer(Texture2D.create(TextureColorspace.RGB))
    ssrRenderProcess:setInputSpecularBuffer(Texture2D.create(TextureColorspace.RGBA))
    ssrRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.GRAY))

    local vignetteRenderProcess = VignetteRenderProcess.new(renderGraph)
    vignetteRenderProcess:setState(true)
    vignetteRenderProcess:enable()
    vignetteRenderProcess:disable()
    assert(not vignetteRenderProcess:isEnabled())
    vignetteRenderProcess:addParent(renderPass)
    vignetteRenderProcess:addParent(VignetteRenderProcess.new(renderGraph))
    vignetteRenderProcess:addChild(renderPass)
    vignetteRenderProcess:addChild(VignetteRenderProcess.new(renderGraph))
    vignetteRenderProcess:resizeBuffers(1, 1)
    assert(vignetteRenderProcess:recoverElapsedTime() >= 0)
    vignetteRenderProcess:setInputBuffer(Texture2D.create(TextureColorspace.GRAY))
    vignetteRenderProcess:setOutputBuffer(Texture2D.create(TextureColorspace.GRAY))
    vignetteRenderProcess:setStrength(0)
    vignetteRenderProcess:setOpacity(1)
    vignetteRenderProcess:setColor(ColorPreset.Black)
  )"));
}

TEST_CASE("LuaRender RenderSystem", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local renderSystem = RenderSystem.new()
    renderSystem       = RenderSystem.new(1, 1)

    assert(renderSystem:getSceneWidth() == 1)
    assert(renderSystem:getSceneHeight() == 1)
    assert(renderSystem:getGeometryPass() ~= nil)
    assert(renderSystem:getRenderGraph() ~= nil)

    local cubemapImg = Image.new(1, 1, ImageColorspace.GRAY)
    renderSystem:setCubemap(Cubemap.new(cubemapImg, cubemapImg, cubemapImg, cubemapImg, cubemapImg, cubemapImg))
    assert(renderSystem:hasCubemap())
    assert(renderSystem:getCubemap() ~= nil)
    renderSystem:removeCubemap()

    renderSystem:resizeViewport(1, 1)
    renderSystem:updateLights()
    renderSystem:updateShaders()
    renderSystem:updateMaterials()
    renderSystem:updateMaterials(MeshRenderer.new())
    renderSystem:saveToImage(FilePath.new("téstÊxpørt.jpg"))
    renderSystem:saveToImage(FilePath.new("téstÊxpørt.hdr"), TextureFormat.DEPTH)
    renderSystem:saveToImage(FilePath.new("téstÊxpørt.png"), TextureFormat.RGBA, PixelDataType.UBYTE)

    assert(renderSystem:getAcceptedComponents() ~= nil)
    assert(not renderSystem:containsEntity(Entity.new(0)))
    -- RenderSystem::update() is not tested, as it requires a specific setup
    --assert(renderSystem:update(FrameTimeInfo.new()))
    renderSystem:destroy()
  )"));

#if !defined(RAZ_NO_WINDOW)
  CHECK(TestUtils::executeLuaScript(R"(
    local renderSystem = RenderSystem.new(1, 1, "", WindowSetting.INVISIBLE)
    renderSystem       = RenderSystem.new(1, 1, "", WindowSetting.INVISIBLE, 1)

    assert(renderSystem:hasWindow())
    assert(renderSystem:getWindow() ~= nil)
    -- The functions with less arguments are not tested to avoid spawning multiple windows
    renderSystem:createWindow(1, 1, "Test", WindowSetting.INVISIBLE)
    renderSystem:createWindow(1, 1, "Test", WindowSetting.INVISIBLE, 1)
  )"));
#endif

  // Enabling XR rendering cannot be tested, as it requires having available XR device & runtime
}

TEST_CASE("LuaRender Shader", "[script][lua][render]") {
  if (Raz::Renderer::checkVersion(4, 3) || Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
    CHECK(TestUtils::executeLuaScript(R"(
      local computeShader = ComputeShader.new()
      computeShader       = ComputeShader.new(FilePath.new())
      computeShader       = ComputeShader.loadFromSource("")

      assert(computeShader:isValid())
      computeShader:import(FilePath.new())
      assert(computeShader:getPath():isEmpty())
      computeShader:load()
      computeShader:compile()
      computeShader:isCompiled()
      assert(computeShader:clone() ~= computeShader)
      computeShader:destroy()
    )"));
  }

  if (!Raz::Renderer::checkVersion(4, 0) && !Raz::Renderer::isExtensionSupported("GL_ARB_tessellation_shader")) {
    CHECK(TestUtils::executeLuaScript(R"(
      local tessCtrlShader = TessellationControlShader.new()
      tessCtrlShader       = TessellationControlShader.new(FilePath.new())
      tessCtrlShader       = TessellationControlShader.loadFromSource("")

      assert(tessCtrlShader:isValid())
      tessCtrlShader:import(FilePath.new())
      assert(tessCtrlShader:getPath():isEmpty())
      tessCtrlShader:load()
      tessCtrlShader:compile()
      tessCtrlShader:isCompiled()
      assert(tessCtrlShader:clone() ~= tessCtrlShader)
      tessCtrlShader:destroy()

      local tessEvalShader = TessellationEvaluationShader.new()
      tessEvalShader       = TessellationEvaluationShader.new(FilePath.new())
      tessEvalShader       = TessellationEvaluationShader.loadFromSource("")

      assert(tessEvalShader:isValid())
      tessEvalShader:import(FilePath.new())
      assert(tessEvalShader:getPath():isEmpty())
      tessEvalShader:load()
      tessEvalShader:compile()
      tessEvalShader:isCompiled()
      assert(tessEvalShader:clone() ~= tessEvalShader)
      tessEvalShader:destroy()
    )"));
  }

  CHECK(TestUtils::executeLuaScript(R"(
    local fragmentShader = FragmentShader.new()
    fragmentShader       = FragmentShader.new(FilePath.new())
    fragmentShader       = FragmentShader.loadFromSource("")

    assert(fragmentShader:isValid())
    fragmentShader:import(FilePath.new())
    assert(fragmentShader:getPath():isEmpty())
    fragmentShader:load()
    fragmentShader:compile()
    fragmentShader:isCompiled()
    assert(fragmentShader:clone() ~= fragmentShader)
    fragmentShader:destroy()

    local geometryShader = GeometryShader.new()
    geometryShader       = GeometryShader.new(FilePath.new())
    geometryShader       = GeometryShader.loadFromSource("")

    assert(geometryShader:isValid())
    geometryShader:import(FilePath.new())
    assert(geometryShader:getPath():isEmpty())
    geometryShader:load()
    geometryShader:compile()
    geometryShader:isCompiled()
    assert(geometryShader:clone() ~= geometryShader)
    geometryShader:destroy()

    local vertexShader = VertexShader.new()
    vertexShader       = VertexShader.new(FilePath.new())
    vertexShader       = VertexShader.loadFromSource("")

    assert(vertexShader:isValid())
    vertexShader:import(FilePath.new())
    assert(vertexShader:getPath():isEmpty())
    vertexShader:load()
    vertexShader:compile()
    vertexShader:isCompiled()
    assert(vertexShader:clone() ~= vertexShader)
    vertexShader:destroy()
  )"));
}

TEST_CASE("LuaRender ShaderProgram", "[script][lua][render]") {
#if !defined(USE_WEBGL)
  if (Raz::Renderer::checkVersion(4, 3) || Raz::Renderer::isExtensionSupported("GL_ARB_compute_shader")) {
    CHECK(TestUtils::executeLuaScript(R"(
      local computeShaderProgram = ComputeShaderProgram.new()

      computeShaderProgram:setIntAttribute(1, "int")
      computeShaderProgram:setUintAttribute(2, MaterialAttribute.Roughness)
      computeShaderProgram:setFloatAttribute(3.3, MaterialAttribute.Ambient)
      computeShaderProgram:setAttribute(Vec3f.new(), "vec3f")
      assert(computeShaderProgram:getAttributeCount() == 4)
      assert(computeShaderProgram:hasAttribute("int"))
      assert(computeShaderProgram:hasAttribute("vec3f"))
      assert(not computeShaderProgram:hasAttribute(MaterialAttribute.Specular))
      computeShaderProgram:sendAttributes()
      computeShaderProgram:removeAttribute("vec3f")
      assert(not computeShaderProgram:hasAttribute("vec3f"))
      computeShaderProgram:clearAttributes()
      assert(computeShaderProgram:getAttributeCount() == 0)

      local tex2D = Texture2D.create(1, 1, TextureColorspace.GRAY)
      computeShaderProgram:setTexture(tex2D, "tex2D")
      computeShaderProgram:setTexture(tex2D, MaterialTexture.Roughness)
      computeShaderProgram:setTexture(tex2D, MaterialTexture.Ambient)
      assert(computeShaderProgram:getTextureCount() == 3)
      assert(computeShaderProgram:hasTexture(tex2D))
      assert(computeShaderProgram:hasTexture("tex2D"))
      assert(not computeShaderProgram:hasTexture(MaterialTexture.Specular))
      assert(computeShaderProgram:getTexture(0) == computeShaderProgram:getTexture("tex2D"))
      computeShaderProgram:initTextures()
      computeShaderProgram:bindTextures()
      computeShaderProgram:removeTexture("tex2D")
      assert(not computeShaderProgram:hasTexture("tex2D"))
      computeShaderProgram:removeTexture(tex2D)
      assert(computeShaderProgram:getTextureCount() == 0)
      computeShaderProgram:clearTextures()

      computeShaderProgram:loadShaders()
      computeShaderProgram:compileShaders()
      computeShaderProgram:link()
      assert(not computeShaderProgram:isLinked())
      computeShaderProgram:updateShaders()
      computeShaderProgram:use()
      assert(not computeShaderProgram:isUsed())
      assert(computeShaderProgram:recoverUniformLocation("nothing") == -1)

      computeShaderProgram:setShader(ComputeShader.new())

      computeShaderProgram:execute(1)
      computeShaderProgram:execute(1, 1)
      computeShaderProgram:execute(1, 1, 1)

      assert(computeShaderProgram:clone():getShader() ~= nil)

      computeShaderProgram:destroyShader()
    )"));
  }
#endif

#if !defined(USE_OPENGL_ES)
  if (Raz::Renderer::checkVersion(4, 0) || Raz::Renderer::isExtensionSupported("GL_ARB_tessellation_shader")) {
    CHECK(TestUtils::executeLuaScript(R"(
      local renderShaderProgram = RenderShaderProgram.new()

      renderShaderProgram:setTessellationControlShader(TessellationControlShader.new())
      renderShaderProgram:setTessellationEvaluationShader(TessellationEvaluationShader.new())

      renderShaderProgram:setShaders(VertexShader.new(), TessellationEvaluationShader.new(), FragmentShader.new())
      renderShaderProgram:setShaders(VertexShader.new(), TessellationControlShader.new(), TessellationEvaluationShader.new(), FragmentShader.new())

      local renderShaderProgram2 = renderShaderProgram:clone()

      assert(renderShaderProgram2:hasTessellationControlShader())
      assert(renderShaderProgram2:getTessellationControlShader() ~= nil)
      assert(renderShaderProgram2:hasTessellationEvaluationShader())
      assert(renderShaderProgram2:getTessellationEvaluationShader() ~= nil)

      renderShaderProgram:destroyTessellationControlShader()
      renderShaderProgram:destroyTessellationEvaluationShader()
    )"));
  }
#endif

  CHECK(TestUtils::executeLuaScript(R"(
    local renderShaderProgram = RenderShaderProgram.new()

    renderShaderProgram:setIntAttribute(1, MaterialAttribute.BaseColor)
    renderShaderProgram:setUintAttribute(2, "uint")
    renderShaderProgram:setFloatAttribute(3.3, MaterialAttribute.Emissive)
    renderShaderProgram:setAttribute(Mat4f.new(), "mat4f")
    assert(renderShaderProgram:getAttributeCount() == 4)
    assert(renderShaderProgram:hasAttribute("uint"))
    assert(renderShaderProgram:hasAttribute("mat4f"))
    assert(not renderShaderProgram:hasAttribute(MaterialAttribute.Metallic))
    renderShaderProgram:sendAttributes()
    renderShaderProgram:removeAttribute("mat4f")
    assert(not renderShaderProgram:hasAttribute("mat4f"))
    renderShaderProgram:clearAttributes()
    assert(renderShaderProgram:getAttributeCount() == 0)

    local tex3D = Texture3D.create(1, 1, 1, TextureColorspace.GRAY)
    renderShaderProgram:setTexture(tex3D, "tex3D")
    renderShaderProgram:setTexture(tex3D, MaterialTexture.BaseColor)
    renderShaderProgram:setTexture(tex3D, MaterialTexture.Emissive)
    assert(renderShaderProgram:getTextureCount() == 3)
    assert(renderShaderProgram:hasTexture(tex3D))
    assert(renderShaderProgram:hasTexture("tex3D"))
    assert(not renderShaderProgram:hasTexture(MaterialTexture.Metallic))
    assert(renderShaderProgram:getTexture(0) == renderShaderProgram:getTexture("tex3D"))
    renderShaderProgram:initTextures()
    renderShaderProgram:bindTextures()
    renderShaderProgram:removeTexture("tex3D")
    assert(not renderShaderProgram:hasTexture("tex3D"))
    renderShaderProgram:removeTexture(tex3D)
    assert(renderShaderProgram:getTextureCount() == 0)
    renderShaderProgram:clearTextures()

    renderShaderProgram:loadShaders()
    renderShaderProgram:compileShaders()
    renderShaderProgram:link()
    assert(not renderShaderProgram:isLinked())
    renderShaderProgram:updateShaders()
    renderShaderProgram:use()
    assert(not renderShaderProgram:isUsed())
    assert(renderShaderProgram:recoverUniformLocation("nothing") == -1)

    renderShaderProgram:setVertexShader(VertexShader.new())
    renderShaderProgram:setGeometryShader(GeometryShader.new())
    renderShaderProgram:setFragmentShader(FragmentShader.new())

    renderShaderProgram:setShaders(VertexShader.new(), FragmentShader.new())
    renderShaderProgram:setShaders(VertexShader.new(), GeometryShader.new(), FragmentShader.new())

    local renderShaderProgram2 = renderShaderProgram:clone()

    assert(renderShaderProgram2:getVertexShader() ~= nil)
    assert(renderShaderProgram2:hasGeometryShader())
    assert(renderShaderProgram2:getGeometryShader() ~= nil)
    assert(renderShaderProgram2:getFragmentShader() ~= nil)

    renderShaderProgram:destroyVertexShader()
    renderShaderProgram:destroyGeometryShader()
    renderShaderProgram:destroyFragmentShader()
  )"));

#if !defined(USE_OPENGL_ES)
  CHECK(TestUtils::executeLuaScript(R"(
    local renderShaderProgram = RenderShaderProgram.new()

    local tex1D = Texture1D.create(1, TextureColorspace.GRAY)
    renderShaderProgram:setTexture(tex1D, "tex1D1")
    renderShaderProgram:setTexture(tex1D, "tex1D2")
    renderShaderProgram:setTexture(tex1D, "tex1D3")
    assert(renderShaderProgram:getTextureCount() == 3)
    assert(renderShaderProgram:hasTexture(tex1D))
    assert(renderShaderProgram:hasTexture("tex1D1"))
    assert(renderShaderProgram:getTexture(0) == renderShaderProgram:getTexture("tex1D1"))
    renderShaderProgram:initTextures()
    renderShaderProgram:bindTextures()
    renderShaderProgram:removeTexture("tex1D1")
    assert(not renderShaderProgram:hasTexture("tex1D1"))
    renderShaderProgram:removeTexture(tex1D)
    assert(renderShaderProgram:getTextureCount() == 0)
    renderShaderProgram:clearTextures()
  )"));
#endif

#if !defined(USE_WEBGL)
  if (Raz::Renderer::checkVersion(4, 2)) {
    CHECK(TestUtils::executeLuaScript(R"(
      local renderShaderProgram = RenderShaderProgram.new()

      local tex2D = Texture2D.create(1, 1, TextureColorspace.GRAY)
      renderShaderProgram:setImageTexture(tex2D, "tex2D1", ImageTextureUsage.READ)
      renderShaderProgram:setImageTexture(tex2D, "tex2D2", ImageTextureUsage.WRITE)
      renderShaderProgram:setImageTexture(tex2D, "tex2D3", ImageTextureUsage.READ_WRITE)
      assert(renderShaderProgram:getImageTextureCount() == 3)
      assert(renderShaderProgram:hasImageTexture(tex2D))
      assert(renderShaderProgram:hasImageTexture("tex2D1"))
      assert(renderShaderProgram:getImageTexture(0) == renderShaderProgram:getImageTexture("tex2D1"))
      renderShaderProgram:initImageTextures()
      renderShaderProgram:bindImageTextures()
      renderShaderProgram:removeImageTexture("tex2D1")
      assert(not renderShaderProgram:hasImageTexture("tex2D1"))
      renderShaderProgram:removeImageTexture(tex2D)
      assert(renderShaderProgram:getImageTextureCount() == 0)
      renderShaderProgram:clearImageTextures()
    )"));
  }
#endif
}

TEST_CASE("LuaRender SubmeshRenderer", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local submeshRenderer = SubmeshRenderer.new()
    submeshRenderer       = SubmeshRenderer.new(Submesh.new())
    submeshRenderer       = SubmeshRenderer.new(Submesh.new(), RenderMode.TRIANGLE)

    submeshRenderer:setRenderMode(RenderMode.TRIANGLE, Submesh.new())
    assert(submeshRenderer:getRenderMode() == RenderMode.TRIANGLE)
    submeshRenderer.materialIndex = 3
    assert(submeshRenderer.materialIndex == 3)
    assert(submeshRenderer:clone() ~= submeshRenderer)
    submeshRenderer:load(Submesh.new())
    submeshRenderer:load(Submesh.new(), RenderMode.POINT)
    submeshRenderer:draw()
  )"));
}

TEST_CASE("LuaRender Texture", "[script][lua][render]") {
#if !defined(USE_OPENGL_ES)
  CHECK(TestUtils::executeLuaScript(R"(
    local tex1D = Texture1D.new(TextureColorspace.INVALID)
    tex1D       = Texture1D.new(TextureColorspace.GRAY, TextureDataType.BYTE)
    tex1D       = Texture1D.new(1, TextureColorspace.RG)
    tex1D       = Texture1D.new(1, TextureColorspace.RGB, TextureDataType.FLOAT16)
    tex1D       = Texture1D.new(ColorPreset.White)
    tex1D       = Texture1D.new(ColorPreset.Gray, 1)

    assert(tex1D:getIndex() ~= 0)
    assert(tex1D:getColorspace() == TextureColorspace.RGB)
    assert(tex1D:getDataType() == TextureDataType.BYTE)
    tex1D:bind()
    tex1D:unbind()
    tex1D:setFilter(TextureFilter.NEAREST)
    tex1D:setFilter(TextureFilter.NEAREST, TextureFilter.NEAREST)
    tex1D:setFilter(TextureFilter.NEAREST, TextureFilter.NEAREST, TextureFilter.NEAREST)
    tex1D:setWrapping(TextureWrapping.REPEAT)
    tex1D:setColorspace(TextureColorspace.RGB)
    tex1D:setColorspace(TextureColorspace.RGB, TextureDataType.BYTE)

    tex1D = Texture1D.create(TextureColorspace.INVALID)
    tex1D = Texture1D.create(TextureColorspace.GRAY, TextureDataType.BYTE)
    tex1D = Texture1D.create(1, TextureColorspace.RG)
    tex1D = Texture1D.create(1, TextureColorspace.RGB, TextureDataType.FLOAT16)
    tex1D = Texture1D.create(ColorPreset.White)
    tex1D = Texture1D.create(ColorPreset.Gray, 1)
    tex1D:resize(2)
    tex1D:fill(ColorPreset.Red)
    assert(tex1D:getWidth() == 2)
  )"));
#endif

  CHECK(TestUtils::executeLuaScript(R"(
    local tex2D = Texture2D.new(TextureColorspace.DEPTH)
    tex2D       = Texture2D.new(TextureColorspace.GRAY, TextureDataType.BYTE)
    tex2D       = Texture2D.new(1, 1, TextureColorspace.RG)
    tex2D       = Texture2D.new(1, 1, TextureColorspace.RGB, TextureDataType.FLOAT16)
    tex2D       = Texture2D.new(Image.new())
    tex2D       = Texture2D.new(Image.new(), false)
    tex2D       = Texture2D.new(Image.new(), false, false)
    tex2D       = Texture2D.new(ColorPreset.Cyan)
    tex2D       = Texture2D.new(ColorPreset.Magenta, 1)
    tex2D       = Texture2D.new(ColorPreset.Yellow, 1, 1)

    assert(tex2D:getIndex() ~= 0)
    assert(tex2D:getColorspace() == TextureColorspace.RGB)
    assert(tex2D:getDataType() == TextureDataType.BYTE)
    tex2D:bind()
    tex2D:unbind()
    tex2D:setFilter(TextureFilter.LINEAR)
    tex2D:setFilter(TextureFilter.LINEAR, TextureFilter.LINEAR)
    tex2D:setFilter(TextureFilter.LINEAR, TextureFilter.LINEAR, TextureFilter.LINEAR)
    tex2D:setWrapping(TextureWrapping.CLAMP)
    tex2D:setColorspace(TextureColorspace.RGB)
    tex2D:setColorspace(TextureColorspace.RGB, TextureDataType.BYTE)

    tex2D = Texture2D.create(TextureColorspace.DEPTH)
    tex2D = Texture2D.create(TextureColorspace.GRAY, TextureDataType.BYTE)
    tex2D = Texture2D.create(1, 1, TextureColorspace.RG)
    tex2D = Texture2D.create(1, 1, TextureColorspace.RGB, TextureDataType.FLOAT16)
    tex2D = Texture2D.create(Image.new())
    tex2D = Texture2D.create(Image.new(), false)
    tex2D = Texture2D.create(Image.new(), false, false)
    tex2D = Texture2D.create(ColorPreset.Cyan)
    tex2D = Texture2D.create(ColorPreset.Magenta, 1)
    tex2D = Texture2D.create(ColorPreset.Yellow, 1, 1)
    tex2D:load(Image.new())
    tex2D:load(Image.new(), false)
    tex2D:load(Image.new(), false, false)
    tex2D:resize(2, 2)
    tex2D:fill(ColorPreset.Green)
    assert(tex2D:getWidth() == 2)
    assert(tex2D:getHeight() == 2)
    assert(tex2D:recoverImage():getChannelCount() == 3)

    local tex3D = Texture3D.new(TextureColorspace.INVALID)
    tex3D       = Texture3D.new(TextureColorspace.RGBA, TextureDataType.BYTE)
    tex3D       = Texture3D.new(1, 1, 1, TextureColorspace.SRGB)
    tex3D       = Texture3D.new(1, 1, 1, TextureColorspace.SRGBA, TextureDataType.FLOAT32)
    tex3D       = Texture3D.new(ColorPreset.Black)
    tex3D       = Texture3D.new(ColorPreset.MediumRed, 1)
    tex3D       = Texture3D.new(ColorPreset.MediumGreen, 1, 1)
    tex3D       = Texture3D.new(ColorPreset.MediumBlue, 1, 1, 1)

    assert(tex3D:getIndex() ~= 0)
    assert(tex3D:getColorspace() == TextureColorspace.RGB)
    assert(tex3D:getDataType() == TextureDataType.BYTE)
    tex3D:bind()
    tex3D:unbind()
    tex3D:setFilter(TextureFilter.NEAREST)
    tex3D:setFilter(TextureFilter.NEAREST, TextureFilter.NEAREST)
    tex3D:setFilter(TextureFilter.NEAREST, TextureFilter.NEAREST, TextureFilter.NEAREST)
    tex3D:setWrapping(TextureWrapping.REPEAT)
    tex3D:setColorspace(TextureColorspace.RGB)
    tex3D:setColorspace(TextureColorspace.RGB, TextureDataType.BYTE)

    tex3D = Texture3D.create(TextureColorspace.INVALID)
    tex3D = Texture3D.create(TextureColorspace.RGBA, TextureDataType.BYTE)
    tex3D = Texture3D.create(1, 1, 1, TextureColorspace.SRGB)
    tex3D = Texture3D.create(1, 1, 1, TextureColorspace.SRGBA, TextureDataType.FLOAT32)
    tex3D = Texture3D.create({ Image.new() })
    tex3D = Texture3D.create({ Image.new() }, false)
    tex3D = Texture3D.create({ Image.new() }, false, false)
    tex3D = Texture3D.create(ColorPreset.Black)
    tex3D = Texture3D.create(ColorPreset.MediumRed, 1)
    tex3D = Texture3D.create(ColorPreset.MediumGreen, 1, 1)
    tex3D = Texture3D.create(ColorPreset.MediumBlue, 1, 1, 1)
    tex3D:load({ Image.new() })
    tex3D:load({ Image.new() }, false)
    tex3D:load({ Image.new() }, false, false)
    tex3D:resize(2, 2, 2)
    tex3D:fill(ColorPreset.Blue)
    assert(tex3D:getWidth() == 2)
    assert(tex3D:getHeight() == 2)
    assert(tex3D:getDepth() == 2)
  )"));
}

TEST_CASE("LuaRender UniformBuffer", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local uniformBuffer = UniformBuffer.new(1)
    uniformBuffer       = UniformBuffer.new(1, UniformBufferUsage.DYNAMIC)

    assert(uniformBuffer:getIndex() ~= 0)
    uniformBuffer:bindUniformBlock(RenderShaderProgram.new(), 1, 0)
    uniformBuffer:bindUniformBlock(RenderShaderProgram.new(), "", 0)
    uniformBuffer:bindBase(0)
    uniformBuffer:bindRange(0, 0, 1)
    uniformBuffer:bind()
    uniformBuffer:unbind()
    uniformBuffer:sendIntData(1, 0)
    uniformBuffer:sendUintData(2, 0)
    uniformBuffer:sendFloatData(3.3, 0)
    uniformBuffer:sendData(Vec2f.new(), 0)
    uniformBuffer:sendData(Vec3f.new(), 0)
    uniformBuffer:sendData(Vec4f.new(), 0)
    uniformBuffer:sendData(Mat2f.new(), 0)
    uniformBuffer:sendData(Mat3f.new(), 0)
    uniformBuffer:sendData(Mat4f.new(), 0)
  )"));
}

TEST_CASE("LuaRender Window", "[script][lua][render]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local renderSystem = RenderSystem.new() -- A RenderSystem is needed to use a Window

    -- The constructors with less arguments are not tested to avoid spawning multiple windows
    local window = Window.new(renderSystem, 1, 1, "", WindowSetting.INVISIBLE)
    window       = Window.new(renderSystem, 1, 1, "", WindowSetting.INVISIBLE, 1)

    assert(window:getWidth() == window:getHeight())

    window:setClearColor(ColorPreset.Black)
    window:setClearColor(ColorPreset.White, 1)
    window:setClearColor(1, 0, 0)
    window:setClearColor(0, 1, 0, 1)
    window:setTitle("Test")
    window:setIcon(Image.new(1, 1, ImageColorspace.RGBA))
    window:resize(2, 2)
    --window:makeFullscreen() -- Making the window fullscreen makes it visible; not testing the function
    window:makeWindowed()
    window:enableFaceCulling()
    window:enableFaceCulling(true)
    window:disableFaceCulling()
    window:enableVerticalSync()
    window:enableVerticalSync(true)
    window:disableVerticalSync()
    window:showCursor()
    window:hideCursor()
    window:disableCursor()
    window:addKeyCallback(Keyboard.A, function () end)
    window:addKeyCallback(Keyboard.F1, function () end, Input.ONCE)
    window:addKeyCallback(Keyboard.NUM0, function () end, Input.ALWAYS, function () end)
    window:addMouseButtonCallback(Mouse.LEFT_CLICK, function () end)
    window:addMouseButtonCallback(Mouse.RIGHT_CLICK, function () end, Input.ONCE)
    window:addMouseButtonCallback(Mouse.MIDDLE_CLICK, function () end, Input.ALWAYS, function () end)
    window:setMouseScrollCallback(function () end)
    window:setMouseMoveCallback(function () end)
    window:setCloseCallback(function () end)
    window:updateCallbacks()
    assert(window:run(0))
    assert(window:recoverMousePosition():x() >= 0)
  )"));

#if !defined(RAZ_NO_OVERLAY)
  CHECK(TestUtils::executeLuaScript(R"(
    local renderSystem = RenderSystem.new() -- A RenderSystem is needed to use a Window
    local window       = Window.new(renderSystem, 1, 1, "", WindowSetting.INVISIBLE)

    assert(window:getOverlay() ~= nil)
    window:enableOverlay()
    window:enableOverlay(true)
    window:disableOverlay()
  )"));
#endif
}
