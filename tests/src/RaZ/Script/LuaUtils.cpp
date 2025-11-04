#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaUtils FilePath", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local pathStr = "../test/file.txt"

    local filePath = FilePath.new()
    filePath       = FilePath.new(pathStr)

    assert(not filePath:isEmpty())

    -- Note that the equality operator (==) works on the variables' actual values only if both sides of the operator are of the same type
    -- Comparing a FilePath to a simple string checks for pointer equality, and would thus return false
    -- See: https://sol2.readthedocs.io/en/latest/api/usertype.html#usertype-automatic-automagic-meta-functions
    assert(filePath:toUtf8() == pathStr) -- Either compare the strings...
    assert(filePath == FilePath.new(pathStr)) -- ... or the paths

    assert(filePath:recoverPathToFile():toUtf8() == "../test/")
    assert(filePath:recoverFileName():toUtf8() == "file.txt")
    assert(filePath:recoverFileName(false):toUtf8() == "file")
    assert(filePath:recoverExtension():toUtf8() == "txt")

    assert(FilePath.new("first/") .. "second" == "first/second") -- Currently, concatenation functions using strings return strings and not paths
    assert("first/" .. FilePath.new("second") == "first/second")
    assert(FilePath.new("first/") .. FilePath.new("second") == FilePath.new("first/second"))
  )"));
}

TEST_CASE("LuaUtils FileUtils", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    assert(FileUtils.isReadable(FilePath.new(RAZ_TESTS_ROOT .. "assets/misc/ͳεs†_fílè_测试.τxt")))
    assert(FileUtils.readFileToArray(FilePath.new(RAZ_TESTS_ROOT .. "assets/misc/ͳεs†_fílè_测试.τxt")):size() == 22)
    assert(FileUtils.readFileToString(FilePath.new(RAZ_TESTS_ROOT .. "assets/misc/ͳεs†_fílè_测试.τxt")) == "НΣļlõ ωθяŁĐ!\n")
  )"));
}

TEST_CASE("LuaUtils Logger", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    Logger.setLoggingLevel(LoggingLevel.ERROR)
    Logger.setLoggingLevel(LoggingLevel.WARNING)
    Logger.setLoggingLevel(LoggingLevel.INFO)
    Logger.setLoggingLevel(LoggingLevel.DEBUG)
    Logger.setLoggingLevel(LoggingLevel.ALL)
    Logger.setLoggingLevel(LoggingLevel.NONE)

    Logger.setLoggingFunction(function (level, message) end)
    Logger.resetLoggingFunction()

    Logger.error("Lua error logging")
    Logger.warn("Lua warning logging")
    Logger.info("Lua info logging")
    Logger.debug("Lua debug logging")
  )"));
}

TEST_CASE("LuaUtils Ray", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local rayHit = RayHit.new()

    rayHit.position = Vec3f.new(1, 2, 3)
    rayHit.normal   = Axis.Y
    rayHit.distance = 10
    assert(rayHit.position == Vec3f.new(1, 2, 3))
    assert(rayHit.normal == Axis.Y)
    assert(rayHit.distance == 10)

    local ray = Ray.new(Vec3f.new(1, 2, 3), Vec3f.new(1):normalize())

    assert(ray:getOrigin() == Vec3f.new(1, 2, 3))
    assert(ray:getDirection() == Vec3f.new(1):normalize())
    assert(ray:getInverseDirection() == Vec3f.new(1.7320508))
    assert(ray:computeProjection(Vec3f.new()) == ray:getOrigin())

    assert(not ray:intersects(Vec3f.new()))
    assert(not ray:intersects(Vec3f.new(), rayHit))
    assert(not ray:intersects(AABB.new(Vec3f.new(), Vec3f.new())))
    assert(not ray:intersects(AABB.new(Vec3f.new(), Vec3f.new()), rayHit))
    --assert(not ray:intersects(Line.new(Vec3f.new(), Vec3f.new())))
    --assert(not ray:intersects(Line.new(Vec3f.new(), Vec3f.new()), rayHit))
    --assert(not ray:intersects(OBB.new(Vec3f.new(), Vec3f.new())))
    --assert(not ray:intersects(OBB.new(Vec3f.new(), Vec3f.new()), rayHit))
    assert(not ray:intersects(Plane.new(Vec3f.new())))
    assert(not ray:intersects(Plane.new(Vec3f.new()), rayHit))
    --assert(not ray:intersects(Quad.new(Vec3f.new(), Vec3f.new(), Vec3f.new(), Vec3f.new())))
    --assert(not ray:intersects(Quad.new(Vec3f.new(), Vec3f.new(), Vec3f.new(), Vec3f.new()), rayHit))
    assert(not ray:intersects(Sphere.new(Vec3f.new(), 0)))
    assert(not ray:intersects(Sphere.new(Vec3f.new(), 0), rayHit))
    assert(not ray:intersects(Triangle.new(Vec3f.new(), Vec3f.new(), Vec3f.new())))
    assert(not ray:intersects(Triangle.new(Vec3f.new(), Vec3f.new(), Vec3f.new()), rayHit))
  )"));
}

TEST_CASE("LuaUtils Shape", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local aabb = AABB.new(Vec3f.new(-1), Vec3f.new(1))
    assert(aabb:getMinPosition() == Vec3f.new(-1))
    assert(aabb:getMaxPosition() == Vec3f.new(1))
    assert(aabb:computeHalfExtents() == Vec3f.new(1))
    assert(aabb:computeCorners().minMinMin == aabb:getMinPosition())

    local line = Line.new(Vec3f.new(-1), Vec3f.new(1))
    assert(line:getBeginPos() == Vec3f.new(-1))
    assert(line:getEndPos() == Vec3f.new(1))
    assert(line:computeSquaredLength() == 12)
    assert(FloatUtils.areNearlyEqual(line:computeLength(), 3.4641015529633))

    local obb = OBB.new(Vec3f.new(-1), Vec3f.new(1))
    obb       = OBB.new(Vec3f.new(-1), Vec3f.new(1), Quaternionf.identity())
    obb       = OBB.new(aabb)
    obb       = OBB.new(aabb, Quaternionf.identity())
    obb:setRotation(Quaternionf.identity())
    assert(obb:getRotation() == obb:getInverseRotation())
    assert(obb:computeRotatedMinPosition() == obb:getOriginalBox():getMinPosition())
    assert(obb:computeRotatedMaxPosition() == obb:getOriginalBox():getMaxPosition())
    assert(obb:computeRotatedHalfExtents() == obb:getOriginalBox():computeHalfExtents())

    local plane = Plane.new(0)
    plane       = Plane.new(0, Axis.Z)
    plane       = Plane.new(Vec3f.new(0))
    plane       = Plane.new(Vec3f.new(0), Axis.Z)
    plane       = Plane.new(Vec3f.new(-1, 0, 0), Vec3f.new(0, 0, 1), Vec3f.new(1, 0, 0))
    assert(FloatUtils.areNearlyEqual(plane:getDistance(), 0.3333333))
    assert(plane:getNormal() == Axis.Y)

    local quad = Quad.new(Vec3f.new(-1, 1, 0), Vec3f.new(1, 1, 0), Vec3f.new(1, -1, 0), Vec3f.new(-1, -1, 0))
    assert(quad:getLeftTopPos() == Vec3f.new(-1, 1, 0))
    assert(quad:getRightTopPos() == Vec3f.new(1, 1, 0))
    assert(quad:getRightBottomPos() == Vec3f.new(1, -1, 0))
    assert(quad:getLeftBottomPos() == Vec3f.new(-1, -1, 0))

    local sphere = Sphere.new(Vec3f.new(), 1)
    assert(sphere:getCenter() == Vec3f.new())
    assert(sphere:getRadius() == 1)

    local triangle = Triangle.new(Vec3f.new(-1, 0, 0), Vec3f.new(0, 0, 1), Vec3f.new(1, 0, 0))
    assert(triangle:getFirstPos() == Vec3f.new(-1, 0, 0))
    assert(triangle:getSecondPos() == Vec3f.new(0, 0, 1))
    assert(triangle:getThirdPos() == Vec3f.new(1, 0, 0))
    assert(not triangle:isCounterClockwise(-Axis.Y))
    triangle:makeCounterClockwise(-Axis.Y)
    assert(triangle:computeNormal() == -Axis.Y)

    -- Testing Shape functions

    assert(aabb:getType() == ShapeType.AABB)
    assert(line:getType() == ShapeType.LINE)
    assert(obb:getType() == ShapeType.OBB)
    assert(plane:getType() == ShapeType.PLANE)
    assert(quad:getType() == ShapeType.QUAD)
    assert(sphere:getType() == ShapeType.SPHERE)
    assert(triangle:getType() == ShapeType.TRIANGLE)

    assert(aabb:contains(Vec3f.new()))
    assert(obb:contains(Vec3f.new()))
    assert(sphere:contains(Vec3f.new()))

    assert(aabb:intersects(aabb))
    assert(aabb:intersects(line))
    --assert(obb:intersects(obb))
    assert(not plane:intersects(plane))
    --assert(quad:intersects(quad))
    assert(sphere:intersects(sphere))
    --assert(triangle:intersects(triangle))
    local rayHit = RayHit.new()
    assert(sphere:intersects(Ray.new(Vec3f.new(), Axis.Z), rayHit))

    sphere:translate(Vec3f.new(1))
    assert(sphere:computeCentroid() == Vec3f.new(1))

    assert(aabb:computeProjection(Vec3f.new()) == Vec3f.new())
    assert(aabb:computeBoundingBox() == aabb)
  )"));
}

TEST_CASE("LuaUtils StrUtils", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    assert(StrUtils.startsWith("test", "te"))
    assert(StrUtils.endsWith("test", "st"))
    assert(StrUtils.toLowercaseCopy("TEST") == "test")
    assert(StrUtils.toUppercaseCopy("test") == "TEST")
    assert(StrUtils.trimLeftCopy("   test   ") == "test   ")
    assert(StrUtils.trimRightCopy("   test   ") == "   test")
    assert(StrUtils.trimCopy("   test   ") == "test")

    local splittedStr = StrUtils.split("this is a test", " ")
    assert(#splittedStr == 4)
    assert(splittedStr[1] == "this")
    assert(splittedStr[4] == "test")
  )"));
}

TEST_CASE("LuaUtils TriggerVolume", "[script][lua][utils]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local triggerer = Triggerer.new()

    local triggerSystem = TriggerSystem.new()

    local triggerVolume = TriggerVolume.new(AABB.new(Vec3f.new(-1), Vec3f.new(1)))
    triggerVolume       = TriggerVolume.new(OBB.new(Vec3f.new(-1), Vec3f.new(1), Quaternionf.identity()))
    triggerVolume       = TriggerVolume.new(Sphere.new(Vec3f.new(0), 1))
    triggerVolume:setEnterAction(function () end)
    triggerVolume:setStayAction(function () end)
    triggerVolume:setLeaveAction(function () end)
    triggerVolume:enable()
    triggerVolume:enable(true)
    triggerVolume:disable()
    triggerVolume:resetEnterAction()
    triggerVolume:resetStayAction()
    triggerVolume:resetLeaveAction()
  )"));
}
