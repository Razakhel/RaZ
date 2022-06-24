#include "Catch.hpp"

#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"

using namespace Raz::Literals;

TEST_CASE("Camera view") {
  Raz::Camera camera;

  const Raz::Quaternionf rotation(45_deg, Raz::Axis::Y);
  Raz::Transform camTransform(Raz::Vec3f(0.f, 1., 0.f), rotation);
  const Raz::Mat4f& viewMat = camera.computeViewMatrix(camTransform);

  CHECK(Raz::Mat3f(viewMat) == Raz::Mat3f(rotation.computeMatrix().inverse()));
  CHECK(Raz::Vec3f(viewMat.recoverColumn(3)) == -camTransform.getPosition());

  CHECK(viewMat == Raz::Mat4f(0.707106709f, 0.f, -0.707106829f,  0.f,
                              0.f,          1.f,  0.f,          -1.f,
                              0.707106829f, 0.f,  0.707106709f,  0.f,
                              0.f,          0.f,  0.f,           1.f));

  camTransform.translate(0.f, -1.f, 0.f);
  camTransform.rotate(-45_deg, Raz::Axis::Y);
  camera.computeViewMatrix(camTransform);

  CHECK(viewMat == Raz::Mat4f::identity());
}

TEST_CASE("Camera look-at") {
  Raz::Camera camera;

  const Raz::Mat4f& viewMat = camera.computeLookAt(Raz::Vec3f(0.f));

  // If target == position, creates a look-at matrix filled with NaNs:
  // [ nan, nan, nan, nan ]
  // [ nan, nan, nan, nan ]
  // [ nan, nan, nan, nan ]
  // [ 0,   0,   0,   1   ]
  for (std::size_t widthIndex = 0; widthIndex < 4; ++widthIndex) {
    for (std::size_t heightIndex = 0; heightIndex < 3; ++heightIndex)
      CHECK(std::isnan(viewMat.getElement(widthIndex, heightIndex)));
  }

  camera.setTarget(Raz::Vec3f(0.f, 0.f, -1.f));
  camera.computeLookAt(Raz::Vec3f(0.f));

  CHECK(viewMat == Raz::Mat4f::identity());

  constexpr Raz::Vec3f position(0.f, 0.f, 1.f);
  camera.computeLookAt(position);

  CHECK(Raz::Vec3f(viewMat.recoverColumn(3)) == -position);
  CHECK(viewMat == Raz::Mat4f(1.f, 0.f, 0.f,  0.f,
                              0.f, 1.f, 0.f,  0.f,
                              0.f, 0.f, 1.f, -1.f,
                              0.f, 0.f, 0.f,  1.f));

  camera.setTarget(Raz::Vec3f(1.f, 2.f, 3.f));
  camera.computeLookAt(position);

  CHECK_THAT(viewMat, IsNearlyEqualToMatrix(Raz::Mat4f(-0.89442718f,   0.f,           0.44721359f,  -0.44721359f,
                                                       -0.298142403f,  0.745356f,    -0.596284807f,  0.596284807f,
                                                       -0.333333343f, -0.666666687f, -0.666666687f,  0.666666687f,
                                                        0.f,           0.f,           0.f,           1.f)));
}

TEST_CASE("Camera perspective projection") {
  Raz::Camera camera(800, 400, 45_deg, 0.1f, 100.f, Raz::ProjectionType::PERSPECTIVE);

  CHECK(camera.computeProjectionMatrix().strictlyEquals(camera.computePerspectiveMatrix())); // Checking that the camera is a perspective one
  CHECK_THAT(camera.getProjectionMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(1.20710671f, 0.f,          0.f,        0.f,
                                                                            0.f,         2.41421342f,  0.f,        0.f,
                                                                            0.f,         0.f,         -1.002002f, -0.2002002f,
                                                                            0.f,         0.f,         -1.f,        0.f)));

  camera.resizeViewport(1280, 720);
  camera.setFieldOfView(90_deg);

  CHECK_THAT(camera.getProjectionMatrix(), IsNearlyEqualToMatrix(Raz::Mat4f(0.5625f, 0.f,  0.f,        0.f,
                                                                            0.f,     1.f,  0.f,        0.f,
                                                                            0.f,     0.f, -1.002002f, -0.2002002f,
                                                                            0.f,     0.f, -1.f,        0.f)));
}
