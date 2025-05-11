#include "RaZ/Data/Bitset.hpp"
#include "RaZ/Data/BoundingVolumeHierarchy.hpp"
#include "RaZ/Data/BoundingVolumeHierarchySystem.hpp"
#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Grid2.hpp"
#include "RaZ/Data/Grid3.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/MeshDistanceField.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

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
    {
      sol::usertype<BoundingVolumeHierarchyNode> bvhNode = state.new_usertype<BoundingVolumeHierarchyNode>("BoundingVolumeHierarchyNode",
                                                                                                           sol::constructors<BoundingVolumeHierarchyNode()>());
      bvhNode["getBoundingBox"] = &BoundingVolumeHierarchyNode::getBoundingBox;
      bvhNode["hasLeftChild"]   = &BoundingVolumeHierarchyNode::hasLeftChild;
      bvhNode["getLeftChild"]   = [] (const BoundingVolumeHierarchyNode& n) { return &n.getLeftChild(); };
      bvhNode["hasRightChild"]  = &BoundingVolumeHierarchyNode::hasRightChild;
      bvhNode["getRightChild"]  = [] (const BoundingVolumeHierarchyNode& n) { return &n.getRightChild(); };
      bvhNode["getTriangle"]    = &BoundingVolumeHierarchyNode::getTriangle;
      bvhNode["isLeaf"]         = &BoundingVolumeHierarchyNode::isLeaf;
      bvhNode["query"]          = sol::overload([] (const BoundingVolumeHierarchyNode& n, const Ray& r) { return n.query(r); },
                                                PickOverload<const Ray&, RayHit*>(&BoundingVolumeHierarchyNode::query));
    }

    {
      sol::usertype<BoundingVolumeHierarchy> bvh = state.new_usertype<BoundingVolumeHierarchy>("BoundingVolumeHierarchy",
                                                                                               sol::constructors<BoundingVolumeHierarchy()>());
      bvh["getRootNode"] = [] (const BoundingVolumeHierarchy& b) { return &b.getRootNode(); };
      // Sol doesn't seem to be able to bind a constant reference to std::vector; leaving a copy here as it is "cheap"
      bvh["build"]       = [] (BoundingVolumeHierarchy& b, std::vector<Entity*> e) { b.build(e); };
      bvh["query"]       = sol::overload([] (const BoundingVolumeHierarchy& b, const Ray& r) { return b.query(r); },
                                         PickOverload<const Ray&, RayHit*>(&BoundingVolumeHierarchy::query));
    }

    {
      sol::usertype<BoundingVolumeHierarchySystem> bvhSystem = state.new_usertype<BoundingVolumeHierarchySystem>("BoundingVolumeHierarchySystem",
                                                                                                                 sol::constructors<
                                                                                                                   BoundingVolumeHierarchySystem()
                                                                                                                 >(),
                                                                                                                 sol::base_classes, sol::bases<System>());
      bvhSystem["getBvh"] = [] (BoundingVolumeHierarchySystem& s) { return &s.getBvh(); };
    }
  }

  {
    sol::usertype<Color> color = state.new_usertype<Color>("Color",
                                                           sol::constructors<Color(),
                                                                             Color(const Vec3f&),
                                                                             Color(float, float, float),
                                                                             Color(const Vec3b&),
                                                                             Color(uint32_t)>());
    color["red"]   = PickConstOverload<>(&Color::red);
    color["green"] = PickConstOverload<>(&Color::green);
    color["blue"]  = PickConstOverload<>(&Color::blue);
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

  {
    sol::usertype<Grid2b> grid2b = state.new_usertype<Grid2b>("Grid2b",
                                                              sol::constructors<Grid2b(std::size_t, std::size_t)>());
    grid2b["getWidth"]  = &Grid2b::getWidth;
    grid2b["getHeight"] = &Grid2b::getHeight;
    grid2b["getValue"]  = &Grid2b::getValue;
    grid2b["setValue"]  = &Grid2b::setValue;

    sol::usertype<Grid2f> grid2f = state.new_usertype<Grid2f>("Grid2f",
                                                              sol::constructors<Grid2f(std::size_t, std::size_t)>());
    grid2f["getWidth"]  = &Grid2f::getWidth;
    grid2f["getHeight"] = &Grid2f::getHeight;
    grid2f["getValue"]  = &Grid2f::getValue;
    grid2f["setValue"]  = &Grid2f::setValue;
  }

  {
    sol::usertype<Grid3b> grid3b = state.new_usertype<Grid3b>("Grid3b",
                                                              sol::constructors<Grid3b(std::size_t, std::size_t, std::size_t)>());
    grid3b["getWidth"]  = &Grid3b::getWidth;
    grid3b["getHeight"] = &Grid3b::getHeight;
    grid3b["getDepth"]  = &Grid3b::getDepth;
    grid3b["getValue"]  = &Grid3b::getValue;
    grid3b["setValue"]  = &Grid3b::setValue;

    sol::usertype<Grid3f> grid3f = state.new_usertype<Grid3f>("Grid3f",
                                                              sol::constructors<Grid3f(std::size_t, std::size_t, std::size_t)>());
    grid3f["getWidth"]  = &Grid3f::getWidth;
    grid3f["getHeight"] = &Grid3f::getHeight;
    grid3f["getDepth"]  = &Grid3f::getDepth;
    grid3f["getValue"]  = &Grid3f::getValue;
    grid3f["setValue"]  = &Grid3f::setValue;
  }

  {
    sol::usertype<MeshDistanceField> mdf = state.new_usertype<MeshDistanceField>("MeshDistanceField",
                                                                                 sol::constructors<
                                                                                   MeshDistanceField(const AABB&, unsigned int, unsigned int, unsigned int)
                                                                                 >(),
                                                                                 sol::base_classes, sol::bases<Grid3f>());
    mdf["setBvh"]        = &MeshDistanceField::setBvh;
    mdf["compute"]       = &MeshDistanceField::compute;
    mdf["recoverSlices"] = &MeshDistanceField::recoverSlices;
  }
}

} // namespace Raz
