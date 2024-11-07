#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerRenderSystemTypes() {
  sol::state& state = getState();

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
    renderSystem["getSceneWidth"]      = &RenderSystem::getSceneWidth;
    renderSystem["getSceneHeight"]     = &RenderSystem::getSceneHeight;
#if !defined(RAZ_NO_WINDOW)
    renderSystem["hasWindow"]          = &RenderSystem::hasWindow;
    renderSystem["getWindow"]          = PickNonConstOverload<>(&RenderSystem::getWindow);
#endif
    renderSystem["getGeometryPass"]    = PickNonConstOverload<>(&RenderSystem::getGeometryPass);
    renderSystem["getRenderGraph"]     = PickNonConstOverload<>(&RenderSystem::getRenderGraph);
    renderSystem["hasCubemap"]         = &RenderSystem::hasCubemap;
    renderSystem["getCubemap"]         = [] (const RenderSystem& r) { return &r.getCubemap(); };
    renderSystem["setCubemap"]         = [] (RenderSystem& r, Cubemap& c) { r.setCubemap(std::move(c)); };
#if defined(RAZ_USE_XR)
    renderSystem["enableXr"]           = [] (RenderSystem& r, XrSystem* x) { r.enableXr(*x); };
#endif
#if !defined(RAZ_NO_WINDOW)
    renderSystem["createWindow"]       = sol::overload([] (RenderSystem& r, unsigned int w, unsigned int h) { r.createWindow(w, h); },
                                                       [] (RenderSystem& r, unsigned int w, unsigned int h, const std::string& t) { r.createWindow(w, h, t); },
                                                       [] (RenderSystem& r, unsigned int w, unsigned int h, const std::string& t,
                                                           WindowSetting s) { r.createWindow(w, h, t, s); },
                                                       PickOverload<unsigned int, unsigned int, const std::string&, WindowSetting,
                                                                    uint8_t>(&RenderSystem::createWindow));
#endif
    renderSystem["resizeViewport"]     = &RenderSystem::resizeViewport;
    renderSystem["updateLights"]       = &RenderSystem::updateLights;
    renderSystem["updateShaders"]      = &RenderSystem::updateShaders;
    renderSystem["updateMaterials"]    = sol::overload([] (const RenderSystem& r) { r.updateMaterials(); },
                                                       [] (const RenderSystem& r, const MeshRenderer* m) { r.updateMaterials(*m); });
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
}

} // namespace Raz
