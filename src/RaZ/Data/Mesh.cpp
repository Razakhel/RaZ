#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Constants.hpp"
#include "RaZ/Utils/Threading.hpp"

#include <unordered_map>

namespace Raz {

namespace {

constexpr Vec3f computeTangent(const Vertex& firstVert, const Vertex& secondVert, const Vertex& thirdVert) noexcept {
  const Vec3f firstEdge  = secondVert.position - firstVert.position;
  const Vec3f secondEdge = thirdVert.position - firstVert.position;

  const Vec2f firstUvDiff  = secondVert.texcoords - firstVert.texcoords;
  const Vec2f secondUvDiff = thirdVert.texcoords - firstVert.texcoords;

  const float denominator = (firstUvDiff.x() * secondUvDiff.y() - secondUvDiff.x() * firstUvDiff.y());

  if (denominator == 0.f)
    return Vec3f(0.f);

  return (firstEdge * secondUvDiff.y() - secondEdge * firstUvDiff.y()) / denominator;
}

} // namespace

Mesh::Mesh(const Plane& plane, float width, float depth) {
  const float height = plane.computeCentroid().y();

  // TODO: creating a Mesh from a Plane doesn't take the normal into account for the vertices' position
  const Vec3f firstPos(-width, height, depth);
  const Vec3f secondPos(width, height, depth);
  const Vec3f thirdPos(width, height, -depth);
  const Vec3f fourthPos(-width, height, -depth);

  Vertex firstCorner {};
  firstCorner.position  = firstPos;
  firstCorner.normal    = plane.getNormal();
  firstCorner.texcoords = Vec2f(0.f, 0.f);

  Vertex secondCorner {};
  secondCorner.position  = secondPos;
  secondCorner.normal    = plane.getNormal();
  secondCorner.texcoords = Vec2f(1.f, 0.f);

  Vertex thirdCorner {};
  thirdCorner.position  = thirdPos;
  thirdCorner.normal    = plane.getNormal();
  thirdCorner.texcoords = Vec2f(1.f, 1.f);

  Vertex fourthCorner {};
  fourthCorner.position  = fourthPos;
  fourthCorner.normal    = plane.getNormal();
  fourthCorner.texcoords = Vec2f(0.f, 1.f);

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(4);

  vertices[0] = firstCorner;
  vertices[1] = secondCorner;
  vertices[2] = thirdCorner;
  vertices[3] = fourthCorner;

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.resize(6);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  computeTangents();
}

Mesh::Mesh(const Sphere& sphere, uint32_t subdivCount, SphereMeshType type) {
  switch (type) {
    case SphereMeshType::UV:
      createUvSphere(sphere, subdivCount, subdivCount);
      break;

    case SphereMeshType::ICO:
      createIcosphere(sphere, subdivCount);
      break;
  }

  computeTangents();
}

Mesh::Mesh(const Triangle& triangle, const Vec2f& firstTexcoords, const Vec2f& secondTexcoords, const Vec2f& thirdTexcoords) {
  const Vec3f& firstPos  = triangle.getFirstPos();
  const Vec3f& secondPos = triangle.getSecondPos();
  const Vec3f& thirdPos  = triangle.getThirdPos();
  const Vec3f normal     = triangle.computeNormal();

  Vertex firstVert {};
  firstVert.position  = firstPos;
  firstVert.texcoords = firstTexcoords;
  firstVert.normal    = normal;

  Vertex secondVert {};
  secondVert.position  = secondPos;
  secondVert.texcoords = secondTexcoords;
  secondVert.normal    = normal;

  Vertex thirdVert {};
  thirdVert.position  = thirdPos;
  thirdVert.texcoords = thirdTexcoords;
  thirdVert.normal    = normal;

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(3);

  vertices[0] = firstVert;
  vertices[1] = secondVert;
  vertices[2] = thirdVert;

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.resize(3);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  computeTangents();
}

Mesh::Mesh(const Quad& quad) {
  const Vec3f& leftTopPos     = quad.getLeftTopPos();
  const Vec3f& rightTopPos    = quad.getRightTopPos();
  const Vec3f& rightBottomPos = quad.getRightBottomPos();
  const Vec3f& leftBottomPos  = quad.getLeftBottomPos();

  Vertex leftTop {};
  leftTop.position  = leftTopPos;
  leftTop.texcoords = Vec2f(0.f, 1.f);

  Vertex rightTop {};
  rightTop.position  = rightTopPos;
  rightTop.texcoords = Vec2f(1.f, 1.f);

  Vertex rightBottom {};
  rightBottom.position  = rightBottomPos;
  rightBottom.texcoords = Vec2f(1.f, 0.f);

  Vertex leftBottom {};
  leftBottom.position  = leftBottomPos;
  leftBottom.texcoords = Vec2f(0.f, 0.f);

  // Computing normals
  leftTop.normal     = (leftTopPos - rightTopPos).cross(leftBottomPos - leftTopPos).normalize();
  rightTop.normal    = (rightTopPos - rightBottomPos).cross(leftTopPos - rightTopPos).normalize();
  rightBottom.normal = (rightBottomPos - leftBottomPos).cross(rightTopPos - rightBottomPos).normalize();
  leftBottom.normal  = (leftBottomPos - leftTopPos).cross(rightBottomPos - leftBottomPos).normalize();

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(4);

  vertices[0] = leftTop;
  vertices[1] = leftBottom;
  vertices[2] = rightBottom;
  vertices[3] = rightTop;

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.resize(6);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  computeTangents();
}

Mesh::Mesh(const AABB& box) {
  const Vec3f& minPos = box.getMinPosition();
  const Vec3f& maxPos = box.getMaxPosition();

  const float minX = minPos.x();
  const float maxX = maxPos.x();
  const float minY = minPos.y();
  const float maxY = maxPos.y();
  const float minZ = minPos.z();
  const float maxZ = maxPos.z();

  Vertex rightTopBack {};
  rightTopBack.position  = Vec3f(maxX, maxY, minZ);
  rightTopBack.texcoords = Vec2f(0.f, 1.f);

  Vertex rightTopFront {};
  rightTopFront.position  = maxPos;
  rightTopFront.texcoords = Vec2f(1.f, 1.f);

  Vertex rightBottomBack {};
  rightBottomBack.position  = Vec3f(maxX, minY, minZ);
  rightBottomBack.texcoords = Vec2f(0.f, 0.f);

  Vertex rightBottomFront {};
  rightBottomFront.position  = Vec3f(maxX, minY, maxZ);
  rightBottomFront.texcoords = Vec2f(1.f, 0.f);

  Vertex leftTopBack {};
  leftTopBack.position  = Vec3f(minX, maxY, minZ);
  leftTopBack.texcoords = Vec2f(1.f, 1.f);

  Vertex leftTopFront {};
  leftTopFront.position  = Vec3f(minX, maxY, maxZ);
  leftTopFront.texcoords = Vec2f(0.f, 1.f);

  Vertex leftBottomBack {};
  leftBottomBack.position  = minPos;
  leftBottomBack.texcoords = Vec2f(1.f, 0.f);

  Vertex leftBottomFront {};
  leftBottomFront.position  = Vec3f(minX, minY, maxZ);
  leftBottomFront.texcoords = Vec2f(0.f, 0.f);

  // Computing normals
  // TODO: compute normals

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(8);

  vertices[0] = rightTopBack;
  vertices[1] = rightTopFront;

  vertices[2] = rightBottomBack;
  vertices[3] = rightBottomFront;

  vertices[4] = leftTopBack;
  vertices[5] = leftTopFront;

  vertices[6] = leftBottomBack;
  vertices[7] = leftBottomFront;

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.resize(36);

  // Right face
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 2;
  indices[4] = 1;
  indices[5] = 3;

  // Left face
  indices[6] = 5;
  indices[7] = 4;
  indices[8] = 7;

  indices[9]  = 7;
  indices[10] = 4;
  indices[11] = 6;

  // Top face
  indices[12] = 0;
  indices[13] = 4;
  indices[14] = 1;

  indices[15] = 1;
  indices[16] = 4;
  indices[17] = 5;

  // Bottom face
  indices[18] = 3;
  indices[19] = 7;
  indices[20] = 2;

  indices[21] = 2;
  indices[22] = 7;
  indices[23] = 6;

  // Front face
  indices[24] = 1;
  indices[25] = 5;
  indices[26] = 3;

  indices[27] = 3;
  indices[28] = 5;
  indices[29] = 7;

  // Back face
  indices[30] = 4;
  indices[31] = 0;
  indices[32] = 6;

  indices[33] = 6;
  indices[34] = 0;
  indices[35] = 2;

  computeTangents();
}

std::size_t Mesh::recoverVertexCount() const {
  std::size_t vertexCount = 0;

  for (const Submesh& submesh : m_submeshes)
    vertexCount += submesh.getVertexCount();

  return vertexCount;
}

std::size_t Mesh::recoverTriangleCount() const {
  std::size_t indexCount = 0;

  for (const Submesh& submesh : m_submeshes)
    indexCount += submesh.getTriangleIndexCount();

  return indexCount / 3;
}

const AABB& Mesh::computeBoundingBox() {
  Vec3f minPos(std::numeric_limits<float>::max());
  Vec3f maxPos(std::numeric_limits<float>::lowest());

  for (Submesh& submesh : m_submeshes) {
    const AABB& boundingBox = submesh.computeBoundingBox();

    minPos.x() = std::min(minPos.x(), boundingBox.getMinPosition().x());
    minPos.y() = std::min(minPos.y(), boundingBox.getMinPosition().y());
    minPos.z() = std::min(minPos.z(), boundingBox.getMinPosition().z());

    maxPos.x() = std::max(maxPos.x(), boundingBox.getMaxPosition().x());
    maxPos.y() = std::max(maxPos.y(), boundingBox.getMaxPosition().y());
    maxPos.z() = std::max(maxPos.z(), boundingBox.getMaxPosition().z());
  }

  m_boundingBox = AABB(minPos, maxPos);
  return m_boundingBox;
}

void Mesh::computeTangents() {
  Threading::parallelize(m_submeshes, [] (const auto& range) noexcept {
    for (Submesh& submesh : range) {
      for (std::size_t i = 0; i < submesh.getTriangleIndexCount(); i += 3) {
        Vertex& firstVert  = submesh.getVertices()[submesh.getTriangleIndices()[i    ]];
        Vertex& secondVert = submesh.getVertices()[submesh.getTriangleIndices()[i + 1]];
        Vertex& thirdVert  = submesh.getVertices()[submesh.getTriangleIndices()[i + 2]];

        const Vec3f tangent = computeTangent(firstVert, secondVert, thirdVert);

        // Adding the computed tangent to each vertex; they will be normalized later
        firstVert.tangent  += tangent;
        secondVert.tangent += tangent;
        thirdVert.tangent  += tangent;
      }

      // Normalizing the accumulated tangents
      for (Vertex& vert : submesh.getVertices()) {
        // Avoiding NaNs by preventing normalization of a 0 vector
        if (vert.tangent.strictlyEquals(Vec3f(0.f)))
          continue;

        vert.tangent = (vert.tangent - vert.normal * vert.tangent.dot(vert.normal)).normalize();
      }
    }
  });
}

void Mesh::createUvSphere(const Sphere& sphere, uint32_t widthCount, uint32_t heightCount) {
  // Algorithm based on the standard/UV sphere presented here: http://www.songho.ca/opengl/gl_sphere.html#sphere

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.reserve((heightCount + 1) * (widthCount + 1));

  const float widthStep  = 2 * Pi<float> / static_cast<float>(widthCount);
  const float heightStep = Pi<float> / static_cast<float>(heightCount);
  const Vec3f center     = sphere.getCenter();

  for (unsigned int heightIndex = 0; heightIndex <= heightCount; ++heightIndex) {
    const float heightAngle = Pi<float> / 2 - static_cast<float>(heightIndex) * heightStep;

    const float xz = sphere.getRadius() * std::cos(heightAngle);
    const float y  = sphere.getRadius() * std::sin(heightAngle);

    for (unsigned int widthIndex = 0; widthIndex <= widthCount; ++widthIndex) {
      const float widthAngle = static_cast<float>(widthIndex) * widthStep;

      const float x = xz * std::cos(widthAngle);
      const float z = xz * std::sin(widthAngle);

      Vertex vert;
      vert.position  = Vec3f(x + center.x(), y + center.y(), z + center.z());
      vert.texcoords = Vec2f(static_cast<float>(widthIndex) / static_cast<float>(widthCount),
                             static_cast<float>(heightIndex) / static_cast<float>(heightCount));
      vert.normal    = Vec3f(x, y, z).normalize(); // Dividing by the inverse radius does not give a perfectly unit vector; normalizing directly

      vertices.emplace_back(vert);
    }
  }

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.reserve(widthCount * 6 + (heightCount - 2) * widthCount * 6);

  // Upper circle
  for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex) {
    const unsigned int widthStride = widthCount + widthIndex;

    indices.push_back(widthStride + 1);
    indices.push_back(widthIndex + 1);
    indices.push_back(widthStride + 2);
  }

  for (unsigned int heightIndex = 1; heightIndex < heightCount - 1; ++heightIndex) {
    unsigned int curHeightStride  = heightIndex * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride);
      indices.push_back(curHeightStride + 1);

      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride + 1);
      indices.push_back(nextHeightStride + 1);
    }
  }

  // Lower circle
  {
    unsigned int curHeightStride  = (heightCount - 1) * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride);
      indices.push_back(curHeightStride + 1);
    }
  }
}

void Mesh::createIcosphere(const Sphere& sphere, uint32_t /* subdivCount */) {
  // Algorithm based on the icosphere presented here:
  // - http://www.songho.ca/opengl/gl_sphere.html#icosphere
  // - https://gist.github.com/warmwaffles/402b9c04318d6ee6dfa4

  const float radius       = sphere.getRadius();
  const float goldenRadius = radius * GoldenRatio<float>;

  Submesh& submesh = m_submeshes.emplace_back();

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(12);

  constexpr float invFactor = 1.f / (Pi<float> * 2);

  vertices[0].normal    = Vec3f(-radius, goldenRadius, 0.f).normalize();
  vertices[0].position  = vertices[0].normal * radius;
  vertices[0].texcoords = Vec2f(std::atan2(vertices[0].normal[0], vertices[0].normal[2]) * invFactor + 0.5f, vertices[0].normal[1] * 0.5f + 0.5f);

  vertices[1].normal    = Vec3f(radius, goldenRadius, 0.f).normalize();
  vertices[1].position  = vertices[1].normal * radius;
  vertices[1].texcoords = Vec2f(std::atan2(vertices[1].normal[0], vertices[1].normal[2]) * invFactor + 0.5f, vertices[1].normal[1] * 0.5f + 0.5f);

  vertices[2].normal    = Vec3f(-radius, -goldenRadius, 0.f).normalize();
  vertices[2].position  = vertices[2].normal * radius;
  vertices[2].texcoords = Vec2f(std::atan2(vertices[2].normal[0], vertices[2].normal[2]) * invFactor + 0.5f, vertices[2].normal[1] * 0.5f + 0.5f);

  vertices[3].normal    = Vec3f(radius, -goldenRadius, 0.f).normalize();
  vertices[3].position  = vertices[3].normal * radius;
  vertices[3].texcoords = Vec2f(std::atan2(vertices[3].normal[0], vertices[3].normal[2]) * invFactor + 0.5f, vertices[3].normal[1] * 0.5f + 0.5f);

  vertices[4].normal    = Vec3f(0.f, -radius, goldenRadius).normalize();
  vertices[4].position  = vertices[4].normal * radius;
  vertices[4].texcoords = Vec2f(std::atan2(vertices[4].normal[0], vertices[4].normal[2]) * invFactor + 0.5f, vertices[4].normal[1] * 0.5f + 0.5f);

  vertices[5].normal    = Vec3f(0.f, radius, goldenRadius).normalize();
  vertices[5].position  = vertices[5].normal * radius;
  vertices[5].texcoords = Vec2f(std::atan2(vertices[5].normal[0], vertices[5].normal[2]) * invFactor + 0.5f, vertices[5].normal[1] * 0.5f + 0.5f);

  vertices[6].normal    = Vec3f(0.f, -radius, -goldenRadius).normalize();
  vertices[6].position  = vertices[6].normal * radius;
  vertices[6].texcoords = Vec2f(std::atan2(vertices[6].normal[0], vertices[6].normal[2]) * invFactor + 0.5f, vertices[6].normal[1] * 0.5f + 0.5f);

  vertices[7].normal    = Vec3f(0.f, radius, -goldenRadius).normalize();
  vertices[7].position  = vertices[7].normal * radius;
  vertices[7].texcoords = Vec2f(std::atan2(vertices[7].normal[0], vertices[7].normal[2]) * invFactor + 0.5f, vertices[7].normal[1] * 0.5f + 0.5f);

  vertices[8].normal    = Vec3f(goldenRadius, 0.f, -radius).normalize();
  vertices[8].position  = vertices[8].normal * radius;
  vertices[8].texcoords = Vec2f(std::atan2(vertices[8].normal[0], vertices[8].normal[2]) * invFactor + 0.5f, vertices[8].normal[1] * 0.5f + 0.5f);

  vertices[9].normal    = Vec3f(goldenRadius, 0.f, radius).normalize();
  vertices[9].position  = vertices[9].normal * radius;
  vertices[9].texcoords = Vec2f(std::atan2(vertices[9].normal[0], vertices[9].normal[2]) * invFactor + 0.5f, vertices[9].normal[1] * 0.5f + 0.5f);

  vertices[10].normal    = Vec3f(-goldenRadius, 0.f, -radius).normalize();
  vertices[10].position  = vertices[10].normal * radius;
  vertices[10].texcoords = Vec2f(std::atan2(vertices[10].normal[0], vertices[10].normal[2]) * invFactor + 0.5f, vertices[10].normal[1] * 0.5f + 0.5f);

  vertices[11].normal    = Vec3f(-goldenRadius, 0.f, radius).normalize();
  vertices[11].position  = vertices[11].normal * radius;
  vertices[11].texcoords = Vec2f(std::atan2(vertices[11].normal[0], vertices[11].normal[2]) * invFactor + 0.5f, vertices[11].normal[1] * 0.5f + 0.5f);

  // Applying a translation on each vertex by the sphere's center
  for (Vertex& vertex : vertices)
    vertex.position += sphere.getCenter();

  std::vector<unsigned int>& indices = submesh.getTriangleIndices();
  indices.resize(60);

  indices[0] = 5;
  indices[1] = 0;
  indices[2] = 11;

  indices[3] = 1;
  indices[4] = 0;
  indices[5] = 5;

  indices[6] = 7;
  indices[7] = 0;
  indices[8] = 1;

  indices[9]  = 10;
  indices[10] = 0;
  indices[11] = 7;

  indices[12] = 11;
  indices[13] = 0;
  indices[14] = 10;

  indices[15] = 9;
  indices[16] = 1;
  indices[17] = 5;

  indices[18] = 4;
  indices[19] = 5;
  indices[20] = 11;

  indices[21] = 2;
  indices[22] = 11;
  indices[23] = 10;

  indices[24] = 6;
  indices[25] = 10;
  indices[26] = 7;

  indices[27] = 8;
  indices[28] = 7;
  indices[29] = 1;

  indices[30] = 4;
  indices[31] = 3;
  indices[32] = 9;

  indices[33] = 2;
  indices[34] = 3;
  indices[35] = 4;

  indices[36] = 6;
  indices[37] = 3;
  indices[38] = 2;

  indices[39] = 8;
  indices[40] = 3;
  indices[41] = 6;

  indices[42] = 9;
  indices[43] = 3;
  indices[44] = 8;

  indices[45] = 5;
  indices[46] = 4;
  indices[47] = 9;

  indices[48] = 11;
  indices[49] = 2;
  indices[50] = 4;

  indices[51] = 10;
  indices[52] = 6;
  indices[53] = 2;

  indices[54] = 7;
  indices[55] = 8;
  indices[56] = 6;

  indices[57] = 1;
  indices[58] = 9;
  indices[59] = 8;

  // Applying subdivisions

  // TODO: subdivisions are clearly broken for the moment, must be investigated

//  std::unordered_map<Vertex, std::size_t> verticesIndices;
//  verticesIndices.emplace(vertices[0], 0);
//  verticesIndices.emplace(vertices[1], 1);
//  verticesIndices.emplace(vertices[2], 2);
//  verticesIndices.emplace(vertices[3], 3);
//  verticesIndices.emplace(vertices[4], 4);
//  verticesIndices.emplace(vertices[5], 5);
//  verticesIndices.emplace(vertices[6], 6);
//  verticesIndices.emplace(vertices[7], 7);
//  verticesIndices.emplace(vertices[8], 8);
//  verticesIndices.emplace(vertices[9], 9);
//  verticesIndices.emplace(vertices[10], 10);
//  verticesIndices.emplace(vertices[11], 11);
//
//  for (uint32_t subdivIndex = 0; subdivIndex < subdivCount; ++subdivIndex) {
//    std::vector<Vertex> newVertices;
//    newVertices.reserve(vertices.size() * 2);
//
//    std::vector<unsigned int> newIndices;
//    newIndices.reserve(indices.size() * 9);
//
//    const std::size_t indexCount = indices.size();
//
//    for (std::size_t triangleIndex = 0; triangleIndex < indexCount; triangleIndex += 3) {
//      // Recovering the original vertices
//      //       3
//      //      / \
//      //     /   \
//      //    /     \
//      //   /       \
//      //  /         \
//      // 1-----------2
//
//      const Vertex& firstVert  = vertices[indices[triangleIndex]];
//      const Vertex& secondVert = vertices[indices[triangleIndex + 1]];
//      const Vertex& thirdVert  = vertices[indices[triangleIndex + 2]];
//
//      newVertices.emplace_back(firstVert);
//      newVertices.emplace_back(secondVert);
//      newVertices.emplace_back(thirdVert);
//
//      // Computing the edge vertices to form another triangle
//      //       x
//      //      / \
//      //     /   \
//      //    3-----2
//      //   / \   / \
//      //  /   \ /   \
//      // x-----1-----x
//
//      Vertex& firstEdgeVert   = newVertices.emplace_back();
//      firstEdgeVert.position  = (firstVert.position + secondVert.position) * 0.5f;
//      firstEdgeVert.texcoords = (firstVert.texcoords + secondVert.texcoords) * 0.5f;
//      firstEdgeVert.normal    = (firstVert.normal + secondVert.normal).normalize();
//
//      Vertex& secondEdgeVert   = newVertices.emplace_back();
//      secondEdgeVert.position  = (secondVert.position + thirdVert.position) * 0.5f;
//      secondEdgeVert.texcoords = (secondVert.texcoords + thirdVert.texcoords) * 0.5f;
//      secondEdgeVert.normal    = (secondVert.normal + thirdVert.normal).normalize();
//
//      Vertex& thirdEdgeVert   = newVertices.emplace_back();
//      thirdEdgeVert.position  = (thirdVert.position + firstVert.position) * 0.5f;
//      thirdEdgeVert.texcoords = (thirdVert.texcoords + firstVert.texcoords) * 0.5f;
//      thirdEdgeVert.normal    = (thirdVert.normal + firstVert.normal).normalize();
//
//      // Adding indices to create the 4 resulting triangles
//      //       x
//      //      / \
//      //     / 4 \
//      //    x-----x
//      //   / \ 1 / \
//      //  / 2 \ / 3 \
//      // x-----x-----x
//
//      const std::size_t firstEdgeVertIndex = newVertices.size() - 3;
//      const std::size_t secondEdgeVertIndex = newVertices.size() - 2;
//      const std::size_t thirdEdgeVertIndex = newVertices.size() - 1;
//
//      verticesIndices.emplace(firstEdgeVert, firstEdgeVertIndex);
//      verticesIndices.emplace(secondEdgeVert, secondEdgeVertIndex);
//      verticesIndices.emplace(thirdEdgeVert, thirdEdgeVertIndex);
//
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(secondEdgeVertIndex);
//      newIndices.emplace_back(thirdEdgeVertIndex);
//
//      newIndices.emplace_back(verticesIndices.find(firstVert)->second);
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(thirdEdgeVertIndex);
//
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(verticesIndices.find(secondVert)->second);
//      newIndices.emplace_back(secondEdgeVertIndex);
//
//      newIndices.emplace_back(thirdEdgeVertIndex);
//      newIndices.emplace_back(secondEdgeVertIndex);
//      newIndices.emplace_back(verticesIndices.find(thirdVert)->second);
//    }
//
//    vertices = std::move(newVertices);
//    indices  = std::move(newIndices);
//  }
}

} // namespace Raz
