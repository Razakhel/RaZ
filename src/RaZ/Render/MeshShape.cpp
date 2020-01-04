#include "RaZ/Math/Constants.hpp"
#include "RaZ/Render/Mesh.hpp"

namespace Raz {

Mesh::Mesh(const Plane& plane, float width, float depth, RenderMode renderMode) : Mesh() {
  const float height = (plane.getNormal() * plane.getDistance())[1];

  const Vec3f firstPos({ -width, height, depth });
  const Vec3f secondPos({ width, height, depth });
  const Vec3f thirdPos({ width, height, -depth });
  const Vec3f fourthPos({ -width, height, -depth });

  Vertex firstCorner {};
  firstCorner.position  = firstPos;
  firstCorner.normal    = plane.getNormal();
  firstCorner.texcoords = Vec2f({ 0.f, 0.f });

  Vertex secondCorner {};
  secondCorner.position  = secondPos;
  secondCorner.normal    = plane.getNormal();
  secondCorner.texcoords = Vec2f({ 1.f, 0.f });

  Vertex thirdCorner {};
  thirdCorner.position  = thirdPos;
  thirdCorner.normal    = plane.getNormal();
  thirdCorner.texcoords = Vec2f({ 1.f, 1.f });

  Vertex fourthCorner {};
  fourthCorner.position  = fourthPos;
  fourthCorner.normal    = plane.getNormal();
  fourthCorner.texcoords = Vec2f({ 0.f, 1.f });

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(4);

  vertices[0] = firstCorner;
  vertices[1] = secondCorner;
  vertices[2] = thirdCorner;
  vertices[3] = fourthCorner;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(6);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  indices[3] = 2;
  indices[4] = 0;
  indices[5] = 3;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Sphere& sphere, uint32_t widthCount, uint32_t heightCount, RenderMode renderMode) : Mesh() {
  // Algorithm based on the standard/UV sphere presented here: http://www.songho.ca/opengl/gl_sphere.html

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.reserve((heightCount + 1) * (widthCount + 1));

  const float widthStep  = 2 * PI<float> / static_cast<float>(widthCount);
  const float heightStep = PI<float> / static_cast<float>(heightCount);
  const float invLength  = 1.f / sphere.getRadius();

  for (unsigned int heightIndex = 0; heightIndex <= heightCount; ++heightIndex) {
    const float heightAngle = PI<float> / 2 - static_cast<float>(heightIndex) * heightStep;

    const float xz = sphere.getRadius() * std::cos(heightAngle);
    const float y  = sphere.getRadius() * std::sin(heightAngle);

    for (unsigned int widthIndex = 0; widthIndex <= widthCount; ++widthIndex) {
      const float widthAngle = static_cast<float>(widthIndex) * widthStep;

      const float x = xz * std::cos(widthAngle);
      const float z = xz * std::sin(widthAngle);

      Vertex vert;
      vert.position  = Vec3f({ x, y, z });
      vert.texcoords = Vec2f({ static_cast<float>(widthIndex) / static_cast<float>(widthCount),
                               static_cast<float>(heightIndex) / static_cast<float>(heightCount) });
      vert.normal    = Vec3f({ x * invLength, y * invLength, z * invLength });
      vert.tangent   = Vec3f({ vert.normal[1], vert.normal[0], vert.normal[2] }); // TODO: how does this give seemingly accurate results in basic cases?

      vertices.emplace_back(vert);
    }
  }

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.reserve(widthCount * 6 + (heightCount - 2) * widthCount * 6);

  // Upper circle
  for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex) {
    const unsigned int widthStride = widthCount + widthIndex;

    indices.push_back(widthIndex + 1);
    indices.push_back(widthStride + 1);
    indices.push_back(widthStride + 2);
  }

  for (unsigned int heightIndex = 1; heightIndex < heightCount - 1; ++heightIndex) {
    unsigned int curHeightStride  = heightIndex * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(curHeightStride);
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride + 1);

      indices.push_back(curHeightStride + 1);
      indices.push_back(nextHeightStride);
      indices.push_back(nextHeightStride + 1);
    }
  }

  // Lower circle
  {
    unsigned int curHeightStride  = (heightCount - 1) * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(curHeightStride);
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride + 1);
    }
  }

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Triangle& triangle, RenderMode renderMode) : Mesh() {
  // TODO: check if vertices are defined counterclockwise

  const Vec3f& firstPos  = triangle.getFirstPos();
  const Vec3f& secondPos = triangle.getSecondPos();
  const Vec3f& thirdPos  = triangle.getThirdPos();

  Vertex firstVert {};
  firstVert.position  = firstPos;
  firstVert.texcoords = Vec2f({ 0.f, 0.f });

  Vertex secondVert {};
  secondVert.position  = secondPos;
  secondVert.texcoords = Vec2f({ 0.5f, 1.f });

  Vertex thirdVert {};
  thirdVert.position  = thirdPos;
  thirdVert.texcoords = Vec2f({ 1.f, 0.f });

  // Computing normals
  firstVert.normal  = (firstPos - secondPos).cross(firstPos - thirdPos).normalize();
  secondVert.normal = (secondPos - thirdPos).cross(secondPos - firstPos).normalize();
  thirdVert.normal  = (thirdPos - firstPos).cross(thirdPos - secondPos).normalize();

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(3);

  vertices[0] = firstVert;
  vertices[1] = secondVert;
  vertices[2] = thirdVert;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(3);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Quad& quad, RenderMode renderMode) : Mesh() {
  const Vec3f& leftTopPos     = quad.getLeftTopPos();
  const Vec3f& rightTopPos    = quad.getRightTopPos();
  const Vec3f& rightBottomPos = quad.getRightBottomPos();
  const Vec3f& leftBottomPos  = quad.getLeftBottomPos();

  Vertex leftTop {};
  leftTop.position  = leftTopPos;
  leftTop.texcoords = Vec2f({ 0.f, 1.f });

  Vertex rightTop {};
  rightTop.position  = rightTopPos;
  rightTop.texcoords = Vec2f({ 1.f, 1.f });

  Vertex rightBottom {};
  rightBottom.position  = rightBottomPos;
  rightBottom.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftBottom {};
  leftBottom.position  = leftBottomPos;
  leftBottom.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display quads like a framebuffer
  leftTop.normal     = (leftTopPos - rightTopPos).cross(leftTopPos - leftBottomPos).normalize();
  rightTop.normal    = (rightTopPos - rightBottomPos).cross(rightTopPos - leftTopPos).normalize();
  rightBottom.normal = (rightBottomPos - leftBottomPos).cross(rightBottomPos - rightTopPos).normalize();
  leftBottom.normal  = (leftBottomPos - leftTopPos).cross(leftBottomPos - rightBottomPos).normalize();

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(4);

  vertices[0] = leftTop;
  vertices[1] = leftBottom;
  vertices[2] = rightBottom;
  vertices[3] = rightTop;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(6);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const AABB& box, RenderMode renderMode) : Mesh() {
  const Vec3f& rightTopFrontPos  = box.getRightTopFrontPos();
  const Vec3f& leftBottomBackPos = box.getLeftBottomBackPos();

  const float rightPos  = rightTopFrontPos[0];
  const float leftPos   = leftBottomBackPos[0];
  const float topPos    = rightTopFrontPos[1];
  const float bottomPos = leftBottomBackPos[1];
  const float frontPos  = rightTopFrontPos[2];
  const float backPos   = leftBottomBackPos[2];

  // TODO: texcoords should not exist for simple display cubes like a skybox

  Vertex rightTopBack {};
  rightTopBack.position  = Vec3f({ rightPos, topPos, backPos });
  rightTopBack.texcoords = Vec2f({ 0.f, 1.f });

  Vertex rightTopFront {};
  rightTopFront.position  = rightTopFrontPos;
  rightTopFront.texcoords = Vec2f({ 1.f, 1.f });

  Vertex rightBottomBack {};
  rightBottomBack.position  = Vec3f({ rightPos, bottomPos, backPos });
  rightBottomBack.texcoords = Vec2f({ 0.f, 0.f });

  Vertex rightBottomFront {};
  rightBottomFront.position  = Vec3f({ rightPos, bottomPos, frontPos });
  rightBottomFront.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftTopBack {};
  leftTopBack.position  = Vec3f({ leftPos, topPos, backPos });
  leftTopBack.texcoords = Vec2f({ 1.f, 1.f });

  Vertex leftTopFront {};
  leftTopFront.position  = Vec3f({ leftPos, topPos, frontPos });
  leftTopFront.texcoords = Vec2f({ 0.f, 1.f });

  Vertex leftBottomBack {};
  leftBottomBack.position  = leftBottomBackPos;
  leftBottomBack.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftBottomFront {};
  leftBottomFront.position  = Vec3f({ leftPos, bottomPos, frontPos });
  leftBottomFront.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display cubes like a skybox
  // TODO: compute normals

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(8);

  vertices[0] = rightTopBack;
  vertices[1] = rightTopFront;

  vertices[2] = rightBottomBack;
  vertices[3] = rightBottomFront;

  vertices[4] = leftTopBack;
  vertices[5] = leftTopFront;

  vertices[6] = leftBottomBack;
  vertices[7] = leftBottomFront;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(36);

  // Right face
  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  indices[3] = 1;
  indices[4] = 2;
  indices[5] = 3;

  // Left face
  indices[6] = 4;
  indices[7] = 5;
  indices[8] = 7;

  indices[9]  = 4;
  indices[10] = 7;
  indices[11] = 6;

  // Top face
  indices[12] = 4;
  indices[13] = 0;
  indices[14] = 1;

  indices[15] = 4;
  indices[16] = 1;
  indices[17] = 5;

  // Bottom face
  indices[18] = 7;
  indices[19] = 3;
  indices[20] = 2;

  indices[21] = 7;
  indices[22] = 2;
  indices[23] = 6;

  // Front face
  indices[24] = 5;
  indices[25] = 1;
  indices[26] = 3;

  indices[27] = 5;
  indices[28] = 3;
  indices[29] = 7;

  // Back face
  indices[30] = 0;
  indices[31] = 4;
  indices[32] = 6;

  indices[33] = 0;
  indices[34] = 6;
  indices[35] = 2;

  setRenderMode(renderMode);
  load();
}

const AABB& Mesh::computeBoundingBox() {
  Vec3f maxPos;
  Vec3f minPos;

  for (Submesh& submesh : m_submeshes) {
    const AABB& boundingBox = submesh.computeBoundingBox();

    maxPos[0] = std::max(maxPos[0], boundingBox.getRightTopFrontPos()[0]);
    maxPos[1] = std::max(maxPos[1], boundingBox.getRightTopFrontPos()[1]);
    maxPos[2] = std::max(maxPos[2], boundingBox.getRightTopFrontPos()[2]);

    minPos[0] = std::min(minPos[0], boundingBox.getLeftBottomBackPos()[0]);
    minPos[1] = std::min(minPos[1], boundingBox.getLeftBottomBackPos()[1]);
    minPos[2] = std::min(minPos[2], boundingBox.getLeftBottomBackPos()[2]);
  }

  m_boundingBox = AABB(maxPos, minPos);
  return m_boundingBox;
}

} // namespace Raz
