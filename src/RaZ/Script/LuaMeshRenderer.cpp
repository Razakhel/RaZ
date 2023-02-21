#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerMeshRendererTypes() {
  sol::state& state = getState();

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
}

} // namespace Raz
