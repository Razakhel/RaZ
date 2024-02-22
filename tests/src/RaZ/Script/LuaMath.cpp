#include "Catch.hpp"

#include "RaZ/Script/LuaWrapper.hpp"

TEST_CASE("LuaMath Angle", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local deg = Degreesf.new(180)
    assert(-deg == Degreesf.new(-180))
    assert(deg + 180 == Degreesf.new(360))
    assert(deg - 90 == Degreesf.new(90))
    assert(deg * 2 == Degreesf.new(360))
    assert(deg / 2 == Degreesf.new(90))
    assert(FloatUtils.areNearlyEqual(Radiansf.new(deg).value, Constant.Pi))

    local rad = Radiansf.new(Constant.Pi)
    assert(-rad == Radiansf.new(-Constant.Pi))
    assert(rad + Constant.Pi == Radiansf.new(Constant.Pi * 2))
    assert(rad - Constant.Pi / 2 == Radiansf.new(Constant.Pi / 2))
    assert(rad * 2 == Radiansf.new(Constant.Pi * 2))
    assert(rad / 2 == Radiansf.new(Constant.Pi / 2))
    assert(FloatUtils.areNearlyEqual(Degreesf.new(rad).value, 180))
  )"));
}

TEST_CASE("LuaMath MathUtils", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    assert(MathUtils.lerp(1, 2, 0.25) == 1.25)
    assert(MathUtils.lerp(Vec3f.new(1), Vec3f.new(2), 0.25) == Vec3f.new(1.25))

    assert(MathUtils.smoothstep(0.25) == 0.15625)
    assert(MathUtils.smoothstep(1, 2, 1.25) == 0.15625)

    assert(MathUtils.smootherstep(0.25) == 0.103515625)
    assert(MathUtils.smootherstep(1, 2, 1.25) == 0.103515625)
  )"));
}

TEST_CASE("LuaMath Matrix", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local mat2 = Mat2f.new()
    mat2       = Mat2f.new(Mat3f.identity())
    mat2       = Mat2f.new(1, 2,
                           3, 4)

    assert(mat2:getElement(0, 1) == 3)
    assert(mat2:getElement(1, 1) == 4)
    assert(mat2[1] == 3)
    assert(mat2[3] == 4)
    assert(mat2:recoverRow(1) == Vec2f.new(3, 4))
    assert(mat2:recoverColumn(1) == Vec2f.new(2, 4))

    assert(mat2:strictlyEquals(Mat2f.fromRows(Vec2f.new(1, 2),
                                              Vec2f.new(3, 4))))
    assert(mat2:strictlyEquals(Mat2f.fromColumns(Vec2f.new(1, 3),
                                                 Vec2f.new(2, 4))))

    assert(mat2:transpose():strictlyEquals(Mat2f.new(1, 3,
                                                     2, 4)))
    assert(mat2:computeDeterminant() == -2)
    assert(mat2:inverse():strictlyEquals(Mat2f.new(-2,  1.5,
                                                    1, -0.5)))

    assert(mat2 + mat2 == mat2 * 2)
    assert(mat2 + 1 == Mat2f.new(2, 3,
                                 4, 5))
    assert(mat2 - mat2 == Mat2f.new())
    assert(mat2 - 1 == Mat2f.new(0, 1,
                                 2, 3))
    assert(mat2 % mat2 == Mat2f.new(1,  4,
                                    9, 16))
    assert(mat2 / mat2 == Mat2f.new(1, 1,
                                    1, 1))
    assert(mat2 / 1 == mat2)
    assert(mat2 * mat2 == Mat2f.new(7, 10,
                                    15, 22))
    assert(mat2 * Vec2f.new(1) == Vec2f.new(3, 7))
    assert(Vec2f.new(1) * mat2 == Vec2f.new(4, 6))

    local mat3 = Mat3f.new()
    mat3       = Mat3f.new(Mat2f.identity())
    mat3       = Mat3f.new(Mat4f.identity())
    mat3       = Mat3f.new(1, 2, 3,
                           4, 5, 6,
                           7, 8, 9)

    assert(mat3:getElement(0, 1) == 4)
    assert(mat3:getElement(2, 2) == 9)
    assert(mat3[1] == 4)
    assert(mat3[8] == 9)
    assert(mat3:recoverRow(1) == Vec3f.new(4, 5, 6))
    assert(mat3:recoverColumn(1) == Vec3f.new(2, 5, 8))

    assert(mat3:strictlyEquals(Mat3f.fromRows(Vec3f.new(1, 2, 3),
                                              Vec3f.new(4, 5, 6),
                                              Vec3f.new(7, 8, 9))))
    assert(mat3:strictlyEquals(Mat3f.fromColumns(Vec3f.new(1, 4, 7),
                                                 Vec3f.new(2, 5, 8),
                                                 Vec3f.new(3, 6, 9))))

    assert(mat3:transpose():strictlyEquals(Mat3f.new(1, 4, 7,
                                                     2, 5, 8,
                                                     3, 6, 9)))
    assert(mat3:computeDeterminant() == 0)
    assert(mat3:inverse():strictlyEquals(mat3))

    assert(mat3 + mat3 == mat3 * 2)
    assert(mat3 + 1 == Mat3f.new(2, 3,  4,
                                 5, 6,  7,
                                 8, 9, 10))
    assert(mat3 - mat3 == Mat3f.new())
    assert(mat3 - 1 == Mat3f.new(0, 1, 2,
                                 3, 4, 5,
                                 6, 7, 8))
    assert(mat3 % mat3 == Mat3f.new( 1,  4,  9,
                                    16, 25, 36,
                                    49, 64, 81))
    assert(mat3 / mat3 == Mat3f.new(1, 1, 1,
                                    1, 1, 1,
                                    1, 1, 1))
    assert(mat3 / 1 == mat3)
    assert(mat3 * mat3 == Mat3f.new( 30,  36,  42,
                                     66,  81,  96,
                                    102, 126, 150))
    assert(mat3 * Vec3f.new(1) == Vec3f.new(6, 15, 24))
    assert(Vec3f.new(1) * mat3 == Vec3f.new(12, 15, 18))

    local mat4 = Mat4f.new()
    mat4       = Mat4f.new(Mat3f.identity())
    mat4       = Mat4f.new( 1,  2,  3,  4,
                            5,  6,  7,  8,
                            9, 10, 11, 12,
                           13, 14, 15, 16)

    assert(mat4:getElement(0, 1) == 5)
    assert(mat4:getElement(3, 3) == 16)
    assert(mat4[1] == 5)
    assert(mat4[15] == 16)
    assert(mat4:recoverRow(1) == Vec4f.new(5, 6, 7, 8))
    assert(mat4:recoverColumn(1) == Vec4f.new(2, 6, 10, 14))

    assert(mat4:strictlyEquals(Mat4f.fromRows(Vec4f.new( 1,  2,  3,  4),
                                              Vec4f.new( 5,  6,  7,  8),
                                              Vec4f.new( 9, 10, 11, 12),
                                              Vec4f.new(13, 14, 15, 16))))
    assert(mat4:strictlyEquals(Mat4f.fromColumns(Vec4f.new(1, 5,  9, 13),
                                                 Vec4f.new(2, 6, 10, 14),
                                                 Vec4f.new(3, 7, 11, 15),
                                                 Vec4f.new(4, 8, 12, 16))))

    assert(mat4:transpose():strictlyEquals(Mat4f.new(1, 5,  9, 13,
                                                     2, 6, 10, 14,
                                                     3, 7, 11, 15,
                                                     4, 8, 12, 16)))
    assert(mat4:computeDeterminant() == 0)
    assert(mat4:inverse():strictlyEquals(mat4))

    assert(mat4 + mat4 == mat4 * 2)
    assert(mat4 + 1 == Mat4f.new( 2,  3,  4,  5,
                                  6,  7,  8,  9,
                                 10, 11, 12, 13,
                                 14, 15, 16, 17))
    assert(mat4 - mat4 == Mat4f.new())
    assert(mat4 - 1 == Mat4f.new( 0,  1,  2,  3,
                                  4,  5,  6,  7,
                                  8,  9, 10, 11,
                                 12, 13, 14, 15))
    assert(mat4 % mat4 == Mat4f.new(  1,   4,   9,  16,
                                     25,  36,  49,  64,
                                     81, 100, 121, 144,
                                    169, 196, 225, 256))
    assert(mat4 / mat4 == Mat4f.new(1, 1, 1, 1,
                                    1, 1, 1, 1,
                                    1, 1, 1, 1,
                                    1, 1, 1, 1))
    assert(mat4 / 1 == mat4)
    assert(mat4 * mat4 == Mat4f.new( 90, 100, 110, 120,
                                    202, 228, 254, 280,
                                    314, 356, 398, 440,
                                    426, 484, 542, 600))
    assert(mat4 * Vec4f.new(1) == Vec4f.new(10, 26, 42, 58))
    assert(Vec4f.new(1) * mat4 == Vec4f.new(28, 32, 36, 40))
  )"));
}

TEST_CASE("LuaMath PerlinNoise", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute1D(0.1), -0.106848))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute1D(0.1, 2), -0.224224))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute1D(0.1, 2, true), 0.387888))

    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute2D(0.1, 0.2), -0.0533116))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute2D(0.1, 0.2, 2), -0.0347926))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute2D(0.1, 0.2, 2, true), 0.4826036))

    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute3D(0.1, 0.2, 0.3), 0.2549784))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute3D(0.1, 0.2, 0.3, 2), 0.330419))
    assert(FloatUtils.areNearlyEqual(PerlinNoise.compute3D(0.1, 0.2, 0.3, 2, true), 0.6652095))
  )"));
}

TEST_CASE("LuaMath Quaternion", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local quat = Quaternionf.new(1, 0, 0, 0)
    quat       = Quaternionf.identity()
    quat       = Quaternionf.new(Degreesf.new(180), Axis.Y)
    quat       = Quaternionf.new(Degreesf.new(180), 0, 1, 0)
    quat       = Quaternionf.new(Radiansf.new(Constant.Pi), Axis.Y)
    quat       = Quaternionf.new(Radiansf.new(Constant.Pi), 0, 1, 0)

    assert(FloatUtils.areNearlyEqual(quat:w(), 0))
    assert(quat:x() == 0)
    assert(quat:y() == 1)
    assert(quat:z() == 0)
    assert(FloatUtils.areNearlyEqual(quat, Quaternionf.new(0, 0, 1, 0)))

    assert(quat:computeNorm() == 1)
    assert(quat:dot(quat) == quat:computeSquaredNorm())
    assert(quat:normalize() == quat)

    assert(quat:lerp(Quaternionf.new(Radiansf.new(Constant.Pi / 2), Axis.Y), 0.25) == Quaternionf.new(0.1767766, 0, 0.9267767, 0))
    assert(quat:nlerp(Quaternionf.new(Radiansf.new(Constant.Pi / 2), Axis.Y), 0.25) == Quaternionf.new(0.1873655, 0, 0.9822902, 0))
    assert(quat:slerp(Quaternionf.new(Radiansf.new(Constant.Pi / 2), Axis.Y), 0.25) == Quaternionf.new(0.1950902, 0, 0.9807852, 0))

    assert(quat:conjugate() == quat:inverse())
    assert(quat:computeMatrix() == Mat4f.new(-1, 0,  0, 0,
                                              0, 1,  0, 0,
                                              0, 0, -1, 0,
                                              0, 0,  0, 1))

    assert(quat * quat == Quaternionf.new(-1, 0, 0, 0))
    assert(quat * Vec3f.new(1) == Vec3f.new(-1, 1, -1))
    assert(Vec3f.new(1) * quat == Vec3f.new(-1, 1, -1))
  )"));
}

TEST_CASE("LuaMath Transform", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local trans = Transform.new()
    trans       = Transform.new(Vec3f.new())
    trans       = Transform.new(Vec3f.new(), Quaternionf.identity())
    trans       = Transform.new(Vec3f.new(), Quaternionf.identity(), Vec3f.new(1))

    trans.position = Vec3f.new(1, 2, 3)
    trans.rotation = Quaternionf.new(Radiansf.new(Degreesf.new(90)), Axis.Y)
    trans.scaling  = Vec3f.new(2)

    assert(trans.position == Vec3f.new(1, 2, 3))
    assert(trans.rotation == Quaternionf.new(0.7071067, 0, 0.7071067, 0))
    assert(trans.scaling == Vec3f.new(2))

    trans:move(Vec3f.new(0, 0, 1))
    trans:move(0, 0, 1)
    trans:translate(Vec3f.new(0, 0, 1))
    trans:translate(0, 0, 1)
    assert(trans.position == Vec3f.new(3, 2, 5))

    trans:rotate(Quaternionf.new(Radiansf.new(Degreesf.new(45)), Axis.X))
    trans:rotate(Radiansf.new(Degreesf.new(45)), Axis.X)
    trans:rotate(Radiansf.new(Constant.Pi), Radiansf.new(Constant.Pi))
    trans:rotate(Radiansf.new(Constant.Pi), Radiansf.new(Constant.Pi), Radiansf.new(Constant.Pi))
    assert(trans.rotation == Quaternionf.new(-0.5, 0.5, 0.5, 0.5))

    trans:scale(Vec3f.new(2))
    trans:scale(2, 1, 2)
    trans:scale(1)
    assert(trans.scaling == Vec3f.new(8, 4, 8))

    assert(trans:computeTranslationMatrix() == Mat4f.new(1, 0, 0, 3,
                                                         0, 1, 0, 2,
                                                         0, 0, 1, 5,
                                                         0, 0, 0, 1))
    assert(trans:computeTranslationMatrix(true) == Mat4f.new(1, 0, 0, -3,
                                                             0, 1, 0, -2,
                                                             0, 0, 1, -5,
                                                             0, 0, 0,  1))
    assert(FloatUtils.areNearlyEqual(trans:computeTransformMatrix(), Mat4f.new(0, 4, 0, 3,
                                                                               0, 0, 8, 2,
                                                                               8, 0, 0, 5,
                                                                               0, 0, 0, 1), 0.000001))
  )"));
}

TEST_CASE("LuaMath Vector byte", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local vec2 = Vec2b.new()
    vec2       = Vec2b.new(1)
    vec2       = Vec2b.new(1, 2)
    vec2       = Vec2b.new(Vec3b.new(1, 2, 3))

    assert(vec2:x() == vec2[0])
    assert(vec2:y() == vec2[1])

    assert(vec2:computeSquaredLength() == vec2:dot(vec2))
    assert(FloatUtils.areNearlyEqual(vec2:computeLength(), 2.236068))
    assert(FloatUtils.areNearlyEqual(vec2:normalize():computeSquaredLength(), 1))

    assert(Vec2b.new(0, 1):lerp(Vec2b.new(1, 0), 0.5) == Vec2b.new(0)) -- Truncated from 0.5 to 0
    assert(Vec2b.new(0, 1):lerpf(Vec2b.new(1, 0), 0.5) == Vec2f.new(0.5)) -- lerpf() is available to avoid truncating
    assert(Vec2b.new(0, 1):nlerp(Vec2b.new(1, 0), 0.5) == Vec2f.new(0.7071067))

    assert(-vec2 == Vec2b.new(255, 254)) -- This is technically doable, but probably never makes sense and may be removed in the future
    assert(vec2 + Vec2b.new(1) == vec2 + 1)
    assert(vec2 - Vec2b.new(1) == vec2 - 1)
    assert(vec2 * Vec2b.new(2) == vec2 * 2)
    assert(vec2 / Vec2b.new(2) == vec2 / 2)

    local vec3 = Vec3b.new()
    vec3       = Vec3b.new(1)
    vec3       = Vec3b.new(1, 2, 3)
    vec3       = Vec3b.new(vec2, 3)
    vec3       = Vec3b.new(Vec4b.new(1, 2, 3, 4))

    assert(vec3:x() == vec3[0])
    assert(vec3:y() == vec3[1])
    assert(vec3:z() == vec3[2])

    assert(vec3:computeSquaredLength() == vec3:dot(vec3))
    assert(FloatUtils.areNearlyEqual(vec3:computeLength(), 3.7416574))
    assert(FloatUtils.areNearlyEqual(vec3:normalize():computeSquaredLength(), 1))

    assert(Vec3b.new(0, 1, 0):lerp(Vec3b.new(1, 0, 1), 0.5) == Vec3b.new(0)) -- Truncated from 0.5 to 0
    assert(Vec3b.new(0, 1, 0):lerpf(Vec3b.new(1, 0, 1), 0.5) == Vec3f.new(0.5))
    assert(Vec3b.new(0, 1, 0):nlerp(Vec3b.new(1, 0, 1), 0.5) == Vec3f.new(0.5773502))

    assert(-vec3 == Vec3b.new(255, 254, 253))
    assert(vec3 + Vec3b.new(1) == vec3 + 1)
    assert(vec3 - Vec3b.new(1) == vec3 - 1)
    assert(vec3 * Vec3b.new(2) == vec3 * 2)
    assert(vec3 / Vec3b.new(2) == vec3 / 2)

    local vec4 = Vec4b.new()
    vec4       = Vec4b.new(1)
    vec4       = Vec4b.new(1, 2, 3, 4)
    vec4       = Vec4b.new(vec3, 4)

    assert(vec4:x() == vec4[0])
    assert(vec4:y() == vec4[1])
    assert(vec4:z() == vec4[2])
    assert(vec4:w() == vec4[3])

    assert(vec4:computeSquaredLength() == vec4:dot(vec4))
    assert(FloatUtils.areNearlyEqual(vec4:computeLength(), 5.4772257))
    assert(FloatUtils.areNearlyEqual(vec4:normalize():computeSquaredLength(), 1))

    assert(Vec4b.new(0, 1, 0, 1):lerp(Vec4b.new(1, 0, 1, 1), 0.5) == Vec4b.new(0, 0, 0, 1)) -- Truncated from 0.5 to 0
    assert(Vec4b.new(0, 1, 0, 1):lerpf(Vec4b.new(1, 0, 1, 1), 0.5) == Vec4f.new(0.5, 0.5, 0.5, 1))
    assert(Vec4b.new(0, 1, 0, 1):nlerp(Vec4b.new(1, 0, 1, 1), 0.5) == Vec4f.new(0.3779644, 0.3779644, 0.3779644, 0.7559289))

    assert(-vec4 == Vec4b.new(255, 254, 253, 252))
    assert(vec4 + Vec4b.new(1) == vec4 + 1)
    assert(vec4 - Vec4b.new(1) == vec4 - 1)
    assert(vec4 * Vec4b.new(2) == vec4 * 2)
    assert(vec4 / Vec4b.new(2) == vec4 / 2)
  )"));
}

TEST_CASE("LuaMath Vector float", "[script][lua][math]") {
  CHECK(Raz::LuaWrapper::execute(R"(
    local vec2 = Vec2f.new()
    vec2       = Vec2f.new(1)
    vec2       = Vec2f.new(1, 2)
    vec2       = Vec2f.new(Vec3f.new(1, 2, 3))

    assert(vec2:x() == vec2[0])
    assert(vec2:y() == vec2[1])

    assert(vec2:computeSquaredLength() == vec2:dot(vec2))
    assert(FloatUtils.areNearlyEqual(vec2:computeLength(), 2.236068))
    assert(FloatUtils.areNearlyEqual(vec2:normalize():computeSquaredLength(), 1))

    assert(vec2:reflect(Vec2f.new(0, 1)) == Vec2f.new(1, -2))

    assert(Vec2f.new(0, 1):lerp(Vec2f.new(1, 0), 0.5) == Vec2f.new(0.5))
    assert(Vec2f.new(0, 1):nlerp(Vec2f.new(1, 0), 0.5) == Vec2f.new(0.7071067))

    assert((-vec2):strictlyEquals(Vec2f.new(-1, -2)))
    assert(vec2 + Vec2f.new(1) == vec2 + 1)
    assert(vec2 - Vec2f.new(1) == vec2 - 1)
    assert(vec2 * Vec2f.new(2) == vec2 * 2)
    assert(vec2 / Vec2f.new(2) == vec2 / 2)

    local vec3 = Vec3f.new()
    vec3       = Vec3f.new(1)
    vec3       = Vec3f.new(1, 2, 3)
    vec3       = Vec3f.new(vec2, 3)
    vec3       = Vec3f.new(Vec4f.new(1, 2, 3, 4))

    assert(vec3:x() == vec3[0])
    assert(vec3:y() == vec3[1])
    assert(vec3:z() == vec3[2])

    assert(vec3:computeSquaredLength() == vec3:dot(vec3))
    assert(FloatUtils.areNearlyEqual(vec3:computeLength(), 3.7416574))
    assert(FloatUtils.areNearlyEqual(vec3:normalize():computeSquaredLength(), 1))

    assert(vec3:cross(Vec3f.new(0, 1, 0)) == Vec3f.new(-3, 0, 1))
    assert(vec3:reflect(Vec3f.new(0, 1, 0)) == Vec3f.new(1, -2, 3))

    assert(Vec3f.new(0, 1, 0):lerp(Vec3f.new(1, 0, 1), 0.5) == Vec3f.new(0.5))
    assert(Vec3f.new(0, 1, 0):nlerp(Vec3f.new(1, 0, 1), 0.5) == Vec3f.new(0.5773502))

    assert((-vec3):strictlyEquals(Vec3f.new(-1, -2, -3)))
    assert(vec3 + Vec3f.new(1) == vec3 + 1)
    assert(vec3 - Vec3f.new(1) == vec3 - 1)
    assert(vec3 * Vec3f.new(2) == vec3 * 2)
    assert(vec3 / Vec3f.new(2) == vec3 / 2)

    local vec4 = Vec4f.new()
    vec4       = Vec4f.new(1)
    vec4       = Vec4f.new(1, 2, 3, 4)
    vec4       = Vec4f.new(vec3, 4)

    assert(vec4:x() == vec4[0])
    assert(vec4:y() == vec4[1])
    assert(vec4:z() == vec4[2])
    assert(vec4:w() == vec4[3])

    assert(vec4:computeSquaredLength() == vec4:dot(vec4))
    assert(FloatUtils.areNearlyEqual(vec4:computeLength(), 5.4772257))
    assert(FloatUtils.areNearlyEqual(vec4:normalize():computeSquaredLength(), 1))

    assert(vec4:reflect(Vec4f.new(0, 1, 0, 1)) == Vec4f.new(1, -10, 3, -8))

    assert(Vec4f.new(0, 1, 0, 1):lerp(Vec4f.new(1, 0, 1, 1), 0.5) == Vec4f.new(0.5, 0.5, 0.5, 1))
    assert(Vec4f.new(0, 1, 0, 1):nlerp(Vec4f.new(1, 0, 1, 1), 0.5) == Vec4f.new(0.3779644, 0.3779644, 0.3779644, 0.7559289))

    assert((-vec4):strictlyEquals(Vec4f.new(-1, -2, -3, -4)))
    assert(vec4 + Vec4f.new(1) == vec4 + 1)
    assert(vec4 - Vec4f.new(1) == vec4 - 1)
    assert(vec4 * Vec4f.new(2) == vec4 * 2)
    assert(vec4 / Vec4f.new(2) == vec4 / 2)
  )"));
}
