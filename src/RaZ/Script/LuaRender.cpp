#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/BloomRenderProcess.hpp"
#include "RaZ/Render/BoxBlurRenderProcess.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ChromaticAberrationRenderProcess.hpp"
#include "RaZ/Render/ConvolutionRenderProcess.hpp"
#include "RaZ/Render/GaussianBlurRenderProcess.hpp"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderGraph.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/SsrRenderProcess.hpp"
#include "RaZ/Render/VignetteRenderProcess.hpp"
#include "RaZ/Render/Window.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerRenderTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Camera> camera = state.new_usertype<Camera>("Camera",
                                                              sol::constructors<Camera(),
                                                                                Camera(unsigned int, unsigned int),
                                                                                Camera(unsigned int, unsigned int, Radiansf),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float, float),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float, float, ProjectionType)>(),
                                                              sol::base_classes, sol::bases<Component>());
    camera["fieldOfView"]                    = sol::property(&Camera::getFieldOfView, &Camera::setFieldOfView);
    camera["orthographicBound"]              = sol::property(&Camera::getOrthographicBound, &Camera::setOrthographicBound);
    camera["cameraType"]                     = sol::property(&Camera::getCameraType, &Camera::setCameraType);
    camera["getViewMatrix"]                  = &Camera::getViewMatrix;
    camera["getInverseViewMatrix"]           = &Camera::getInverseViewMatrix;
    camera["getProjectionMatrix"]            = &Camera::getProjectionMatrix;
    camera["getInverseProjectionMatrix"]     = &Camera::getInverseProjectionMatrix;
    camera["setProjectionType"]              = &Camera::setProjectionType;
    camera["setTarget"]                      = &Camera::setTarget;
    camera["computeViewMatrix"]              = &Camera::computeViewMatrix;
    camera["computeLookAt"]                  = &Camera::computeLookAt;
    camera["computeInverseViewMatrix"]       = &Camera::computeInverseViewMatrix;
    camera["computePerspectiveMatrix"]       = &Camera::computePerspectiveMatrix;
    camera["computeOrthographicMatrix"]      = sol::overload(PickOverload<float, float, float, float, float, float>(&Camera::computeOrthographicMatrix),
                                                             PickOverload<>(&Camera::computeOrthographicMatrix));
    camera["computeProjectionMatrix"]        = &Camera::computeProjectionMatrix;
    camera["computeInverseProjectionMatrix"] = &Camera::computeInverseProjectionMatrix;
    camera["resizeViewport"]                 = &Camera::resizeViewport;
    camera["unproject"]                      = sol::overload(PickOverload<const Vec4f&>(&Camera::unproject),
                                                             PickOverload<const Vec3f&>(&Camera::unproject),
                                                             PickOverload<const Vec2f&>(&Camera::unproject));

    state.new_enum<CameraType>("CameraType", {
      { "FREE_FLY", CameraType::FREE_FLY },
      { "LOOK_AT",  CameraType::LOOK_AT }
    });

    state.new_enum<ProjectionType>("ProjectionType", {
      { "PERSPECTIVE",  ProjectionType::PERSPECTIVE },
      { "ORTHOGRAPHIC", ProjectionType::ORTHOGRAPHIC }
    });
  }

  {
    sol::usertype<Cubemap> cubemap = state.new_usertype<Cubemap>("Cubemap",
                                                                 sol::constructors<Cubemap(),
                                                                                   Cubemap(const Image&, const Image&,
                                                                                           const Image&, const Image&,
                                                                                           const Image&, const Image&)>());
    cubemap["getIndex"]   = &Cubemap::getIndex;
    cubemap["getProgram"] = [] (const Cubemap& c) { return &c.getProgram(); };
    cubemap["load"]       = &Cubemap::load;
    cubemap["bind"]       = &Cubemap::bind;
    cubemap["unbind"]     = &Cubemap::unbind;
    cubemap["draw"]       = &Cubemap::draw;
  }

  {
    sol::usertype<Framebuffer> framebuffer = state.new_usertype<Framebuffer>("Framebuffer",
                                                                             sol::constructors<Framebuffer()>());
    framebuffer["getIndex"]            = &Framebuffer::getIndex;
    framebuffer["isEmpty"]             = &Framebuffer::isEmpty;
    framebuffer["hasDepthBuffer"]      = &Framebuffer::hasDepthBuffer;
    framebuffer["getDepthBuffer"]      = [] (const Framebuffer& f) { return &f.getDepthBuffer(); };
    framebuffer["getColorBufferCount"] = &Framebuffer::getColorBufferCount;
    framebuffer["getColorBuffer"]      = [] (const Framebuffer& f, std::size_t i) { return &f.getColorBuffer(i); };
    framebuffer["recoverVertexShader"] = &Framebuffer::recoverVertexShader;
    framebuffer["setDepthBuffer"]      = &Framebuffer::setDepthBuffer;
    framebuffer["addColorBuffer"]      = &Framebuffer::addColorBuffer;
    framebuffer["removeTextureBuffer"] = &Framebuffer::removeTextureBuffer;
    framebuffer["clearDepthBuffer"]    = &Framebuffer::clearDepthBuffer;
    framebuffer["clearColorBuffers"]   = &Framebuffer::clearColorBuffers;
    framebuffer["clearTextureBuffers"] = &Framebuffer::clearTextureBuffers;
    framebuffer["resizeBuffers"]       = &Framebuffer::resizeBuffers;
    framebuffer["mapBuffers"]          = &Framebuffer::mapBuffers;
    framebuffer["bind"]                = &Framebuffer::bind;
    framebuffer["unbind"]              = &Framebuffer::unbind;
    framebuffer["display"]             = &Framebuffer::display;
  }

  // GraphicObjects
  {
    {
      sol::usertype<IndexBuffer> indexBuffer = state.new_usertype<IndexBuffer>("IndexBuffer",
                                                                               sol::constructors<IndexBuffer()>());
      indexBuffer["getIndex"]           = &IndexBuffer::getIndex;
      indexBuffer["bind"]               = &IndexBuffer::bind;
      indexBuffer["unbind"]             = &IndexBuffer::unbind;
      indexBuffer["lineIndexCount"]     = &IndexBuffer::lineIndexCount;
      indexBuffer["triangleIndexCount"] = &IndexBuffer::triangleIndexCount;
    }

    {
      sol::usertype<VertexArray> vertexArray = state.new_usertype<VertexArray>("VertexArray",
                                                                               sol::constructors<VertexArray()>());
      vertexArray["getIndex"] = &VertexArray::getIndex;
      vertexArray["bind"]     = &VertexArray::bind;
      vertexArray["unbind"]   = &VertexArray::unbind;
    }

    {
      sol::usertype<VertexBuffer> vertexBuffer = state.new_usertype<VertexBuffer>("VertexBuffer",
                                                                                  sol::constructors<VertexBuffer()>());
      vertexBuffer["getIndex"]    = &VertexBuffer::getIndex;
      vertexBuffer["bind"]        = &VertexBuffer::bind;
      vertexBuffer["unbind"]      = &VertexBuffer::unbind;
      vertexBuffer["vertexCount"] = &VertexBuffer::vertexCount;
    }
  }

  {
    sol::usertype<Light> light = state.new_usertype<Light>("Light",
                                                           sol::constructors<Light(LightType, float),
                                                                             Light(LightType, float, const Color&),
                                                                             Light(LightType, const Vec3f&, float),
                                                                             Light(LightType, const Vec3f&, float, const Color&),
                                                                             Light(LightType, const Vec3f&, float, Radiansf),
                                                                             Light(LightType, const Vec3f&, float, Radiansf, const Color&)>(),
                                                           sol::base_classes, sol::bases<Component>());
    light["type"]      = sol::property(&Light::getType, &Light::setType);
    light["direction"] = sol::property(&Light::getDirection, &Light::setDirection);
    light["energy"]    = sol::property(&Light::getEnergy, &Light::setEnergy);
    light["color"]     = sol::property(&Light::getColor, &Light::setColor);
    light["angle"]     = sol::property(&Light::getAngle, &Light::setAngle);

    state.new_enum<LightType>("LightType", {
      { "POINT",       LightType::POINT },
      { "DIRECTIONAL", LightType::DIRECTIONAL },
      { "SPOT",        LightType::SPOT }
    });
  }

  {
    sol::usertype<Material> material = state.new_usertype<Material>("Material",
                                                                    sol::constructors<Material(),
                                                                                      Material(MaterialType)>());
    material["getProgram"] = PickNonConstOverload<>(&Material::getProgram);
    material["isEmpty"]    = &Material::isEmpty;
    material["clone"]      = &Material::clone;
    material["loadType"]   = &Material::loadType;

    state.new_enum<std::string>("MaterialAttribute", {
      { "BaseColor",    MaterialAttribute::BaseColor },
      { "Emissive",     MaterialAttribute::Emissive },
      { "Metallic",     MaterialAttribute::Metallic },
      { "Roughness",    MaterialAttribute::Roughness },
      { "Ambient",      MaterialAttribute::Ambient },
      { "Specular",     MaterialAttribute::Specular },
      { "Transparency", MaterialAttribute::Transparency }
    });

    state.new_enum<std::string>("MaterialTexture", {
      { "BaseColor",    MaterialTexture::BaseColor },
      { "Emissive",     MaterialTexture::Emissive },
      { "Ambient",      MaterialTexture::Ambient },
      { "Normal",       MaterialTexture::Normal },
      { "Metallic",     MaterialTexture::Metallic },
      { "Roughness",    MaterialTexture::Roughness },
      { "Specular",     MaterialTexture::Specular },
      { "Transparency", MaterialTexture::Transparency },
      { "Bump",         MaterialTexture::Bump }
    });

    state.new_enum<MaterialType>("MaterialType", {
      { "COOK_TORRANCE",     MaterialType::COOK_TORRANCE },
      { "BLINN_PHONG",       MaterialType::BLINN_PHONG },
      { "SINGLE_TEXTURE_2D", MaterialType::SINGLE_TEXTURE_2D },
      { "SINGLE_TEXTURE_3D", MaterialType::SINGLE_TEXTURE_3D }
    });
  }

  {
    sol::usertype<MeshRenderer> meshRenderer = state.new_usertype<MeshRenderer>("MeshRenderer",
                                                                                sol::constructors<MeshRenderer(),
                                                                                                  MeshRenderer(const Mesh&),
                                                                                                  MeshRenderer(const Mesh&, RenderMode)>(),
                                                                                sol::base_classes, sol::bases<Component>());
    meshRenderer["isEnabled"]           = &MeshRenderer::isEnabled;
    meshRenderer["getSubmeshRenderers"] = PickNonConstOverload<>(&MeshRenderer::getSubmeshRenderers);
    meshRenderer["getMaterials"]        = PickNonConstOverload<>(&MeshRenderer::getMaterials);
    meshRenderer["enable"]              = sol::overload([] (MeshRenderer& r) { r.enable(); },
                                                        PickOverload<bool>(&MeshRenderer::enable));
    meshRenderer["disable"]             = &MeshRenderer::disable;
    meshRenderer["setRenderMode"]       = &MeshRenderer::setRenderMode;
    meshRenderer["setMaterial"]         = [] (MeshRenderer& r, Material& mat) { return &r.setMaterial(std::move(mat)); };
    meshRenderer["addMaterial"]         = sol::overload([] (MeshRenderer& r) { return &r.addMaterial(); },
                                                        [] (MeshRenderer& r, Material& mat) { return &r.addMaterial(std::move(mat)); });
    meshRenderer["removeMaterial"]      = &MeshRenderer::removeMaterial;
    meshRenderer["addSubmeshRenderer"]  = sol::overload(&MeshRenderer::addSubmeshRenderer<>,
                                                        &MeshRenderer::addSubmeshRenderer<const Submesh&>,
                                                        &MeshRenderer::addSubmeshRenderer<const Submesh&, RenderMode>);
    meshRenderer["clone"]               = &MeshRenderer::clone;
    meshRenderer["load"]                = sol::overload([] (MeshRenderer& r, const Mesh& m) { r.load(m); },
                                                        PickOverload<const Mesh&, RenderMode>(&MeshRenderer::load));
    meshRenderer["loadMaterials"]       = &MeshRenderer::loadMaterials;
    meshRenderer["draw"]                = &MeshRenderer::draw;
  }

  // Overlay
  {
    {
      sol::usertype<Overlay> overlay = state.new_usertype<Overlay>("Overlay",
                                                                   sol::constructors<Overlay()>());
      overlay["isEmpty"]          = &Overlay::isEmpty;
      overlay["addWindow"]        = sol::overload([] (Overlay& o, std::string t) { return &o.addWindow(std::move(t)); },
                                                  [] (Overlay& o, std::string t, const Vec2f& s) { return &o.addWindow(std::move(t), s); },
                                                  PickOverload<std::string, const Vec2f&, const Vec2f&>(&Overlay::addWindow));
      overlay["hasKeyboardFocus"] = &Overlay::hasKeyboardFocus;
      overlay["hasMouseFocus"]    = &Overlay::hasMouseFocus;
      overlay["render"]           = &Overlay::render;
    }

    {
      state.new_usertype<OverlayButton>("OverlayButton",
                                        sol::constructors<OverlayButton(std::string, std::function<void()>)>(),
                                        sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      state.new_usertype<OverlayCheckbox>("OverlayCheckbox",
                                          sol::constructors<OverlayCheckbox(std::string, std::function<void()>, std::function<void()>, bool)>(),
                                          sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      sol::usertype<OverlayColoredLabel> overlayColoredLabel = state.new_usertype<OverlayColoredLabel>("OverlayColoredLabel",
                                                                                                       sol::constructors<OverlayColoredLabel(std::string,
                                                                                                                                             const Color&),
                                                                                                                         OverlayColoredLabel(std::string,
                                                                                                                                             const Color&,
                                                                                                                                             float)>(),
                                                                                                       sol::base_classes, sol::bases<OverlayElement>());
      overlayColoredLabel["text"]  = sol::property(&OverlayColoredLabel::getText, PickOverload<std::string>(&OverlayColoredLabel::setText));
      overlayColoredLabel["color"] = sol::property(&OverlayColoredLabel::getColor, [] (OverlayColoredLabel& l, const Color& c) { l.setColor(c); });
      overlayColoredLabel["alpha"] = sol::property(&OverlayColoredLabel::getAlpha, &OverlayColoredLabel::setAlpha);
    }

    {
      state.new_usertype<OverlayDropdown>("OverlayDropdown",
                                          sol::constructors<OverlayDropdown(std::string, std::vector<std::string>,
                                                                            std::function<void(const std::string&, std::size_t)>),
                                                            OverlayDropdown(std::string, std::vector<std::string>,
                                                                            std::function<void(const std::string&, std::size_t)>, std::size_t)>(),
                                          sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      sol::usertype<OverlayElement> overlayElement = state.new_usertype<OverlayElement>("OverlayElement", sol::no_constructor);
      overlayElement["getType"]   = &OverlayElement::getType;
      overlayElement["isEnabled"] = &OverlayElement::isEnabled;
      overlayElement["enable"]    = sol::overload([] (OverlayElement& r) { r.enable(); },
                                                  PickOverload<bool>(&OverlayElement::enable));
      overlayElement["disable"]   = &OverlayElement::disable;
    }

    {
      state.new_usertype<OverlayFpsCounter>("OverlayFpsCounter",
                                            sol::constructors<OverlayFpsCounter(std::string)>(),
                                            sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      state.new_usertype<OverlayFrameTime>("OverlayFrameTime",
                                           sol::constructors<OverlayFrameTime(std::string)>(),
                                           sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      sol::usertype<OverlayLabel> overlayLabel = state.new_usertype<OverlayLabel>("OverlayLabel",
                                                                                  sol::constructors<OverlayLabel(std::string)>(),
                                                                                  sol::base_classes, sol::bases<OverlayElement>());
      overlayLabel["text"] = sol::property(&OverlayLabel::getText, &OverlayLabel::setText);
    }

    {
      state.new_usertype<OverlayListBox>("OverlayListBox",
                                         sol::constructors<OverlayListBox(std::string, std::vector<std::string>,
                                                                          std::function<void(const std::string&, std::size_t)>),
                                                           OverlayListBox(std::string, std::vector<std::string>,
                                                                          std::function<void(const std::string&, std::size_t)>, std::size_t)>(),
                                         sol::base_classes, sol::bases<OverlayElement>());
    }

    // OverlayPlot
    {
      {
        sol::usertype<OverlayPlot> overlayPlot = state.new_usertype<OverlayPlot>("OverlayPlot",
                                                                                 sol::constructors<OverlayPlot(std::string, std::size_t),
                                                                                                   OverlayPlot(std::string, std::size_t, std::string),
                                                                                                   OverlayPlot(std::string, std::size_t, std::string,
                                                                                                               std::string),
                                                                                                   OverlayPlot(std::string, std::size_t, std::string,
                                                                                                               std::string, float)>(),
                                                                                 sol::base_classes, sol::bases<OverlayElement>());
        overlayPlot["addEntry"] = sol::overload([] (OverlayPlot& p, std::string n) { return p.addEntry(std::move(n)); },
                                                PickOverload<std::string, OverlayPlotType>(&OverlayPlot::addEntry));
      }

      {
        sol::usertype<OverlayPlotEntry> overlayPlotEntry = state.new_usertype<OverlayPlotEntry>("OverlayPlotEntry", sol::no_constructor);
        overlayPlotEntry["push"] = &OverlayPlotEntry::push;
      }

      state.new_enum<OverlayPlotType>("OverlayPlotType", {
        { "LINE",   OverlayPlotType::LINE },
        { "SHADED", OverlayPlotType::SHADED }
      });
    }

    {
      sol::usertype<OverlayProgressBar> overlayProgressBar = state.new_usertype<OverlayProgressBar>("OverlayProgressBar",
                                                                                                    sol::constructors<OverlayProgressBar(int, int),
                                                                                                                      OverlayProgressBar(int, int, bool)>(),
                                                                                                    sol::base_classes, sol::bases<OverlayElement>());
      overlayProgressBar["currentValue"] = sol::property(&OverlayProgressBar::getCurrentValue, &OverlayProgressBar::setCurrentValue);
      overlayProgressBar["hasStarted"]   = &OverlayProgressBar::hasStarted;
      overlayProgressBar["hasFinished"]  = &OverlayProgressBar::hasFinished;
      overlayProgressBar["add"]          = &OverlayProgressBar::operator+=;
    }

    {
      state.new_usertype<OverlaySeparator>("OverlaySeparator",
                                           sol::constructors<OverlaySeparator()>(),
                                           sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      state.new_usertype<OverlaySlider>("OverlaySlider",
                                        sol::constructors<OverlaySlider(std::string, std::function<void(float)>, float, float, float)>(),
                                        sol::base_classes, sol::bases<OverlayElement>());
    }

    {
      sol::usertype<OverlayTextbox> overlayTextbox = state.new_usertype<OverlayTextbox>("OverlayTextbox",
                                                                                        sol::constructors<
                                                                                          OverlayTextbox(std::string,
                                                                                                         std::function<void(const std::string&)>)
                                                                                        >(),
                                                                                        sol::base_classes, sol::bases<OverlayElement>());
      overlayTextbox["text"]   = sol::property(&OverlayTextbox::getText, &OverlayTextbox::setText);
      overlayTextbox["append"] = &OverlayTextbox::append;
      overlayTextbox["clear"]  = &OverlayTextbox::clear;
    }

    {
      sol::usertype<OverlayTexture> overlayTexture = state.new_usertype<OverlayTexture>("OverlayTexture",
                                                                                        sol::constructors<OverlayTexture(const Texture2D&, unsigned int,
                                                                                                                         unsigned int),
                                                                                                          OverlayTexture(const Texture2D&)>(),
                                                                                        sol::base_classes, sol::bases<OverlayElement>());
      overlayTexture["setTexture"] = sol::overload(PickOverload<const Texture2D&, unsigned int, unsigned int>(&OverlayTexture::setTexture),
                                                   PickOverload<const Texture2D&>(&OverlayTexture::setTexture));
    }

    {
      sol::usertype<OverlayWindow> overlayWindow = state.new_usertype<OverlayWindow>("OverlayWindow",
                                                                                     sol::constructors<OverlayWindow(std::string),
                                                                                                       OverlayWindow(std::string, const Vec2f&),
                                                                                                       OverlayWindow(std::string, const Vec2f&,
                                                                                                                     const Vec2f&)>());
      overlayWindow["isEnabled"]       = &OverlayWindow::isEnabled;
      overlayWindow["enable"]          = sol::overload([] (OverlayWindow& r) { r.enable(); },
                                                       PickOverload<bool>(&OverlayWindow::enable));
      overlayWindow["disable"]         = &OverlayWindow::disable;
      overlayWindow["addLabel"]        = &OverlayWindow::addLabel;
      overlayWindow["addColoredLabel"] = sol::overload([] (OverlayWindow& w, std::string t, const Color& c) { return &w.addColoredLabel(std::move(t), c); },
                                                       PickOverload<std::string, const Color&, float>(&OverlayWindow::addColoredLabel),
                                                       [] (OverlayWindow& w, std::string t,
                                                           float r, float g, float b) { return &w.addColoredLabel(std::move(t), r, g, b); },
                                                       PickOverload<std::string, float, float, float, float>(&OverlayWindow::addColoredLabel));
      overlayWindow["addButton"]       = &OverlayWindow::addButton;
      overlayWindow["addCheckbox"]     = &OverlayWindow::addCheckbox;
      overlayWindow["addSlider"]       = &OverlayWindow::addSlider;
      overlayWindow["addTextbox"]      = &OverlayWindow::addTextbox;
      overlayWindow["addListBox"]      = sol::overload([] (OverlayWindow& w, std::string l, std::vector<std::string> e,
                                                           std::function<void(const std::string&, std::size_t)> c) { return &w.addListBox(std::move(l),
                                                                                                                                          std::move(e),
                                                                                                                                          std::move(c)); },
                                                       PickOverload<std::string, std::vector<std::string>, std::function<void(const std::string&, std::size_t)>,
                                                                    std::size_t>(&OverlayWindow::addListBox));
      overlayWindow["addDropdown"]     = sol::overload([] (OverlayWindow& w, std::string l, std::vector<std::string> e,
                                                           std::function<void(const std::string&, std::size_t)> c) { return &w.addDropdown(std::move(l),
                                                                                                                                           std::move(e),
                                                                                                                                           std::move(c)); },
                                                       PickOverload<std::string, std::vector<std::string>, std::function<void(const std::string&, std::size_t)>,
                                                                    std::size_t>(&OverlayWindow::addDropdown));
      overlayWindow["addTexture"]      = sol::overload(PickOverload<const Texture2D&, unsigned int, unsigned int>(&OverlayWindow::addTexture),
                                                       PickOverload<const Texture2D&>(&OverlayWindow::addTexture));
      overlayWindow["addProgressBar"]  = sol::overload([] (OverlayWindow& w, int min, int max) { return &w.addProgressBar(min, max); },
                                                       PickOverload<int, int, bool>(&OverlayWindow::addProgressBar));
      overlayWindow["addPlot"]         = sol::overload([] (OverlayWindow& w, std::string l, std::size_t c) { return &w.addPlot(std::move(l), c); },
                                                       [] (OverlayWindow& w, std::string l, std::size_t c,
                                                           std::string x) { return &w.addPlot(std::move(l), c, std::move(x)); },
                                                       [] (OverlayWindow& w, std::string l, std::size_t c,
                                                           std::string x, std::string y) { return &w.addPlot(std::move(l), c, std::move(x), std::move(y)); },
                                                       PickOverload<std::string, std::size_t, std::string, std::string, float>(&OverlayWindow::addPlot));
      overlayWindow["addSeparator"]    = &OverlayWindow::addSeparator;
      overlayWindow["addFrameTime"]    = &OverlayWindow::addFrameTime;
      overlayWindow["addFpsCounter"]   = &OverlayWindow::addFpsCounter;
      overlayWindow["render"]          = &OverlayWindow::render;
    }

    state.new_enum<OverlayElementType>("OverlayElementType", {
      { "LABEL",         OverlayElementType::LABEL },
      { "COLORED_LABEL", OverlayElementType::COLORED_LABEL },
      { "BUTTON",        OverlayElementType::BUTTON },
      { "CHECKBOX",      OverlayElementType::CHECKBOX },
      { "SLIDER",        OverlayElementType::SLIDER },
      { "TEXTBOX",       OverlayElementType::TEXTBOX },
      { "LIST_BOX",      OverlayElementType::LIST_BOX },
      { "DROPDOWN",      OverlayElementType::DROPDOWN },
      { "TEXTURE",       OverlayElementType::TEXTURE },
      { "PROGRESS_BAR",  OverlayElementType::PROGRESS_BAR },
      { "PLOT",          OverlayElementType::PLOT },
      { "SEPARATOR",     OverlayElementType::SEPARATOR },
      { "FRAME_TIME",    OverlayElementType::FRAME_TIME },
      { "FPS_COUNTER",   OverlayElementType::FPS_COUNTER }
    });
  }

  {
    sol::usertype<RenderGraph> renderGraph = state.new_usertype<RenderGraph>("RenderGraph",
                                                                             sol::constructors<RenderGraph()>(),
                                                                             sol::base_classes, sol::bases<Graph<RenderPass>>());
    renderGraph["getNodeCount"] = &RenderGraph::getNodeCount;
    renderGraph["getNode"]      = PickNonConstOverload<std::size_t>(&RenderGraph::getNode);
    renderGraph["addNode"]      = sol::overload(&RenderGraph::addNode<>,
                                                [] (RenderGraph& g, FragmentShader& s) { g.addNode(std::move(s)); },
                                                [] (RenderGraph& g, FragmentShader& s, std::string n) { g.addNode(std::move(s), std::move(n)); });
    renderGraph["removeNode"]   = &RenderGraph::removeNode;

    renderGraph["isValid"]                             = &RenderGraph::isValid;
    renderGraph["getGeometryPass"]                     = PickNonConstOverload<>(&RenderGraph::getGeometryPass);
    renderGraph["addBloomRenderProcess"]               = &RenderGraph::addRenderProcess<BloomRenderProcess>;
    renderGraph["addBoxBlurRenderProcess"]             = &RenderGraph::addRenderProcess<BoxBlurRenderProcess>;
    renderGraph["addChromaticAberrationRenderProcess"] = &RenderGraph::addRenderProcess<ChromaticAberrationRenderProcess>;
    renderGraph["addConvolutionRenderProcess"]         = sol::overload(&RenderGraph::addRenderProcess<ConvolutionRenderProcess, const Mat3f&>,
                                                                       &RenderGraph::addRenderProcess<ConvolutionRenderProcess, const Mat3f&, std::string>);
    renderGraph["addGaussianBlurRenderProcess"]        = &RenderGraph::addRenderProcess<GaussianBlurRenderProcess>;
    renderGraph["addSsrRenderProcess"]                 = &RenderGraph::addRenderProcess<SsrRenderProcess>;
    renderGraph["addVignetteRenderProcess"]            = &RenderGraph::addRenderProcess<VignetteRenderProcess>;
    renderGraph["resizeViewport"]                      = &RenderGraph::resizeViewport;
    renderGraph["updateShaders"]                       = &RenderGraph::updateShaders;
  }

  {
    sol::usertype<RenderPass> renderPass = state.new_usertype<RenderPass>("RenderPass",
                                                                          sol::constructors<RenderPass()>(),
                                                                          sol::base_classes, sol::bases<GraphNode<RenderPass>>());
    renderPass["getParentCount"] = &RenderPass::getParentCount;
    renderPass["getParent"]      = PickNonConstOverload<std::size_t>(&RenderPass::getParent);
    renderPass["getChildCount"]  = &RenderPass::getChildCount;
    renderPass["getChild"]       = PickNonConstOverload<std::size_t>(&RenderPass::getChild);
    renderPass["isRoot"]         = &RenderPass::isRoot;
    renderPass["isLeaf"]         = &RenderPass::isLeaf;
    renderPass["isIsolated"]     = &RenderPass::isIsolated;
    renderPass["addParents"]     = [] (RenderPass& p, sol::variadic_args args) { for (auto parent : args) p.addParents(parent); };
    renderPass["removeParents"]  = [] (RenderPass& p, sol::variadic_args args) { for (auto parent : args) p.removeParents(parent); };
    renderPass["addChildren"]    = [] (RenderPass& p, sol::variadic_args args) { for (auto child : args) p.addChildren(child); };
    renderPass["removeChildren"] = [] (RenderPass& p, sol::variadic_args args) { for (auto child : args) p.removeChildren(child); };

    renderPass["isEnabled"]            = &RenderPass::isEnabled;
    renderPass["getName"]              = &RenderPass::getName;
    renderPass["getProgram"]           = PickNonConstOverload<>(&RenderPass::getProgram);
    renderPass["getReadTextureCount"]  = &RenderPass::getReadTextureCount;
    renderPass["getReadTexture"]       = sol::overload([] (const RenderPass& p, std::size_t i) { return &p.getReadTexture(i); },
                                                       [] (const RenderPass& p, const std::string& n) { return &p.getReadTexture(n); });
    renderPass["hasReadTexture"]       = &RenderPass::hasReadTexture;
    renderPass["getFramebuffer"]       = [] (const RenderPass& p) { return &p.getFramebuffer(); };
    renderPass["recoverElapsedTime"]   = &RenderPass::recoverElapsedTime;
    renderPass["setName"]              = &RenderPass::setName;
    renderPass["setProgram"]           = [] (RenderPass& p, RenderShaderProgram& sp) { p.setProgram(std::move(sp)); };
    renderPass["enable"]               = sol::overload([] (RenderPass& p) { p.enable(); },
                                                       PickOverload<bool>(&RenderPass::enable));
    renderPass["disable"]              = &RenderPass::disable;
    renderPass["isValid"]              = &RenderPass::isValid;
    renderPass["addReadTexture"]       = sol::overload(
#if !defined(USE_OPENGL_ES)
                                                       [] (RenderPass& p, Texture1DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); },
#endif
                                                       [] (RenderPass& p, Texture2DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); },
                                                       [] (RenderPass& p, Texture3DPtr t, const std::string& n) { p.addReadTexture(std::move(t), n); });
    renderPass["removeReadTexture"]    = &RenderPass::removeReadTexture;
    renderPass["clearReadTextures"]    = &RenderPass::clearReadTextures;
    renderPass["setWriteDepthTexture"] = &RenderPass::setWriteDepthTexture;
    renderPass["addWriteColorTexture"] = &RenderPass::addWriteColorTexture;
    renderPass["removeWriteTexture"]   = &RenderPass::removeWriteTexture;
    renderPass["clearWriteTextures"]   = &RenderPass::clearWriteTextures;
    renderPass["resizeWriteBuffers"]   = &RenderPass::resizeWriteBuffers;
    renderPass["execute"]              = &RenderPass::execute;
  }

  // RenderProcess
  {
    {
      auto bloomRenderProcess = state.new_usertype<BloomRenderProcess>("BloomRenderProcess",
                                                                       sol::constructors<BloomRenderProcess(RenderGraph&)>(),
                                                                       sol::base_classes, sol::bases<RenderProcess>());
      bloomRenderProcess["getThresholdPass"]        = &BloomRenderProcess::getThresholdPass;
      bloomRenderProcess["getDownscalePassCount"]   = &BloomRenderProcess::getDownscalePassCount;
      bloomRenderProcess["getDownscalePass"]        = PickNonConstOverload<std::size_t>(&BloomRenderProcess::getDownscalePass);
      bloomRenderProcess["getDownscaleBufferCount"] = &BloomRenderProcess::getDownscaleBufferCount;
      bloomRenderProcess["getDownscaleBuffer"]      = [] (BloomRenderProcess& b, std::size_t i) { return &b.getDownscaleBuffer(i); };
      bloomRenderProcess["getUpscalePassCount"]     = &BloomRenderProcess::getUpscalePassCount;
      bloomRenderProcess["getUpscalePass"]          = PickNonConstOverload<std::size_t>(&BloomRenderProcess::getUpscalePass);
      bloomRenderProcess["getUpscaleBufferCount"]   = &BloomRenderProcess::getUpscaleBufferCount;
      bloomRenderProcess["getUpscaleBuffer"]        = [] (BloomRenderProcess& b, std::size_t i) { return &b.getUpscaleBuffer(i); };
      bloomRenderProcess["setInputColorBuffer"]     = &BloomRenderProcess::setInputColorBuffer;
      bloomRenderProcess["setOutputBuffer"]         = &BloomRenderProcess::setOutputBuffer;
      bloomRenderProcess["setThresholdValue"]       = &BloomRenderProcess::setThresholdValue;
    }

    {
      auto boxBlurRenderProcess = state.new_usertype<BoxBlurRenderProcess>("BoxBlurRenderProcess",
                                                                           sol::constructors<BoxBlurRenderProcess(RenderGraph&)>(),
                                                                           sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      boxBlurRenderProcess["setInputBuffer"]  = &BoxBlurRenderProcess::setInputBuffer;
      boxBlurRenderProcess["setOutputBuffer"] = &BoxBlurRenderProcess::setOutputBuffer;
      boxBlurRenderProcess["setStrength"]     = &BoxBlurRenderProcess::setStrength;
    }

    {
      auto chromAberrRenderProcess = state.new_usertype<ChromaticAberrationRenderProcess>("ChromaticAberrationRenderProcess",
                                                                                          sol::constructors<ChromaticAberrationRenderProcess(RenderGraph&)>(),
                                                                                          sol::base_classes, sol::bases<MonoPassRenderProcess,
                                                                                                                        RenderProcess>());
      chromAberrRenderProcess["setInputBuffer"]  = &ChromaticAberrationRenderProcess::setInputBuffer;
      chromAberrRenderProcess["setOutputBuffer"] = &ChromaticAberrationRenderProcess::setOutputBuffer;
      chromAberrRenderProcess["setStrength"]     = &ChromaticAberrationRenderProcess::setStrength;
      chromAberrRenderProcess["setDirection"]    = &ChromaticAberrationRenderProcess::setDirection;
      chromAberrRenderProcess["setMaskTexture"]  = &ChromaticAberrationRenderProcess::setMaskTexture;
    }

    {
      auto convolutionRenderProcess = state.new_usertype<ConvolutionRenderProcess>("ConvolutionRenderProcess",
                                                                                   sol::constructors<ConvolutionRenderProcess(RenderGraph&, const Mat3f&),
                                                                                                     ConvolutionRenderProcess(RenderGraph&, const Mat3f&,
                                                                                                                              std::string)>(),
                                                                                   sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      convolutionRenderProcess["setInputBuffer"]  = &ConvolutionRenderProcess::setInputBuffer;
      convolutionRenderProcess["setOutputBuffer"] = &ConvolutionRenderProcess::setOutputBuffer;
      convolutionRenderProcess["setKernel"]       = &ConvolutionRenderProcess::setKernel;
    }

    {
      auto gaussianBlurRenderProcess = state.new_usertype<GaussianBlurRenderProcess>("GaussianBlurRenderProcess",
                                                                                     sol::constructors<GaussianBlurRenderProcess(RenderGraph&)>(),
                                                                                     sol::base_classes, sol::bases<RenderProcess>());
      gaussianBlurRenderProcess["getHorizontalPass"] = [] (GaussianBlurRenderProcess& gb) { return &gb.getHorizontalPass(); };
      gaussianBlurRenderProcess["getVerticalPass"]   = [] (GaussianBlurRenderProcess& gb) { return &gb.getVerticalPass(); };
      gaussianBlurRenderProcess["setInputBuffer"]    = &GaussianBlurRenderProcess::setInputBuffer;
      gaussianBlurRenderProcess["setOutputBuffer"]   = &GaussianBlurRenderProcess::setOutputBuffer;
    }

    {
      state.new_usertype<MonoPassRenderProcess>("MonoPassRenderProcess", sol::no_constructor, sol::base_classes, sol::bases<RenderProcess>());
    }

    {
      sol::usertype<RenderProcess> renderProcess = state.new_usertype<RenderProcess>("RenderProcess", sol::no_constructor);
      renderProcess["isEnabled"]          = &RenderProcess::isEnabled;
      renderProcess["setState"]           = &RenderProcess::setState;
      renderProcess["enable"]             = &RenderProcess::enable;
      renderProcess["disable"]            = &RenderProcess::disable;
      renderProcess["addParent"]          = sol::overload(PickOverload<RenderPass&>(&RenderProcess::addParent),
                                                          PickOverload<RenderProcess&>(&RenderProcess::addParent));
      renderProcess["addChild"]           = sol::overload(PickOverload<RenderPass&>(&RenderProcess::addChild),
                                                          PickOverload<RenderProcess&>(&RenderProcess::addChild));
      renderProcess["resizeBuffers"]      = &RenderProcess::resizeBuffers;
      renderProcess["recoverElapsedTime"] = &RenderProcess::recoverElapsedTime;
    }

    {
      auto ssrRenderProcess = state.new_usertype<SsrRenderProcess>("SsrRenderProcess",
                                                                   sol::constructors<SsrRenderProcess(RenderGraph&)>(),
                                                                   sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      ssrRenderProcess["setInputDepthBuffer"]        = &SsrRenderProcess::setInputDepthBuffer;
      ssrRenderProcess["setInputColorBuffer"]        = &SsrRenderProcess::setInputColorBuffer;
      ssrRenderProcess["setInputBlurredColorBuffer"] = &SsrRenderProcess::setInputBlurredColorBuffer;
      ssrRenderProcess["setInputNormalBuffer"]       = &SsrRenderProcess::setInputNormalBuffer;
      ssrRenderProcess["setInputSpecularBuffer"]     = &SsrRenderProcess::setInputSpecularBuffer;
      ssrRenderProcess["setOutputBuffer"]            = &SsrRenderProcess::setOutputBuffer;
    }

    {
      auto vignetteRenderProcess = state.new_usertype<VignetteRenderProcess>("VignetteRenderProcess",
                                                                             sol::constructors<VignetteRenderProcess(RenderGraph&)>(),
                                                                             sol::base_classes, sol::bases<MonoPassRenderProcess, RenderProcess>());
      vignetteRenderProcess["setInputBuffer"]  = &VignetteRenderProcess::setInputBuffer;
      vignetteRenderProcess["setOutputBuffer"] = &VignetteRenderProcess::setOutputBuffer;
      vignetteRenderProcess["setStrength"]     = &VignetteRenderProcess::setStrength;
      vignetteRenderProcess["setOpacity"]      = &VignetteRenderProcess::setOpacity;
      vignetteRenderProcess["setColor"]        = &VignetteRenderProcess::setColor;
    }
  }

  {
    sol::usertype<RenderSystem> renderSystem = state.new_usertype<RenderSystem>("RenderSystem",
                                                                                sol::constructors<RenderSystem(),
                                                                                                  RenderSystem(unsigned int, unsigned int)
#if !defined(RAZ_NO_WINDOW)
                                                                                                  ,
                                                                                                  RenderSystem(unsigned int, unsigned int, const std::string&),
                                                                                                  RenderSystem(unsigned int, unsigned int, const std::string&,
                                                                                                               WindowSetting),
                                                                                                  RenderSystem(unsigned int, unsigned int, const std::string&,
                                                                                                               WindowSetting, uint8_t)
#endif
                                                                                                  >(),
                                                                                sol::base_classes, sol::bases<System>());
#if !defined(RAZ_NO_WINDOW)
    renderSystem["hasWindow"]          = &RenderSystem::hasWindow;
    renderSystem["getWindow"]          = PickNonConstOverload<>(&RenderSystem::getWindow);
#endif
    renderSystem["getGeometryPass"]    = PickNonConstOverload<>(&RenderSystem::getGeometryPass);
    renderSystem["getRenderGraph"]     = PickNonConstOverload<>(&RenderSystem::getRenderGraph);
    renderSystem["hasCubemap"]         = &RenderSystem::hasCubemap;
    renderSystem["getCubemap"]         = [] (const RenderSystem& r) { return &r.getCubemap(); };
    renderSystem["setCubemap"]         = [] (RenderSystem& r, Cubemap& c) { r.setCubemap(std::move(c)); };
#if !defined(RAZ_NO_WINDOW)
    renderSystem["createWindow"]       = sol::overload([] (RenderSystem& r, unsigned int w, unsigned int h) { r.createWindow(w, h); },
                                                       [] (RenderSystem& r, unsigned int w, unsigned int h, const std::string& t) { r.createWindow(w, h, t); },
                                                       [] (RenderSystem& r, unsigned int w, unsigned int h, const std::string& t,
                                                           WindowSetting s) { r.createWindow(w, h, t, s); },
                                                       PickOverload<unsigned int, unsigned int, const std::string&, WindowSetting,
                                                                    uint8_t>(&RenderSystem::createWindow));
#endif
    renderSystem["resizeViewport"]     = &RenderSystem::resizeViewport;
    renderSystem["sendCameraMatrices"] = &RenderSystem::sendCameraMatrices;
    renderSystem["updateLights"]       = &RenderSystem::updateLights;
    renderSystem["updateShaders"]      = &RenderSystem::updateShaders;
    renderSystem["updateMaterials"]    = sol::overload([] (const RenderSystem& r) { r.updateMaterials(); },
                                                       PickOverload<const MeshRenderer&>(&RenderSystem::updateMaterials));
    renderSystem["saveToImage"]        = sol::overload([] (const RenderSystem& r, const FilePath& p) { r.saveToImage(p); },
                                                       [] (const RenderSystem& r, const FilePath& p, TextureFormat f) { r.saveToImage(p, f); },
                                                       PickOverload<const FilePath&, TextureFormat, PixelDataType>(&RenderSystem::saveToImage));
    renderSystem["removeCubemap"]      = &RenderSystem::removeCubemap;

    state.new_enum<TextureFormat>("TextureFormat", {
      { "RED",           TextureFormat::RED },
      { "GREEN",         TextureFormat::GREEN },
      { "BLUE",          TextureFormat::BLUE },
      { "RG",            TextureFormat::RG },
      { "RGB",           TextureFormat::RGB },
      { "BGR",           TextureFormat::BGR },
      { "RGBA",          TextureFormat::RGBA },
      { "BGRA",          TextureFormat::BGRA },
      { "SRGB",          TextureFormat::SRGB },
      { "DEPTH",         TextureFormat::DEPTH },
      { "STENCIL",       TextureFormat::STENCIL },
      { "DEPTH_STENCIL", TextureFormat::DEPTH_STENCIL }
    });

    state.new_enum<PixelDataType>("PixelDataType", {
      { "UBYTE", PixelDataType::UBYTE },
      { "FLOAT", PixelDataType::FLOAT }
    });
  }

  // Shaders
  {
    {
      sol::usertype<ComputeShader> computeShader = state.new_usertype<ComputeShader>("ComputeShader",
                                                                                     sol::constructors<ComputeShader(),
                                                                                                       ComputeShader(FilePath)>(),
                                                                                     sol::base_classes, sol::bases<Shader>());
      computeShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&ComputeShader::loadFromSource),
                                                      PickOverload<const char*>(&ComputeShader::loadFromSource));
      computeShader["clone"]          = &ComputeShader::clone;
    }

    {
      sol::usertype<FragmentShader> fragmentShader = state.new_usertype<FragmentShader>("FragmentShader",
                                                                                        sol::constructors<FragmentShader(),
                                                                                                          FragmentShader(FilePath)>(),
                                                                                        sol::base_classes, sol::bases<Shader>());
      fragmentShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&FragmentShader::loadFromSource),
                                                       PickOverload<const char*>(&FragmentShader::loadFromSource));
      fragmentShader["clone"]          = &FragmentShader::clone;
    }

    {
      sol::usertype<GeometryShader> geometryShader = state.new_usertype<GeometryShader>("GeometryShader",
                                                                                        sol::constructors<GeometryShader(),
                                                                                                          GeometryShader(FilePath)>(),
                                                                                        sol::base_classes, sol::bases<Shader>());
      geometryShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&GeometryShader::loadFromSource),
                                                       PickOverload<const char*>(&GeometryShader::loadFromSource));
      geometryShader["clone"]          = &GeometryShader::clone;
    }

    {
      sol::usertype<Shader> shader = state.new_usertype<Shader>("Shader", sol::no_constructor);
      shader["getIndex"]   = &Shader::getIndex;
      shader["getPath"]    = &Shader::getPath;
      shader["isValid"]    = &Shader::isValid;
      shader["import"]     = &Shader::import;
      shader["load"]       = &Shader::load;
      shader["compile"]    = &Shader::compile;
      shader["isCompiled"] = &Shader::isCompiled;
      shader["destroy"]    = &Shader::destroy;
    }

    {
      sol::usertype<TessellationControlShader> tessCtrlShader = state.new_usertype<TessellationControlShader>("TessellationControlShader",
                                                                                                              sol::constructors<
                                                                                                                TessellationControlShader(),
                                                                                                                TessellationControlShader(FilePath)
                                                                                                              >(),
                                                                                                              sol::base_classes, sol::bases<Shader>());
      tessCtrlShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&TessellationControlShader::loadFromSource),
                                                       PickOverload<const char*>(&TessellationControlShader::loadFromSource));
      tessCtrlShader["clone"]          = &TessellationControlShader::clone;
    }

    {
      sol::usertype<TessellationEvaluationShader> tessEvalShader = state.new_usertype<TessellationEvaluationShader>("TessellationEvaluationShader",
                                                                                                                    sol::constructors<
                                                                                                                      TessellationEvaluationShader(),
                                                                                                                      TessellationEvaluationShader(FilePath)
                                                                                                                    >(),
                                                                                                                    sol::base_classes, sol::bases<Shader>());
      tessEvalShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&TessellationEvaluationShader::loadFromSource),
                                                       PickOverload<const char*>(&TessellationEvaluationShader::loadFromSource));
      tessEvalShader["clone"]          = &TessellationEvaluationShader::clone;
    }

    {
      sol::usertype<VertexShader> vertexShader = state.new_usertype<VertexShader>("VertexShader",
                                                                                  sol::constructors<VertexShader(),
                                                                                                    VertexShader(FilePath)>(),
                                                                                  sol::base_classes, sol::bases<Shader>());
      vertexShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&VertexShader::loadFromSource),
                                                     PickOverload<const char*>(&VertexShader::loadFromSource));
      vertexShader["clone"]          = &VertexShader::clone;
    }
  }

  // ShaderProgram
  {
#if !defined(USE_OPENGL_ES)
    {
      sol::usertype<ComputeShaderProgram> computeShaderProgram = state.new_usertype<ComputeShaderProgram>("ComputeShaderProgram",
                                                                                                          sol::constructors<ComputeShaderProgram()>(),
                                                                                                          sol::base_classes, sol::bases<ShaderProgram>());
      computeShaderProgram["getShader"]     = [] (const ComputeShaderProgram& p) { return &p.getShader(); };
      computeShaderProgram["setShader"]     = [] (ComputeShaderProgram& p, ComputeShader& s) { p.setShader(std::move(s)); };
      computeShaderProgram["clone"]         = &ComputeShaderProgram::clone;
      computeShaderProgram["execute"]       = sol::overload([] (const ComputeShaderProgram& p, unsigned int x) { p.execute(x); },
                                                            [] (const ComputeShaderProgram& p, unsigned int x, unsigned int y) { p.execute(x, y); },
                                                            PickOverload<unsigned int, unsigned int, unsigned int>(&ComputeShaderProgram::execute));
      computeShaderProgram["destroyShader"] = &ComputeShaderProgram::destroyShader;
    }
#endif

    {
      sol::usertype<RenderShaderProgram> renderShaderProgram = state.new_usertype<RenderShaderProgram>("RenderShaderProgram",
                                                                                                       sol::constructors<RenderShaderProgram()>(),
                                                                                                       sol::base_classes, sol::bases<ShaderProgram>());
      renderShaderProgram["getVertexShader"]                     = [] (const RenderShaderProgram& p) { return &p.getVertexShader(); };
      renderShaderProgram["hasTessellationControlShader"]        = &RenderShaderProgram::hasTessellationControlShader;
      renderShaderProgram["getTessellationControlShader"]        = [] (const RenderShaderProgram& p) { return &p.getTessellationControlShader(); };
      renderShaderProgram["hasTessellationEvaluationShader"]     = &RenderShaderProgram::hasTessellationEvaluationShader;
      renderShaderProgram["getTessellationEvaluationShader"]     = [] (const RenderShaderProgram& p) { return &p.getTessellationEvaluationShader(); };
      renderShaderProgram["hasGeometryShader"]                   = &RenderShaderProgram::hasGeometryShader;
      renderShaderProgram["getGeometryShader"]                   = [] (const RenderShaderProgram& p) { return &p.getGeometryShader(); };
      renderShaderProgram["getFragmentShader"]                   = [] (const RenderShaderProgram& p) { return &p.getFragmentShader(); };
      renderShaderProgram["setVertexShader"]                     = [] (RenderShaderProgram& p, VertexShader& s) { p.setVertexShader(std::move(s)); };
      renderShaderProgram["setTessellationControlShader"]        = [] (RenderShaderProgram& p,
                                                                       TessellationControlShader& s) { p.setTessellationControlShader(std::move(s)); };
      renderShaderProgram["setTessellationEvaluationShader"]     = [] (RenderShaderProgram& p,
                                                                       TessellationEvaluationShader& s) { p.setTessellationEvaluationShader(std::move(s)); };
      renderShaderProgram["setGeometryShader"]                   = [] (RenderShaderProgram& p, GeometryShader& s) { p.setGeometryShader(std::move(s)); };
      renderShaderProgram["setFragmentShader"]                   = [] (RenderShaderProgram& p, FragmentShader& s) { p.setFragmentShader(std::move(s)); };
      renderShaderProgram["setShaders"]                          = sol::overload([] (RenderShaderProgram& p, VertexShader& v,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, GeometryShader& g,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(g),
                                                                                                                       std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, TessellationEvaluationShader& te,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(te),
                                                                                                                       std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, TessellationControlShader& tc,
                                                                                     TessellationEvaluationShader& te,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(tc),
                                                                                                                       std::move(te), std::move(f)); });
      renderShaderProgram["clone"]                               = &RenderShaderProgram::clone;
      renderShaderProgram["destroyVertexShader"]                 = &RenderShaderProgram::destroyVertexShader;
      renderShaderProgram["destroyTessellationControlShader"]    = &RenderShaderProgram::destroyTessellationControlShader;
      renderShaderProgram["destroyTessellationEvaluationShader"] = &RenderShaderProgram::destroyTessellationEvaluationShader;
      renderShaderProgram["destroyGeometryShader"]               = &RenderShaderProgram::destroyGeometryShader;
      renderShaderProgram["destroyFragmentShader"]               = &RenderShaderProgram::destroyFragmentShader;
    }

    {
      sol::usertype<ShaderProgram> shaderProgram = state.new_usertype<ShaderProgram>("ShaderProgram", sol::no_constructor);
      shaderProgram["hasAttribute"]           = [] (const ShaderProgram& p, const std::string& n) { return p.hasAttribute(n); };
      shaderProgram["getAttributeCount"]      = &ShaderProgram::getAttributeCount;
      shaderProgram["hasTexture"]             = sol::overload(PickOverload<const Texture&>(&ShaderProgram::hasTexture),
                                                              PickOverload<const std::string&>(&ShaderProgram::hasTexture));
      shaderProgram["getTextureCount"]        = &ShaderProgram::getTextureCount;
      shaderProgram["getTexture"]             = sol::overload([] (const ShaderProgram& p, std::size_t i) { return &p.getTexture(i); },
                                                              [] (const ShaderProgram& p, const std::string& n) { return &p.getTexture(n); });
      shaderProgram["setIntAttribute"]        = &ShaderProgram::setAttribute<int>;
      shaderProgram["setUintAttribute"]       = &ShaderProgram::setAttribute<unsigned int>;
      shaderProgram["setFloatAttribute"]      = &ShaderProgram::setAttribute<float>;
      shaderProgram["setAttribute"]           = sol::overload(&ShaderProgram::setAttribute<const Vec2i&>,
                                                              &ShaderProgram::setAttribute<const Vec3i&>,
                                                              &ShaderProgram::setAttribute<const Vec4i&>,
                                                              &ShaderProgram::setAttribute<const Vec2u&>,
                                                              &ShaderProgram::setAttribute<const Vec3u&>,
                                                              &ShaderProgram::setAttribute<const Vec4u&>,
                                                              &ShaderProgram::setAttribute<const Vec2f&>,
                                                              &ShaderProgram::setAttribute<const Vec3f&>,
                                                              &ShaderProgram::setAttribute<const Vec4f&>,
                                                              &ShaderProgram::setAttribute<const Mat2f&>,
                                                              &ShaderProgram::setAttribute<const Mat3f&>,
                                                              &ShaderProgram::setAttribute<const Mat4f&>);
      // Sol does not seem to be able to bind shared pointers from derived classes to a shared pointer of the base class
      //   (e.g., Texture2DPtr cannot be given directly to setTexture(), which takes a TexturePtr)
      shaderProgram["setTexture"]             = sol::overload(
#if !defined(USE_OPENGL_ES)
                                                              [] (ShaderProgram& p, Texture1DPtr t, const std::string& n) { p.setTexture(std::move(t), n); },
#endif
                                                              [] (ShaderProgram& p, Texture2DPtr t, const std::string& n) { p.setTexture(std::move(t), n); },
                                                              [] (ShaderProgram& p, Texture3DPtr t, const std::string& n) { p.setTexture(std::move(t), n); });
      shaderProgram["loadShaders"]            = &ShaderProgram::loadShaders;
      shaderProgram["compileShaders"]         = &ShaderProgram::compileShaders;
      shaderProgram["link"]                   = &ShaderProgram::link;
      shaderProgram["isLinked"]               = &ShaderProgram::isLinked;
      shaderProgram["updateShaders"]          = &ShaderProgram::updateShaders;
      shaderProgram["use"]                    = &ShaderProgram::use;
      shaderProgram["isUsed"]                 = &ShaderProgram::isUsed;
      shaderProgram["sendAttributes"]         = &ShaderProgram::sendAttributes;
      shaderProgram["removeAttribute"]        = &ShaderProgram::removeAttribute;
      shaderProgram["clearAttributes"]        = &ShaderProgram::clearAttributes;
      shaderProgram["initTextures"]           = &ShaderProgram::initTextures;
      shaderProgram["bindTextures"]           = &ShaderProgram::bindTextures;
      shaderProgram["removeTexture"]          = sol::overload(PickOverload<const Texture&>(&ShaderProgram::removeTexture),
                                                              PickOverload<const std::string&>(&ShaderProgram::removeTexture));
      shaderProgram["clearTextures"]          = &ShaderProgram::clearTextures;
      shaderProgram["recoverUniformLocation"] = &ShaderProgram::recoverUniformLocation;
    }
  }

  {
    sol::usertype<SubmeshRenderer> submeshRenderer = state.new_usertype<SubmeshRenderer>("SubmeshRenderer",
                                                                                         sol::constructors<SubmeshRenderer(),
                                                                                                           SubmeshRenderer(const Submesh&),
                                                                                                           SubmeshRenderer(const Submesh&, RenderMode)>());
    submeshRenderer["getRenderMode"] = &SubmeshRenderer::getRenderMode;
    submeshRenderer["setRenderMode"] = &SubmeshRenderer::setRenderMode;
    submeshRenderer["materialIndex"] = sol::property(&SubmeshRenderer::getMaterialIndex, &SubmeshRenderer::setMaterialIndex);
    submeshRenderer["clone"]         = &SubmeshRenderer::clone;
    submeshRenderer["load"]          = sol::overload([] (SubmeshRenderer& r, const Submesh& s) { r.load(s); },
                                                     PickOverload<const Submesh&, RenderMode>(&SubmeshRenderer::load));
    submeshRenderer["draw"]          = &SubmeshRenderer::draw;

    state.new_enum<RenderMode>("RenderMode", {
      { "POINT",    RenderMode::POINT },
      { "TRIANGLE", RenderMode::TRIANGLE },
#if !defined(USE_OPENGL_ES)
      { "PATCH",    RenderMode::PATCH }
#endif
    });
  }

  // Texture
  {
    {
      sol::usertype<Texture> texture = state.new_usertype<Texture>("Texture", sol::no_constructor);
      texture["getIndex"]      = &Texture::getIndex;
      texture["getColorspace"] = &Texture::getColorspace;
      texture["getDataType"]   = &Texture::getDataType;
      texture["bind"]          = &Texture::bind;
      texture["unbind"]        = &Texture::unbind;
      texture["setFilter"]     = sol::overload(PickOverload<TextureFilter>(&Texture::setFilter),
                                               PickOverload<TextureFilter, TextureFilter>(&Texture::setFilter),
                                               PickOverload<TextureFilter, TextureFilter, TextureFilter>(&Texture::setFilter));
      texture["setWrapping"]   = &Texture::setWrapping;
      texture["setColorspace"] = sol::overload(PickOverload<TextureColorspace>(&Texture::setColorspace),
                                               PickOverload<TextureColorspace, TextureDataType>(&Texture::setColorspace));
    }

#if !defined(USE_OPENGL_ES)
    {
      sol::usertype<Texture1D> texture1D = state.new_usertype<Texture1D>("Texture1D",
                                                                         sol::constructors<Texture1D(TextureColorspace),
                                                                                           Texture1D(TextureColorspace, TextureDataType),
                                                                                           Texture1D(unsigned int, TextureColorspace),
                                                                                           Texture1D(unsigned int, TextureColorspace, TextureDataType),
                                                                                           Texture1D(const Color&),
                                                                                           Texture1D(const Color&, unsigned int)>(),
                                                                         sol::base_classes, sol::bases<Texture>());
      texture1D["getWidth"] = &Texture1D::getWidth;
      texture1D["create"]   = sol::overload(&Texture1D::create<TextureColorspace>,
                                            &Texture1D::create<TextureColorspace, TextureDataType>,
                                            &Texture1D::create<unsigned int, TextureColorspace>,
                                            &Texture1D::create<unsigned int, TextureColorspace, TextureDataType>,
                                            &Texture1D::create<const Color&>,
                                            &Texture1D::create<const Color&, unsigned int>);
      texture1D["resize"]   = &Texture1D::resize;
      texture1D["fill"]     = &Texture1D::fill;
    }
#endif

    {
      sol::usertype<Texture2D> texture2D = state.new_usertype<Texture2D>("Texture2D",
                                                                         sol::constructors<Texture2D(TextureColorspace),
                                                                                           Texture2D(TextureColorspace, TextureDataType),
                                                                                           Texture2D(unsigned int, unsigned int, TextureColorspace),
                                                                                           Texture2D(unsigned int, unsigned int,
                                                                                                     TextureColorspace, TextureDataType),
                                                                                           Texture2D(const Image&),
                                                                                           Texture2D(const Image&, bool),
                                                                                           Texture2D(const Color&),
                                                                                           Texture2D(const Color&, unsigned int),
                                                                                           Texture2D(const Color&, unsigned int, unsigned int)>(),
                                                                         sol::base_classes, sol::bases<Texture>());
      texture2D["getWidth"]     = &Texture2D::getWidth;
      texture2D["getHeight"]    = &Texture2D::getHeight;
      texture2D["create"]       = sol::overload(&Texture2D::create<TextureColorspace>,
                                                &Texture2D::create<TextureColorspace, TextureDataType>,
                                                &Texture2D::create<unsigned int, unsigned int, TextureColorspace>,
                                                &Texture2D::create<unsigned int, unsigned int, TextureColorspace, TextureDataType>,
                                                &Texture2D::create<const Image&>,
                                                &Texture2D::create<const Image&, bool>,
                                                &Texture2D::create<const Color&>,
                                                &Texture2D::create<const Color&, unsigned int>,
                                                &Texture2D::create<const Color&, unsigned int, unsigned int>);
      texture2D["resize"]       = &Texture2D::resize;
      texture2D["load"]         = sol::overload([] (Texture2D& t, const Image& img) { t.load(img); },
                                                PickOverload<const Image&, bool>(&Texture2D::load));
      texture2D["fill"]         = &Texture2D::fill;
#if !defined(USE_OPENGL_ES)
      texture2D["recoverImage"] = &Texture2D::recoverImage;
#endif
    }

    {
      sol::usertype<Texture3D> texture3D = state.new_usertype<Texture3D>("Texture3D",
                                                                         sol::constructors<Texture3D(TextureColorspace),
                                                                                           Texture3D(TextureColorspace, TextureDataType),
                                                                                           Texture3D(unsigned int, unsigned int, unsigned int,
                                                                                                     TextureColorspace),
                                                                                           Texture3D(unsigned int, unsigned int, unsigned int,
                                                                                                     TextureColorspace, TextureDataType),
                                                                                           Texture3D(const Color&),
                                                                                           Texture3D(const Color&, unsigned int),
                                                                                           Texture3D(const Color&, unsigned int, unsigned int),
                                                                                           Texture3D(const Color&, unsigned int, unsigned int, unsigned int)>(),
                                                                         sol::base_classes, sol::bases<Texture>());
      texture3D["getWidth"]  = &Texture3D::getWidth;
      texture3D["getHeight"] = &Texture3D::getHeight;
      texture3D["getDepth"]  = &Texture3D::getDepth;
      texture3D["create"]    = sol::overload(&Texture3D::create<TextureColorspace>,
                                             &Texture3D::create<TextureColorspace, TextureDataType>,
                                             &Texture3D::create<unsigned int, unsigned int, unsigned int, TextureColorspace>,
                                             &Texture3D::create<unsigned int, unsigned int, unsigned int, TextureColorspace, TextureDataType>,
                                             &Texture3D::create<const Color&>,
                                             &Texture3D::create<const Color&, unsigned int>,
                                             &Texture3D::create<const Color&, unsigned int, unsigned int>,
                                             &Texture3D::create<const Color&, unsigned int, unsigned int, unsigned int>);
      texture3D["resize"]    = &Texture3D::resize;
      texture3D["fill"]      = &Texture3D::fill;
    }

    state.new_enum<TextureColorspace>("TextureColorspace", {
      { "INVALID", TextureColorspace::INVALID },
      { "GRAY",    TextureColorspace::GRAY },
      { "RG",      TextureColorspace::RG },
      { "RGB",     TextureColorspace::RGB },
      { "RGBA",    TextureColorspace::RGBA },
      { "SRGB",    TextureColorspace::SRGB },
      { "SRGBA",   TextureColorspace::SRGBA },
      { "DEPTH",   TextureColorspace::DEPTH }
    });

    state.new_enum<TextureDataType>("TextureDataType", {
      { "BYTE",    TextureDataType::BYTE },
      { "FLOAT16", TextureDataType::FLOAT16 },
      { "FLOAT32", TextureDataType::FLOAT32 }
    });

    state.new_enum<TextureFilter>("TextureFilter", {
      { "NEAREST", TextureFilter::NEAREST },
      { "LINEAR",  TextureFilter::LINEAR }
    });

    state.new_enum<TextureWrapping>("TextureWrapping", {
      { "REPEAT", TextureWrapping::REPEAT },
      { "CLAMP",  TextureWrapping::CLAMP }
    });
  }

  {
    sol::usertype<UniformBuffer> uniformBuffer = state.new_usertype<UniformBuffer>("UniformBuffer",
                                                                                   sol::constructors<UniformBuffer(unsigned int),
                                                                                                     UniformBuffer(unsigned int, UniformBufferUsage)>());
    uniformBuffer["getIndex"]         = &UniformBuffer::getIndex;
    uniformBuffer["bindUniformBlock"] = sol::overload(PickOverload<const ShaderProgram&, unsigned int, unsigned int>(&UniformBuffer::bindUniformBlock),
                                                      PickOverload<const ShaderProgram&, const std::string&, unsigned int>(&UniformBuffer::bindUniformBlock));
    uniformBuffer["bindBase"]         = &UniformBuffer::bindBase;
    uniformBuffer["bindRange"]        = &UniformBuffer::bindRange;
    uniformBuffer["bind"]             = &UniformBuffer::bind;
    uniformBuffer["unbind"]           = &UniformBuffer::unbind;
    uniformBuffer["sendIntData"]      = &UniformBuffer::sendData<int>;
    uniformBuffer["sendUintData"]     = &UniformBuffer::sendData<unsigned int>;
    uniformBuffer["sendFloatData"]    = &UniformBuffer::sendData<float>;
    uniformBuffer["sendData"]         = sol::overload(&UniformBuffer::sendData<const Vec2i&>,
                                                      &UniformBuffer::sendData<const Vec3i&>,
                                                      &UniformBuffer::sendData<const Vec4i&>,
                                                      &UniformBuffer::sendData<const Vec2u&>,
                                                      &UniformBuffer::sendData<const Vec3u&>,
                                                      &UniformBuffer::sendData<const Vec4u&>,
                                                      &UniformBuffer::sendData<const Vec2f&>,
                                                      &UniformBuffer::sendData<const Vec3f&>,
                                                      &UniformBuffer::sendData<const Vec4f&>,
                                                      &UniformBuffer::sendData<const Mat2f&>,
                                                      &UniformBuffer::sendData<const Mat3f&>,
                                                      &UniformBuffer::sendData<const Mat4f&>);

    state.new_enum<UniformBufferUsage>("UniformBufferUsage", {
      { "STATIC",  UniformBufferUsage::STATIC },
      { "DYNAMIC", UniformBufferUsage::DYNAMIC },
      { "STREAM",  UniformBufferUsage::STREAM }
    });
  }

  {
    sol::usertype<Window> window = state.new_usertype<Window>("Window",
                                                              sol::constructors<Window(RenderSystem&, unsigned int, unsigned int),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&,
                                                                                       WindowSetting),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&,
                                                                                       WindowSetting, uint8_t)>());
    window["getWidth"]                 = &Window::getWidth;
    window["getHeight"]                = &Window::getHeight;
#if !defined(RAZ_NO_OVERLAY)
    window["getOverlay"]               = &Window::getOverlay;
#endif
    window["setClearColor"]            = sol::overload(PickOverload<const Vec4f&>(&Window::setClearColor),
                                                       [] (const Window& w, float r, float g, float b) { w.setClearColor(r, g, b); },
                                                       PickOverload<float, float, float, float>(&Window::setClearColor));
    window["setTitle"]                 = &Window::setTitle;
    window["setIcon"]                  = &Window::setIcon;
    window["resize"]                   = &Window::resize;
    window["makeFullscreen"]           = &Window::makeFullscreen;
    window["makeWindowed"]             = &Window::makeWindowed;
    window["enableFaceCulling"]        = sol::overload([] (const Window& w) { w.enableFaceCulling(); },
                                                       PickOverload<bool>(&Window::enableFaceCulling));
    window["disableFaceCulling"]       = &Window::disableFaceCulling;
    window["recoverVerticalSyncState"] = &Window::recoverVerticalSyncState;
    window["enableVerticalSync"]       = sol::overload([] (const Window& w) { w.enableVerticalSync(); },
                                                       PickOverload<bool>(&Window::enableVerticalSync));
    window["disableVerticalSync"]      = &Window::disableVerticalSync;
    window["showCursor"]               = &Window::showCursor;
    window["hideCursor"]               = &Window::hideCursor;
    window["disableCursor"]            = &Window::disableCursor;
    window["addKeyCallback"]           = sol::overload([] (Window& w, Keyboard::Key k, std::function<void(float)> c) { w.addKeyCallback(k, std::move(c)); },
                                                       [] (Window& w, Keyboard::Key k, std::function<void(float)> c,
                                                           Input::ActionTrigger f) { w.addKeyCallback(k, std::move(c), f); },
                                                       PickOverload<Keyboard::Key, std::function<void(float)>,
                                                                    Input::ActionTrigger, std::function<void()>>(&Window::addKeyCallback));
    window["addMouseButtonCallback"]   = sol::overload([] (Window& w, Mouse::Button b,
                                                           std::function<void(float)> c) { w.addMouseButtonCallback(b, std::move(c)); },
                                                       [] (Window& w, Mouse::Button b, std::function<void(float)> c,
                                                           Input::ActionTrigger f) { w.addMouseButtonCallback(b, std::move(c), f); },
                                                       PickOverload<Mouse::Button, std::function<void(float)>, Input::ActionTrigger,
                                                                    std::function<void()>>(&Window::addMouseButtonCallback));
    window["setMouseScrollCallback"]   = &Window::setMouseScrollCallback;
    window["setMouseMoveCallback"]     = &Window::setMouseMoveCallback;
    window["setCloseCallback"]         = &Window::setCloseCallback;
    window["updateCallbacks"]          = &Window::updateCallbacks;
#if !defined(RAZ_NO_OVERLAY)
    window["enableOverlay"]            = sol::overload([] (Window& w) { w.enableOverlay(); },
                                                       PickOverload<bool>(&Window::enableOverlay));
    window["disableOverlay"]           = &Window::disableOverlay;
#endif
    window["run"]                      = &Window::run;
    window["recoverMousePosition"]     = &Window::recoverMousePosition;
    window["setShouldClose"]           = &Window::setShouldClose;
    window["close"]                    = &Window::close;

    state.new_enum<WindowSetting>("WindowSetting", {
      { "FOCUSED",        WindowSetting::FOCUSED },
      { "RESIZABLE",      WindowSetting::RESIZABLE },
      { "VISIBLE",        WindowSetting::VISIBLE },
      { "DECORATED",      WindowSetting::DECORATED },
      { "AUTO_MINIMIZE",  WindowSetting::AUTO_MINIMIZE },
      { "ALWAYS_ON_TOP",  WindowSetting::ALWAYS_ON_TOP },
      { "MAXIMIZED",      WindowSetting::MAXIMIZED },
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
      { "CENTER_CURSOR",  WindowSetting::CENTER_CURSOR },
      { "TRANSPARENT_FB", WindowSetting::TRANSPARENT_FB },
      { "AUTOFOCUS",      WindowSetting::AUTOFOCUS },
#endif
      { "DEFAULT",        WindowSetting::DEFAULT },
      { "NON_RESIZABLE",  WindowSetting::NON_RESIZABLE },
      { "WINDOWED",       WindowSetting::WINDOWED },
      { "BORDERLESS",     WindowSetting::BORDERLESS },
      { "INVISIBLE",      WindowSetting::INVISIBLE }
    });

    state.new_enum<Input::ActionTrigger>("Input", {
      { "ONCE",   Input::ONCE },
      { "ALWAYS", Input::ALWAYS }
    });

    state.new_enum<Keyboard::Key>("Keyboard", {
      { "A", Keyboard::A },
      { "B", Keyboard::B },
      { "C", Keyboard::C },
      { "D", Keyboard::D },
      { "E", Keyboard::E },
      { "F", Keyboard::F },
      { "G", Keyboard::G },
      { "H", Keyboard::H },
      { "I", Keyboard::I },
      { "J", Keyboard::J },
      { "K", Keyboard::K },
      { "L", Keyboard::L },
      { "M", Keyboard::M },
      { "N", Keyboard::N },
      { "O", Keyboard::O },
      { "P", Keyboard::P },
      { "Q", Keyboard::Q },
      { "R", Keyboard::R },
      { "S", Keyboard::S },
      { "T", Keyboard::T },
      { "U", Keyboard::U },
      { "V", Keyboard::V },
      { "W", Keyboard::W },
      { "X", Keyboard::X },
      { "Y", Keyboard::Y },
      { "Z", Keyboard::Z },

      { "F1",  Keyboard::F1 },
      { "F2",  Keyboard::F2 },
      { "F3",  Keyboard::F3 },
      { "F4",  Keyboard::F4 },
      { "F5",  Keyboard::F5 },
      { "F6",  Keyboard::F6 },
      { "F7",  Keyboard::F7 },
      { "F8",  Keyboard::F8 },
      { "F9",  Keyboard::F9 },
      { "F10", Keyboard::F10 },
      { "F11", Keyboard::F11 },
      { "F12", Keyboard::F12 },

      { "UP",    Keyboard::UP },
      { "DOWN",  Keyboard::DOWN },
      { "RIGHT", Keyboard::RIGHT },
      { "LEFT",  Keyboard::LEFT },

      { "NUMLOCK",   Keyboard::NUMLOCK },
      { "NUM0",      Keyboard::NUM0 },
      { "NUM1",      Keyboard::NUM1 },
      { "NUM2",      Keyboard::NUM2 },
      { "NUM3",      Keyboard::NUM3 },
      { "NUM4",      Keyboard::NUM4 },
      { "NUM5",      Keyboard::NUM5 },
      { "NUM6",      Keyboard::NUM6 },
      { "NUM7",      Keyboard::NUM7 },
      { "NUM8",      Keyboard::NUM8 },
      { "NUM9",      Keyboard::NUM9 },
      { "DECIMAL",   Keyboard::DECIMAL },
      { "DIVIDE",    Keyboard::DIVIDE },
      { "MULTIPLY",  Keyboard::MULTIPLY },
      { "SUBSTRACT", Keyboard::SUBSTRACT },
      { "ADD",       Keyboard::ADD },

      { "LEFT_SHIFT",  Keyboard::LEFT_SHIFT },
      { "RIGHT_SHIFT", Keyboard::RIGHT_SHIFT },
      { "LEFT_CTRL",   Keyboard::LEFT_CTRL },
      { "RIGHT_CTRL",  Keyboard::RIGHT_CTRL },
      { "LEFT_ALT",    Keyboard::LEFT_ALT },
      { "RIGHT_ALT",   Keyboard::RIGHT_ALT },

      { "HOME",         Keyboard::HOME },
      { "END",          Keyboard::END },
      { "PAGEUP",       Keyboard::PAGEUP },
      { "PAGEDOWN",     Keyboard::PAGEDOWN },
      { "CAPSLOCK",     Keyboard::CAPSLOCK },
      { "SPACE",        Keyboard::SPACE },
      { "BACKSPACE",    Keyboard::BACKSPACE },
      { "INSERT",       Keyboard::INSERT },
      { "ESCAPE",       Keyboard::ESCAPE },
      { "PRINT_SCREEN", Keyboard::PRINT_SCREEN },
      { "PAUSE",        Keyboard::PAUSE }
    });

    state.new_enum<Mouse::Button>("Mouse", {
      { "LEFT_CLICK",   Mouse::LEFT_CLICK },
      { "RIGHT_CLICK",  Mouse::RIGHT_CLICK },
      { "MIDDLE_CLICK", Mouse::MIDDLE_CLICK }
    });
  }
}

} // namespace Raz
