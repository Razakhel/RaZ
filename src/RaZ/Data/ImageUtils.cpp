#include "RaZ/Data/ImageUtils.hpp"
#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Utils/Threading.hpp"

#include "tracy/Tracy.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace Raz {

std::array<Image, 6> ImageUtils::convertEquirectangularToCubemap(const Image& equirectangularImg) {
  ZoneScopedN("ImageUtils::convertEquirectangularToCubemap");

  if (equirectangularImg.isEmpty())
    throw std::invalid_argument("[ImageUtils] Empty equirectangular image given");

  const unsigned int faceSize  = equirectangularImg.getHeight() / 2;
  const float invFaceSize      = 1.f / static_cast<float>(faceSize);
  const auto srcWidth          = static_cast<int>(equirectangularImg.getWidth());
  const auto srcHeight         = static_cast<int>(equirectangularImg.getHeight());
  const ImageDataType dataType = equirectangularImg.getDataType();
  const uint8_t channelCount   = equirectangularImg.getChannelCount();

  std::array<Image, 6> faces;

  for (unsigned int faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
    Image& faceImg = faces[faceIndex];
    faceImg        = Image(faceSize, faceSize, equirectangularImg.getColorspace(), dataType);

    Threading::parallelize(0, faceSize, [&faceImg, &equirectangularImg, faceSize, invFaceSize, faceIndex, srcWidth, srcHeight, channelCount, dataType] (const Threading::IndexRange& range) {
      ZoneScopedN("ImageUtils::convertEquirectangularToCubemap");

      for (std::size_t heightIndex = range.beginIndex; heightIndex < range.endIndex; ++heightIndex) {
        const float faceV = 2.f * (static_cast<float>(heightIndex) + 0.5f) * invFaceSize - 1.f;

        for (std::size_t widthIndex = 0; widthIndex < faceSize; ++widthIndex) {
          const float faceU = 2.f * (static_cast<float>(widthIndex) + 0.5f) * invFaceSize - 1.f;

          Vec3f direction;

          switch (faceIndex) {
            case 0: direction = Vec3f( 1.f,   -faceV, -faceU); break; // +X (right)
            case 1: direction = Vec3f(-1.f,   -faceV,  faceU); break; // -X (left)
            case 2: direction = Vec3f( faceU,  1.f,    faceV); break; // +Y (top)
            case 3: direction = Vec3f( faceU, -1.f,   -faceV); break; // -Y (bottom)
            case 4: direction = Vec3f( faceU, -faceV,  1.f);   break; // +Z (front)
            case 5: direction = Vec3f(-faceU, -faceV, -1.f);   break; // -Z (back)
            default: assert(false);
          }

          const float theta = std::atan2(direction.z(), direction.x()); // Longitude
          const float phi   = std::asin(direction.y() / direction.computeLength()); // Latitude

          const float srcPixelX = (theta / (2.f * std::numbers::pi_v<float>) + 0.5f) * static_cast<float>(srcWidth);
          const float srcPixelY = (0.5f - phi / std::numbers::pi_v<float>) * static_cast<float>(srcHeight);

          const auto srcBaseX = static_cast<int>(std::floor(srcPixelX - 0.5f));
          const auto srcBaseY = static_cast<int>(std::floor(srcPixelY - 0.5f));
          const float fracX   = srcPixelX - 0.5f - static_cast<float>(srcBaseX);
          const float fracY   = srcPixelY - 0.5f - static_cast<float>(srcBaseY);

          // Horizontal wrap (panorama is 360°)
          const auto wrapX = [srcWidth] (int x) constexpr noexcept {
            return static_cast<std::size_t>(((x % srcWidth) + srcWidth) % srcWidth);
          };
          // Vertical clamp (at the poles)
          const auto clampY = [srcHeight] (int y) constexpr noexcept {
            return static_cast<std::size_t>(std::clamp(y, 0, srcHeight - 1));
          };

          const std::size_t srcLeftIndex   = wrapX(srcBaseX);
          const std::size_t srcRightIndex  = wrapX(srcBaseX + 1);
          const std::size_t srcTopIndex    = clampY(srcBaseY);
          const std::size_t srcBottomIndex = clampY(srcBaseY + 1);

          for (uint8_t channelIndex = 0; channelIndex < channelCount; ++channelIndex) {
            if (dataType == ImageDataType::FLOAT) {
              const float topLeftVal     = equirectangularImg.recoverFloatValue(srcLeftIndex, srcTopIndex, channelIndex);
              const float topRightVal    = equirectangularImg.recoverFloatValue(srcRightIndex, srcTopIndex, channelIndex);
              const float bottomLeftVal  = equirectangularImg.recoverFloatValue(srcLeftIndex, srcBottomIndex, channelIndex);
              const float bottomRightVal = equirectangularImg.recoverFloatValue(srcRightIndex, srcBottomIndex, channelIndex);
              const float finalVal       = MathUtils::bilerp(topLeftVal, topRightVal, bottomLeftVal, bottomRightVal, fracX, fracY);
              faceImg.setFloatValue(widthIndex, heightIndex, channelIndex, finalVal);
            } else {
              const auto topLeftVal     = static_cast<float>(equirectangularImg.recoverByteValue(srcLeftIndex, srcTopIndex, channelIndex));
              const auto topRightVal    = static_cast<float>(equirectangularImg.recoverByteValue(srcRightIndex, srcTopIndex, channelIndex));
              const auto bottomLeftVal  = static_cast<float>(equirectangularImg.recoverByteValue(srcLeftIndex, srcBottomIndex, channelIndex));
              const auto bottomRightVal = static_cast<float>(equirectangularImg.recoverByteValue(srcRightIndex, srcBottomIndex, channelIndex));
              const float finalVal      = MathUtils::bilerp(topLeftVal, topRightVal, bottomLeftVal, bottomRightVal, fracX, fracY);
              faceImg.setByteValue(widthIndex, heightIndex, channelIndex, static_cast<uint8_t>(std::clamp(std::round(finalVal), 0.f, 255.f)));
            }
          }
        }
      }
    });
  }

  return faces;
}

} // namespace Raz
