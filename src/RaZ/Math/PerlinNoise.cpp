#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Math/PerlinNoise.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz::PerlinNoise{

namespace {

constexpr std::array<unsigned int, 512> permutations = {
  151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
  8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
  35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
  134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
  55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
  18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
  250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
  189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
  172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
  228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
  107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
  138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
  151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
  8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117,
  35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
  134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
  55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
  18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
  250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182,
  189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
  172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
  228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
  107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
  138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

constexpr std::array<Vec2f, 8> gradients2D = {
  Vec2f(          1.f,            0.f), Vec2f(          -1.f,            0.f),
  Vec2f(          0.f,            1.f), Vec2f(           0.f,           -1.f),
  Vec2f(0.7071067691f,  0.7071067691f), Vec2f(-0.7071067691f,  0.7071067691f),
  Vec2f(0.7071067691f, -0.7071067691f), Vec2f(-0.7071067691f, -0.7071067691f)
};

constexpr float getGradient1D(unsigned int x) {
  return (permutations[x] % 2 == 0 ? 1.f : -1.f);
}

constexpr float getValue(float x) {
  // Determining coordinates on the line
  //
  //  x0---------x0+1

  const auto intX = static_cast<unsigned int>(x);

  const unsigned int x0 = intX & 255u;

  const float leftGrad  = getGradient1D(x0);
  const float rightGrad = getGradient1D(x0 + 1);

  // Computing the distance to the coordinate
  //
  //  |------X--|
  //      xWeight

  const float xWeight = x - static_cast<float>(intX);

  const float leftDot  = xWeight * leftGrad;
  const float rightDot = (xWeight - 1) * rightGrad;

  const float smoothX = MathUtils::smootherstep(xWeight);

  return MathUtils::lerp(leftDot, rightDot, smoothX);
}

constexpr const Vec2f& getGradient2D(unsigned int x, unsigned int y) {
  return gradients2D[permutations[permutations[x] + y] % gradients2D.size()];
}

constexpr float getValue(float x, float y) {
  // Recovering integer coordinates on the quad
  //
  //  y0+1______x0+1/y0+1
  //     |      |
  //     |      |
  // x0/y0______x0+1

  const auto intX = static_cast<unsigned int>(x);
  const auto intY = static_cast<unsigned int>(y);

  const unsigned int x0 = intX & 255u;
  const unsigned int y0 = intY & 255u;

  // Recovering pseudo-random gradients at each corner of the quad
  const Vec2f& botLeftGrad  = getGradient2D(x0, y0);
  const Vec2f& botRightGrad = getGradient2D(x0 + 1, y0);
  const Vec2f& topLeftGrad  = getGradient2D(x0, y0 + 1);
  const Vec2f& topRightGrad = getGradient2D(x0 + 1, y0 + 1);

  // Computing the distance to the coordinates
  //  _____________
  //  |           |
  //  | xWeight   |
  //  |---------X |
  //  |         | yWeight
  //  |_________|_|

  const float xWeight = x - static_cast<float>(intX);
  const float yWeight = y - static_cast<float>(intY);

  const float botLeftDot  = Vec2f(xWeight    , yWeight    ).dot(botLeftGrad);
  const float botRightDot = Vec2f(xWeight - 1, yWeight    ).dot(botRightGrad);
  const float topLeftDot  = Vec2f(xWeight    , yWeight - 1).dot(topLeftGrad);
  const float topRightDot = Vec2f(xWeight - 1, yWeight - 1).dot(topRightGrad);

  const float smoothX = MathUtils::smootherstep(xWeight);
  const float smoothY = MathUtils::smootherstep(yWeight);

  const float botCoeff = MathUtils::lerp(botLeftDot, botRightDot, smoothX);
  const float topCoeff = MathUtils::lerp(topLeftDot, topRightDot, smoothX);

  return MathUtils::lerp(botCoeff, topCoeff, smoothY);
}

} // namespace

float get1D(float x, uint8_t octaveCount, bool normalize) {
  float frequency = 1.f;
  float amplitude = 1.f;
  float total     = 0.f;

  for (uint8_t i = 0; i < octaveCount; ++i) {
    total += getValue(x * frequency) * amplitude;

    amplitude *= 0.5f;
    frequency *= 2.f;
  }

  if (normalize)
    return (total + 1) / 2; // Scaling between [0; 1]

  return total;
}

float get2D(float x, float y, uint8_t octaveCount, bool normalize) {
  float frequency = 1.f;
  float amplitude = 1.f;
  float total     = 0.f;

  for (uint8_t i = 0; i < octaveCount; ++i) {
    total += getValue(x * frequency, y * frequency) * amplitude;

    amplitude *= 0.5f;
    frequency *= 2.f;
  }

  if (normalize)
    return (total + 1) / 2; // Scaling between [0; 1]

  return total;
}

} // namespace Raz::PerlinNoise
