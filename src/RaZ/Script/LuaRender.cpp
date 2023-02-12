#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"
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
