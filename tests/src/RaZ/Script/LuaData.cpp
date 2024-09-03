#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaData Bitset", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local bitset = Bitset.new()
    bitset       = Bitset.new(1)
    bitset       = Bitset.new(1, false)

    bitset:setBit(1, true) -- [0, 1]
    assert(bitset:getSize() == 2)
    assert(bitset[0] ~= bitset[1])
    assert(bitset:getEnabledBitCount() == bitset:getDisabledBitCount())

    bitset:resize(3) -- [0, 1, 0]
    assert(bitset:getSize() == 3)
    assert(bitset:getDisabledBitCount() == 2)

    assert((~bitset):getDisabledBitCount() == 1) -- [1, 0, 1]
    assert((bitset & bitset) == bitset)
    assert((bitset | bitset) == bitset)
    assert((bitset ~ bitset):getDisabledBitCount() == 3) -- [0, 0, 0]
    assert((bitset << 1):getSize() == 4) -- [0, 1, 0, 0]
    assert((bitset >> 1):getSize() == 2) -- [0, 1]

    bitset:reset()
    assert(bitset:getDisabledBitCount() == bitset:getSize())

    bitset:clear()
    assert(bitset:isEmpty())
  )"));
}

TEST_CASE("LuaData BoundingVolumeHierarchy", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local bvh = BoundingVolumeHierarchy.new()

    bvh:build({})

    local rayHit = RayHit.new()
    assert(bvh:query(Ray.new(Vec3f.new(), Axis.Z)) == nil)
    assert(bvh:query(Ray.new(Vec3f.new(), Axis.Z), rayHit) == nil)

    local bvhRootNode = bvh:getRootNode()

    assert(bvhRootNode:getBoundingBox() == AABB.new(Vec3f.new(), Vec3f.new()))
    assert(bvhRootNode:hasLeftChild() == bvhRootNode:hasRightChild())
    -- Getting children cannot be tested because of the assertions checking that they exist
    --assert(bvhRootNode:getLeftChild() == nil)
    --assert(bvhRootNode:getRightChild() == nil)
    assert(bvhRootNode:getTriangle() == Triangle.new(Vec3f.new(), Vec3f.new(), Vec3f.new()))
    assert(bvhRootNode:isLeaf())
    assert(bvhRootNode:query(Ray.new(Vec3f.new(), Axis.Z)) == nil)
    assert(bvhRootNode:query(Ray.new(Vec3f.new(), Axis.Z), rayHit) == nil)
  )"));
}

TEST_CASE("LuaData BoundingVolumeHierarchySystem", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local bvhSystem = BoundingVolumeHierarchySystem.new()

    assert(bvhSystem:getBvh() ~= nil)

    assert(bvhSystem:getAcceptedComponents() ~= nil)
    assert(not bvhSystem:containsEntity(Entity.new(0)))
    assert(bvhSystem:update(FrameTimeInfo.new()))
    bvhSystem:destroy()
  )"));
}

TEST_CASE("LuaData Color", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    assert(Color.new() == ColorPreset.Black)
    assert(Color.new(Vec3f.new(1, 0, 0)) == ColorPreset.Red)
    assert(Color.new(0, 1, 0) == ColorPreset.Green)
    assert(Color.new(0x0000FF) == ColorPreset.Blue)
    assert(ColorPreset.MediumRed:red() == 1)
    assert(ColorPreset.MediumGreen:green() == 1)
    assert(ColorPreset.MediumBlue:blue() == 1)
    assert(ColorPreset.Gray:toVec():strictlyEquals(Vec3f.new(0.5)))
  )"));
}

TEST_CASE("LuaData Image", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local img = Image.new()
    img       = Image.new(ImageColorspace.GRAY)
    img       = Image.new(ImageColorspace.GRAY_ALPHA, ImageDataType.BYTE)
    img       = Image.new(1, 1, ImageColorspace.RGB)
    img       = Image.new(1, 1, ImageColorspace.RGBA, ImageDataType.FLOAT)

    assert(img:getWidth() == img:getHeight())
    assert(img:getColorspace() == ImageColorspace.RGBA)
    assert(img:getDataType() == ImageDataType.FLOAT)
    assert(img:getChannelCount() == 4)
    assert(not img:isEmpty())
    img:setFloatValue(0, 0, 0, 3.0)
    assert(img:recoverFloatValue(0, 0, 0) == 3.0)
    img:setVec4fPixel(0, 0, Vec4f.new(1, 2, 3, 4))
    assert(img:recoverVec4fPixel(0, 0) == Vec4f.new(1, 2, 3, 4))

    img = Image.new(1, 1, ImageColorspace.SRGB, ImageDataType.BYTE)
    assert(img:getChannelCount() == 3)
    img:setByteValue(0, 0, 0, 127)
    assert(img:recoverByteValue(0, 0, 0) == 127)
    img:setVec3bPixel(0, 0, Vec3b.new(1, 2, 3))
    assert(img:recoverVec3bPixel(0, 0) == Vec3b.new(1, 2, 3))

    local pngPath = FilePath.new(RAZ_TESTS_ROOT .. "assets/images/dëfàùltTêst.png")
    local tgaPath = FilePath.new(RAZ_TESTS_ROOT .. "assets/images/dëfàùltTêst.tga")
    assert(ImageFormat.loadFromData(FileUtils.readFileToArray(pngPath)) ~= ImageFormat.load(pngPath, true))
    assert(ImageFormat.loadFromData(FileUtils.readFileToArray(tgaPath), true) == TgaFormat.load(tgaPath, true))
  )"));
}

TEST_CASE("LuaData Mesh", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local mesh = Mesh.new()
    mesh       = Mesh.new(Plane.new(0), 0, 0)
    mesh       = Mesh.new(Sphere.new(Vec3f.new(), 1), 1, SphereMeshType.UV)
    mesh       = Mesh.new(Triangle.new(Vec3f.new(), Vec3f.new(), Vec3f.new()), Vec2f.new(), Vec2f.new(), Vec2f.new())
    mesh       = Mesh.new(Quad.new(Vec3f.new(), Vec3f.new(), Vec3f.new(), Vec3f.new()))
    mesh       = Mesh.new(AABB.new(Vec3f.new(), Vec3f.new()))

    assert(mesh:getSubmeshes():size() == 1)
    mesh:addSubmesh()
    assert(mesh:getSubmeshes():size() == 2)
    assert(mesh:computeBoundingBox() == mesh:getBoundingBox())
    assert(mesh:recoverVertexCount() == 24)
    assert(mesh:recoverTriangleCount() == 12)
    mesh:computeTangents()

    local meshData, _ = MeshFormat.load(FilePath.new(RAZ_TESTS_ROOT .. "assets/meshes/çûbè_BP.obj"))
    assert(meshData:recoverVertexCount() == 24)
    MeshFormat.save(FilePath.new("téstÊxpørt.obj"), meshData)

    meshData, _ = ObjFormat.load(FilePath.new("téstÊxpørt.obj"))
    assert(meshData:recoverVertexCount() == 24)
    ObjFormat.save(FilePath.new("téstÊxpørt.obj"), meshData)

    meshData, _ = GltfFormat.load(FilePath.new(RAZ_TESTS_ROOT .. "assets/meshes/ßøӾ.glb"))
    assert(meshData:recoverVertexCount() == 24)
  )"));

#if defined(RAZ_USE_FBX)
  CHECK(TestUtils::executeLuaScript(R"(
    local fbxPath = FilePath.new(RAZ_TESTS_ROOT .. "../assets/meshes/shaderBall.fbx")

    local meshData, _ = MeshFormat.load(fbxPath)
    assert(meshData:recoverVertexCount() == 40004)

    meshData, _ = FbxFormat.load(fbxPath)
    assert(meshData:recoverVertexCount() == 40004)
  )"));
#endif
}

TEST_CASE("LuaData MeshDistanceField", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local mdf = MeshDistanceField.new(AABB.new(Vec3f.new(), Vec3f.new()), 2, 2, 2)

    assert(mdf:getDistance(0, 0, 0) ~= 0)
    mdf:setBvh(BoundingVolumeHierarchy.new())
    mdf:compute(1)
    assert(mdf:recoverSlices():size() == 2)
  )"));
}

TEST_CASE("LuaData Submesh", "[script][lua][data]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local submesh = Submesh.new()

    assert(submesh:getVertices():empty())
    assert(submesh:getVertexCount() == 0)
    assert(submesh:getTriangleIndices():empty())
    assert(submesh:getTriangleIndexCount() == 0)
    assert(submesh:computeBoundingBox() == submesh:getBoundingBox())

    local vertex = Vertex.new()

    vertex.position  = Vec3f.new(1, 2, 3)
    vertex.texcoords = Vec2f.new(0.25, 0.75)
    vertex.normal    = Axis.Y
    vertex.tangent   = Axis.X
    assert(vertex.position == Vec3f.new(1, 2, 3))
    assert(vertex.texcoords == Vec2f.new(0.25, 0.75))
    assert(vertex.normal == Axis.Y)
    assert(vertex.tangent == Axis.X)
  )"));
}
