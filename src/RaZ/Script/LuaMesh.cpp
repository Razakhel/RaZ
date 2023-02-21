#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerMeshTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Mesh> mesh = state.new_usertype<Mesh>("Mesh",
                                                        sol::constructors<Mesh(),
                                                                          Mesh(const Plane&, float, float),
                                                                          Mesh(const Sphere&, uint32_t, SphereMeshType),
                                                                          Mesh(const Triangle&, const Vec2f&, const Vec2f&, const Vec2f&),
                                                                          Mesh(const Quad&),
                                                                          Mesh(const AABB&)>(),
                                                        sol::base_classes, sol::bases<Component>());
    mesh["getSubmeshes"]         = PickNonConstOverload<>(&Mesh::getSubmeshes);
    mesh["getBoundingBox"]       = &Mesh::getBoundingBox;
    mesh["recoverVertexCount"]   = &Mesh::recoverVertexCount;
    mesh["recoverTriangleCount"] = &Mesh::recoverTriangleCount;
    mesh["addSubmesh"]           = &Mesh::addSubmesh<>;
    mesh["computeBoundingBox"]   = &Mesh::computeBoundingBox;
    mesh["computeTangents"]      = &Mesh::computeTangents;

    state.new_enum<SphereMeshType>("SphereMeshType", {
      { "UV",  SphereMeshType::UV },
      { "ICO", SphereMeshType::ICO }
    });
  }

  {
    sol::usertype<Submesh> submesh = state.new_usertype<Submesh>("Submesh",
                                                                 sol::constructors<Submesh()>());
    submesh["getVertices"]           = PickConstOverload<>(&Submesh::getVertices);
    submesh["getVertexCount"]        = &Submesh::getVertexCount;
    submesh["getTriangleIndices"]    = PickConstOverload<>(&Submesh::getTriangleIndices);
    submesh["getTriangleIndexCount"] = &Submesh::getTriangleIndexCount;
    submesh["getBoundingBox"]        = &Submesh::getBoundingBox;
    submesh["computeBoundingBox"]    = &Submesh::computeBoundingBox;

    sol::usertype<Vertex> vertex = state.new_usertype<Vertex>("Vertex",
                                                              sol::constructors<Vertex()>());
    vertex["position"]  = &Vertex::position;
    vertex["texcoords"] = &Vertex::texcoords;
    vertex["normal"]    = &Vertex::normal;
    vertex["tangent"]   = &Vertex::tangent;
  }
}

} // namespace Raz
