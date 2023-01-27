#include "RaZ/Animation/Skeleton.hpp"
#include "RaZ/Audio/Sound.hpp"
#include "RaZ/Data/Bitset.hpp"
#include "RaZ/Data/BvhFormat.hpp"
#include "RaZ/Data/BvhSystem.hpp"
#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/FbxFormat.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/MeshFormat.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Data/OffFormat.hpp"
#include "RaZ/Data/PngFormat.hpp"
#include "RaZ/Data/TgaFormat.hpp"
#include "RaZ/Data/WavFormat.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerDataTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Bitset> bitset = state.new_usertype<Bitset>("Bitset",
                                                              sol::constructors<Bitset(),
                                                                                Bitset(std::size_t),
                                                                                Bitset(std::size_t, bool)>());
    bitset["getSize"]             = &Bitset::getSize;
    bitset["isEmpty"]             = &Bitset::isEmpty;
    bitset["getEnabledBitCount"]  = &Bitset::getEnabledBitCount;
    bitset["getDisabledBitCount"] = &Bitset::getDisabledBitCount;
    bitset["setBit"]              = sol::overload([] (Bitset& b, std::size_t p) { b.setBit(p); },
                                                  PickOverload<std::size_t, bool>(&Bitset::setBit));
    bitset["resize"]              = &Bitset::resize;
    bitset["reset"]               = &Bitset::reset;
    bitset["clear"]               = &Bitset::clear;
    bitset.set_function(sol::meta_function::bitwise_not, &Bitset::operator~);
    bitset.set_function(sol::meta_function::bitwise_and, &Bitset::operator&);
    bitset.set_function(sol::meta_function::bitwise_or, &Bitset::operator|);
    bitset.set_function(sol::meta_function::bitwise_xor, &Bitset::operator^);
    bitset.set_function(sol::meta_function::bitwise_left_shift, &Bitset::operator<<);
    bitset.set_function(sol::meta_function::bitwise_right_shift, &Bitset::operator>>);
    bitset.set_function(sol::meta_function::index, &Bitset::operator[]);
  }

  {
    sol::table bvhFormat = state["BvhFormat"].get_or_create<sol::table>();
    bvhFormat["load"]    = &BvhFormat::load;
  }

  {
    {
      sol::usertype<BvhNode> bvhNode = state.new_usertype<BvhNode>("BvhNode",
                                                                   sol::constructors<BvhNode()>());
      bvhNode["getBoundingBox"] = &BvhNode::getBoundingBox;
      bvhNode["hasLeftChild"]   = &BvhNode::hasLeftChild;
      bvhNode["getLeftChild"]   = [] (const BvhNode& n) { return &n.getLeftChild(); };
      bvhNode["hasRightChild"]  = &BvhNode::hasRightChild;
      bvhNode["getRightChild"]  = [] (const BvhNode& n) { return &n.getRightChild(); };
      bvhNode["getTriangle"]    = &BvhNode::getTriangle;
      bvhNode["isLeaf"]         = &BvhNode::isLeaf;
      bvhNode["query"]          = sol::overload([] (const BvhNode& n, const Ray& r) { return n.query(r); },
                                                PickOverload<const Ray&, RayHit*>(&BvhNode::query));
    }

    {
      sol::usertype<BvhSystem> bvhSystem = state.new_usertype<BvhSystem>("BvhSystem",
                                                                         sol::constructors<BvhSystem()>(),
                                                                         sol::base_classes, sol::bases<System>());
      bvhSystem["getRootNode"] = [] (const BvhSystem& s) { return &s.getRootNode(); };
      bvhSystem["build"]       = &BvhSystem::build;
      bvhSystem["query"]       = sol::overload([] (const BvhSystem& s, const Ray& r) { return s.query(r); },
                                               PickOverload<const Ray&, RayHit*>(&BvhSystem::query));
    }
  }

  {
    sol::usertype<Color> color = state.new_usertype<Color>("Color",
                                                           sol::constructors<Color(),
                                                                             Color(const Vec3f&),
                                                                             Color(float, float, float),
                                                                             Color(const Vec3b&),
                                                                             Color(uint32_t)>());
    color["toVec"] = &Color::operator const Vec3f&;

    sol::table colorPreset     = state["ColorPreset"].get_or_create<sol::table>();
    colorPreset["Black"]       = ColorPreset::Black;
    colorPreset["Gray"]        = ColorPreset::Gray;
    colorPreset["Red"]         = ColorPreset::Red;
    colorPreset["Green"]       = ColorPreset::Green;
    colorPreset["Blue"]        = ColorPreset::Blue;
    colorPreset["MediumRed"]   = ColorPreset::MediumRed;
    colorPreset["MediumGreen"] = ColorPreset::MediumGreen;
    colorPreset["MediumBlue"]  = ColorPreset::MediumBlue;
    colorPreset["Cyan"]        = ColorPreset::Cyan;
    colorPreset["Magenta"]     = ColorPreset::Magenta;
    colorPreset["Yellow"]      = ColorPreset::Yellow;
    colorPreset["White"]       = ColorPreset::White;
  }

#if defined(RAZ_USE_FBX)
  {
    sol::table fbxFormat = state["FbxFormat"].get_or_create<sol::table>();
    fbxFormat["load"]    = &FbxFormat::load;
  }
#endif

  {
    sol::usertype<Image> image = state.new_usertype<Image>("Image",
                                                           sol::constructors<Image(),
                                                                             Image(ImageColorspace),
                                                                             Image(ImageColorspace, ImageDataType),
                                                                             Image(unsigned int, unsigned int, ImageColorspace),
                                                                             Image(unsigned int, unsigned int, ImageColorspace, ImageDataType)>());
    image["getWidth"]        = &Image::getWidth;
    image["getHeight"]       = &Image::getHeight;
    image["getColorspace"]   = &Image::getColorspace;
    image["getDataType"]     = &Image::getDataType;
    image["getChannelCount"] = &Image::getChannelCount;
    image["isEmpty"]         = &Image::isEmpty;

    state.new_enum<ImageColorspace>("ImageColorspace", {
      { "GRAY",       ImageColorspace::GRAY },
      { "GRAY_ALPHA", ImageColorspace::GRAY_ALPHA },
      { "RGB",        ImageColorspace::RGB },
      { "RGBA",       ImageColorspace::RGBA },
      { "SRGB",       ImageColorspace::SRGB },
      { "SRGBA",      ImageColorspace::SRGBA }
    });

    state.new_enum<ImageDataType>("ImageDataType", {
      { "BYTE",  ImageDataType::BYTE },
      { "FLOAT", ImageDataType::FLOAT }
    });
  }

  {
    sol::table imageFormat = state["ImageFormat"].get_or_create<sol::table>();
    imageFormat["load"]    = sol::overload([] (const FilePath& p) { return ImageFormat::load(p); },
                                           PickOverload<const FilePath&, bool>(&ImageFormat::load));
    imageFormat["save"]    = sol::overload([] (const FilePath& p, const Image& i) { ImageFormat::save(p, i); },
                                           PickOverload<const FilePath&, const Image&, bool>(&ImageFormat::save));
  }

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
    sol::table meshFormat = state["MeshFormat"].get_or_create<sol::table>();
    meshFormat["load"]    = &MeshFormat::load;
    meshFormat["save"]    = sol::overload([] (const FilePath& p, const Mesh& m) { MeshFormat::save(p, m); },
                                          PickOverload<const FilePath&, const Mesh&, const MeshRenderer*>(&MeshFormat::save));
  }

  {
    sol::table objFormat = state["ObjFormat"].get_or_create<sol::table>();
    objFormat["load"]    = &ObjFormat::load;
    objFormat["save"]    = sol::overload([] (const FilePath& p, const Mesh& m) { ObjFormat::save(p, m); },
                                         PickOverload<const FilePath&, const Mesh&, const MeshRenderer*>(&ObjFormat::save));
  }

  {
    sol::table offFormat = state["OffFormat"].get_or_create<sol::table>();
    offFormat["load"]    = &OffFormat::load;
  }

  {
    sol::table pngFormat = state["PngFormat"].get_or_create<sol::table>();
    pngFormat["load"]    = sol::overload([] (const FilePath& p) { return PngFormat::load(p); },
                                         PickOverload<const FilePath&, bool>(&PngFormat::load));
    pngFormat["save"]    = sol::overload([] (const FilePath& p, const Image& i) { PngFormat::save(p, i); },
                                         PickOverload<const FilePath&, const Image&, bool>(&PngFormat::save));
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

  {
    sol::table tgaFormat = state["TgaFormat"].get_or_create<sol::table>();
    tgaFormat["load"]    = sol::overload([] (const FilePath& p) { return TgaFormat::load(p); },
                                         PickOverload<const FilePath&, bool>(&TgaFormat::load));
  }

  {
    sol::table wavFormat = state["WavFormat"].get_or_create<sol::table>();
    wavFormat["load"]    = &WavFormat::load;
  }
}

} // namespace Raz
