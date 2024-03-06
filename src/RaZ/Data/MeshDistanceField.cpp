#include "RaZ/Data/BoundingVolumeHierarchy.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/MeshDistanceField.hpp"
#include "RaZ/Math/MathUtils.hpp"
#include "RaZ/Utils/Ray.hpp"
#include "RaZ/Utils/Threading.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

MeshDistanceField::MeshDistanceField(const AABB& area, unsigned int width, unsigned int height, unsigned int depth)
  : m_area{ area }, m_width{ width }, m_height{ height }, m_depth{ depth }, m_distanceField(width * height * depth, std::numeric_limits<float>::max()) {
  if (m_width < 2 || m_height < 2 || m_depth < 2)
    throw std::invalid_argument("[MeshDistanceField] The width, height & depth must all be equal to or greater than 2.");
}

float MeshDistanceField::getDistance(std::size_t widthIndex, std::size_t heightIndex, std::size_t depthIndex) const {
  return m_distanceField[computeIndex(widthIndex, heightIndex, depthIndex)];
}

void MeshDistanceField::compute(std::size_t sampleCount) {
  ZoneScopedN("MeshDistanceField::compute");

  if (m_bvh == nullptr)
    throw std::runtime_error("[MeshDistanceField] Computing a mesh distance field requires having given a BVH.");

  std::fill(m_distanceField.begin(), m_distanceField.end(), std::numeric_limits<float>::max());

  const Vec3f areaExtents = m_area.getMaxPosition() - m_area.getMinPosition();
  const float widthStep   = areaExtents.x() / static_cast<float>(m_width - 1);
  const float heightStep  = areaExtents.y() / static_cast<float>(m_height - 1);
  const float depthStep   = areaExtents.z() / static_cast<float>(m_depth - 1);

  Threading::parallelize(0, m_depth, [this, widthStep, heightStep, depthStep, sampleCount] (const Threading::IndexRange& range) {
    ZoneScopedN("MeshDistanceField::compute");

    for (std::size_t depthIndex = range.beginIndex; depthIndex < range.endIndex; ++depthIndex) {
      for (std::size_t heightIndex = 0; heightIndex < m_height; ++heightIndex) {
        for (std::size_t widthIndex = 0; widthIndex < m_width; ++widthIndex) {
          const Vec3f rayPos = m_area.getMinPosition() + Vec3f(static_cast<float>(widthIndex) * widthStep,
                                                               static_cast<float>(heightIndex) * heightStep,
                                                               static_cast<float>(depthIndex) * depthStep);
          float& distance = m_distanceField[computeIndex(widthIndex, heightIndex, depthIndex)];

          for (const Vec3f& rayDir : MathUtils::computeFibonacciSpherePoints(sampleCount)) {
            RayHit hit {};

            if (!m_bvh->query(Ray(rayPos, rayDir), &hit))
              continue;

            if (rayDir.dot(hit.normal) > 0.f)
              hit.distance = -hit.distance;

            if (std::abs(hit.distance) < std::abs(distance))
              distance = hit.distance;
          }
        }
      }
    }
  });
}

std::vector<Image> MeshDistanceField::recoverSlices() const {
  ZoneScopedN("MeshDistanceField::recoverSlices");

  std::vector<Image> slices;
  slices.reserve(m_depth);

  for (std::size_t depthIndex = 0; depthIndex < m_depth; ++depthIndex) {
    Image& slice = slices.emplace_back(m_width, m_height, ImageColorspace::GRAY, ImageDataType::FLOAT);

    for (std::size_t heightIndex = 0; heightIndex < m_height; ++heightIndex) {
      for (std::size_t widthIndex = 0; widthIndex < m_width; ++widthIndex) {
        const float distance = getDistance(widthIndex, heightIndex, depthIndex);
        slice.setPixel(widthIndex, heightIndex, distance);
      }
    }
  }

  return slices;
}

} // namespace Raz
