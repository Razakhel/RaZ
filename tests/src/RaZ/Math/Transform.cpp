#include "Catch.hpp"

#include "RaZ/Math/Angle.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"

using namespace Raz::Literals;

TEST_CASE("Transform position", "[math]") {
  Raz::Transform transform;
  CHECK(transform.getPosition() == Raz::Vec3f(0.f, 0.f, 0.f));
  CHECK(transform.hasUpdated()); // By default, a transform always has an updated status
  transform.setUpdated(false);

  transform.translate(Raz::Vec3f(0.f, 0.f, 1.f));
  transform.translate(0.f, 0.f, 1.f); // Equivalent of the above
  CHECK(transform.getPosition() == Raz::Vec3f(0.f, 0.f, 2.f));
  CHECK(transform.hasUpdated()); // Changing the position does set the updated state
  transform.setUpdated(false);

  transform.setPosition(Raz::Vec3f(0.f, 0.f, 0.f));
  transform.setPosition(0.f, 0.f, 0.f); // Equivalent of the above
  CHECK(transform.getPosition() == Raz::Vec3f(0.f, 0.f, 0.f));
  CHECK(transform.hasUpdated());

  // Assigning a rotation for the following calls
  transform.setRotation(90_deg, Raz::Axis::Y);
  transform.setUpdated(false);

  // move() performs a relative displacement, taking rotation into account
  transform.move(Raz::Vec3f(0.f, 0.f, 1.f));
  transform.move(0.f, 0.f, 1.f); // Equivalent of the above
  CHECK(transform.getPosition() == Raz::Vec3f(2.f, 0.f, 0.f));
  CHECK(transform.hasUpdated());
  transform.setUpdated(false);

  const Raz::Mat4f translationMat = transform.computeTranslationMatrix(false);
  CHECK(Raz::Mat3f(translationMat) == Raz::Mat3f::identity());
  CHECK(Raz::Vec3f(translationMat.recoverColumn(3)) == transform.getPosition());
  CHECK_FALSE(transform.hasUpdated()); // Computing the translation matrix does not change the update state

  const Raz::Mat4f reversedTranslationMat = transform.computeTranslationMatrix(true); // Reversing the translation
  CHECK(Raz::Mat3f(reversedTranslationMat) == Raz::Mat3f::identity());
  CHECK(Raz::Vec3f(reversedTranslationMat.recoverColumn(3)) == -transform.getPosition());
  CHECK_FALSE(transform.hasUpdated());
}

TEST_CASE("Transform rotation", "[math]") {
  Raz::Transform transform;
  CHECK(transform.getRotation() == Raz::Quaternionf::identity());
  CHECK(transform.hasUpdated()); // By default, a transform always has an updated status
  transform.setUpdated(false);

  transform.setRotation(Raz::Quaternionf(90_deg, Raz::Axis::Y));
  transform.setRotation(90_deg, Raz::Axis::Y); // Equivalent of the above
  CHECK(transform.getRotation() == Raz::Quaternionf(90_deg, Raz::Axis::Y));
  CHECK(transform.hasUpdated()); // Changing the rotation does set the updated state
  transform.setUpdated(false);

  transform.rotate(Raz::Quaternionf(-45_deg, Raz::Axis::Y));
  transform.rotate(-45_deg, Raz::Axis::Y); // Equivalent of the above
  CHECK(transform.getRotation() == Raz::Quaternionf::identity());
  CHECK(transform.hasUpdated());
  transform.setUpdated(false);

  // Rotating on multiple axes at the same time; the rotation around X will be applied before the one around Y
  transform.rotate(90_deg, 45_deg);
  CHECK(transform.getRotation() == Raz::Quaternionf(45_deg, Raz::Axis::Y) * Raz::Quaternionf(90_deg, Raz::Axis::X));
  CHECK(transform.hasUpdated());
  transform.setUpdated(false);

  // TODO: rotate(x, y, z)'s operations must be verified before being tested
  //transform.setRotation(Raz::Quaternionf::identity());
  //transform.rotate(90_deg, 90_deg, 90_deg);
  //CHECK(transform.getRotation() == ?);
  //CHECK(transform.hasUpdated());
}

TEST_CASE("Transform scale", "[math]") {
  Raz::Transform transform;
  CHECK(transform.getScale() == Raz::Vec3f(1.f, 1.f, 1.f));
  CHECK(transform.hasUpdated()); // By default, a transform always has an updated status
  transform.setUpdated(false);

  transform.setScale(2.f);
  transform.setScale(Raz::Vec3f(2.f, 2.f, 2.f)); // Equivalent of the above
  transform.setScale(2.f, 2.f, 2.f); // Equivalent of the above
  CHECK(transform.getScale() == Raz::Vec3f(2.f, 2.f, 2.f));
  CHECK(transform.hasUpdated()); // Changing the scale does set the updated state
  transform.setUpdated(false);

  // The scale is relative: 0.5 downscales the current value by half
  transform.scale(0.5f);
  transform.scale(Raz::Vec3f(0.5f, 0.5f, 0.5f)); // Equivalent of the above
  transform.scale(0.5f, 0.5f, 0.5f); // Equivalent of the above
  CHECK(transform.getScale() == Raz::Vec3f(0.25f, 0.25f, 0.25f));
  CHECK(transform.hasUpdated());
}

TEST_CASE("Transform operations", "[math]") {
  Raz::Transform transform(Raz::Vec3f(1.f, 0.f, 1.f), Raz::Quaternionf(90_deg, Raz::Axis::Y), Raz::Vec3f(2.f));

  CHECK(transform.getPosition() == Raz::Vec3f(1.f, 0.f, 1.f));
  CHECK(transform.getRotation() == Raz::Quaternionf(90_deg, Raz::Axis::Y));
  CHECK(transform.getScale() == Raz::Vec3f(2.f, 2.f, 2.f));

  transform.setUpdated(false);
  Raz::Mat4f transformMat = transform.computeTransformMatrix();
  CHECK(Raz::Vec3f(transformMat.recoverColumn(3)) == transform.getPosition());
  CHECK(transformMat == Raz::Mat4f( 0.f, 0.f, 2.f, 1.f,
                                    0.f, 2.f, 0.f, 0.f,
                                   -2.f, 0.f, 0.f, 1.f,
                                    0.f, 0.f, 0.f, 1.f));
  CHECK_FALSE(transform.hasUpdated()); // Computing the transform matrix doesn't change the update state

  transform.rotate(45_deg, Raz::Axis::X);
  transform.move(0.f, 0.f, -1.f);
  transform.setScale(1.f);

  transform.setUpdated(false);
  transformMat = transform.computeTransformMatrix();
  CHECK(Raz::Vec3f(transformMat.recoverColumn(3)) == transform.getPosition());
  CHECK(transformMat == Raz::Mat4f(-8.9407e-08f, 0.707106829f,  0.707106709f, 0.292893231f,
                                    0.f,         0.707106769f, -0.707106829f, 0.707106769f,
                                   -1.f,         0.f,          -1.19209e-07f, 1.f,
                                    0.f,         0.f,           0.f,          1.f));
  CHECK_FALSE(transform.hasUpdated());
}
