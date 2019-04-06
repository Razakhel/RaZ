#include "RaZ/Render/Mesh.hpp"

namespace Raz {

Mesh::Mesh(const Triangle& triangle) : Mesh() {
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

  std::vector<Vertex>& vertices = m_submeshes.front()->getVertices();
  vertices.resize(3);

  vertices[0] = firstVert;
  vertices[1] = secondVert;
  vertices[2] = thirdVert;

  std::vector<unsigned int>& indices = m_submeshes.front()->getIndices();
  indices.resize(3);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  load();
}

Mesh::Mesh(const Quad& quad) : Mesh() {
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

  std::vector<Vertex>& vertices = m_submeshes.front()->getVertices();
  vertices.resize(4);

  vertices[0] = leftTop;
  vertices[1] = leftBottom;
  vertices[2] = rightBottom;
  vertices[3] = rightTop;

  std::vector<unsigned int>& indices = m_submeshes.front()->getIndices();
  indices.resize(6);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  load();
}

Mesh::Mesh(const AABB& box) : Mesh() {
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

  std::vector<Vertex>& vertices = m_submeshes.front()->getVertices();
  vertices.resize(8);

  vertices[0] = rightTopBack;
  vertices[1] = rightTopFront;

  vertices[2] = rightBottomBack;
  vertices[3] = rightBottomFront;

  vertices[4] = leftTopBack;
  vertices[5] = leftTopFront;

  vertices[6] = leftBottomBack;
  vertices[7] = leftBottomFront;

  std::vector<unsigned int>& indices = m_submeshes.front()->getIndices();
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

  load();
}

} // namespace Raz
