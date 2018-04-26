#include "RaZ/Render/Mesh.hpp"

namespace Raz {

MeshPtr Mesh::createTriangle(const Raz::Vec3f& leftPos, const Raz::Vec3f& topPos, const Raz::Vec3f& rightPos) {
  MeshPtr triangle = std::make_unique<Mesh>();

  // TODO: check if vertices are defined counterclockwise

  Vertex left {};
  left.position  = leftPos;
  left.texcoords = Vec2f({ 0.f, 0.f });

  Vertex top {};
  top.position  = topPos;
  top.texcoords = Vec2f({ 0.5f, 1.f });

  Vertex right {};
  right.position  = rightPos;
  right.texcoords = Vec2f({ 1.f, 0.f });

  // Computing normals
  left.normal  = (leftPos - topPos).cross(leftPos - rightPos).normalize();
  top.normal   = (topPos - rightPos).cross(topPos - leftPos).normalize();
  right.normal = (rightPos - leftPos).cross(rightPos - topPos).normalize();

  triangle->getSubmeshes().front()->getVbo().getVertices().resize(3);
  triangle->getSubmeshes().front()->getEbo().getIndices().resize(3);

  triangle->getSubmeshes().front()->getVbo().getVertices()[0] = left;
  triangle->getSubmeshes().front()->getVbo().getVertices()[1] = top;
  triangle->getSubmeshes().front()->getVbo().getVertices()[2] = right;

  triangle->getSubmeshes().front()->getEbo().getIndices()[0] = 1;
  triangle->getSubmeshes().front()->getEbo().getIndices()[1] = 0;
  triangle->getSubmeshes().front()->getEbo().getIndices()[2] = 2;

  triangle->load();

  return triangle;
}

MeshPtr Mesh::createQuad(const Vec3f& leftTopPos, const Vec3f& rightTopPos, const Vec3f& rightBottomPos, const Vec3f& leftBottomPos) {
  MeshPtr quad = std::make_unique<Mesh>();

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

  quad->getSubmeshes().front()->getVbo().getVertices().resize(4);
  quad->getSubmeshes().front()->getEbo().getIndices().resize(6);

  quad->getSubmeshes().front()->getVbo().getVertices()[0] = leftTop;
  quad->getSubmeshes().front()->getVbo().getVertices()[1] = leftBottom;
  quad->getSubmeshes().front()->getVbo().getVertices()[2] = rightBottom;
  quad->getSubmeshes().front()->getVbo().getVertices()[3] = rightTop;

  quad->getSubmeshes().front()->getEbo().getIndices()[0] = 0;
  quad->getSubmeshes().front()->getEbo().getIndices()[1] = 1;
  quad->getSubmeshes().front()->getEbo().getIndices()[2] = 2;

  quad->getSubmeshes().front()->getEbo().getIndices()[3] = 0;
  quad->getSubmeshes().front()->getEbo().getIndices()[4] = 2;
  quad->getSubmeshes().front()->getEbo().getIndices()[5] = 3;

  quad->load();

  return quad;
}

MeshPtr Mesh::createAABB(const Vec3f& rightTopFrontPos, const Vec3f& leftBottomBackPos) {
  MeshPtr aabb = std::make_unique<Mesh>();

  // TODO: texcoords should not exist for simple display cubes like a skybox

  Vertex rightTopBack {};
  rightTopBack.position  = Vec3f({ rightTopFrontPos[0], rightTopFrontPos[1], leftBottomBackPos[2] });
  rightTopBack.texcoords = Vec2f({ 0.f, 1.f });

  Vertex rightTopFront {};
  rightTopFront.position  = rightTopFrontPos;
  rightTopFront.texcoords = Vec2f({ 1.f, 1.f });

  Vertex rightBottomBack {};
  rightBottomBack.position  = Vec3f({ rightTopFrontPos[0], leftBottomBackPos[1], leftBottomBackPos[2] });
  rightBottomBack.texcoords = Vec2f({ 0.f, 0.f });

  Vertex rightBottomFront {};
  rightBottomFront.position  = Vec3f({ rightTopFrontPos[0], leftBottomBackPos[1], rightTopFrontPos[2] });
  rightBottomFront.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftTopBack {};
  leftTopBack.position  = Vec3f({ leftBottomBackPos[0], rightTopFrontPos[1], leftBottomBackPos[2] });
  leftTopBack.texcoords = Vec2f({ 1.f, 1.f });

  Vertex leftTopFront {};
  leftTopFront.position  = Vec3f({ leftBottomBackPos[0], rightTopFrontPos[1], rightTopFrontPos[2] });
  leftTopFront.texcoords = Vec2f({ 0.f, 1.f });

  Vertex leftBottomBack {};
  leftBottomBack.position  = leftBottomBackPos;
  leftBottomBack.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftBottomFront {};
  leftBottomFront.position  = Vec3f({ leftBottomBackPos[0], leftBottomBackPos[1], rightTopFrontPos[2] });
  leftBottomFront.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display cubes like a skybox
  // TODO: compute normals

  aabb->getSubmeshes().front()->getVbo().getVertices().resize(8);
  aabb->getSubmeshes().front()->getEbo().getIndices().resize(36);

  aabb->getSubmeshes().front()->getVbo().getVertices()[0] = rightTopBack;
  aabb->getSubmeshes().front()->getVbo().getVertices()[1] = rightTopFront;

  aabb->getSubmeshes().front()->getVbo().getVertices()[2] = rightBottomBack;
  aabb->getSubmeshes().front()->getVbo().getVertices()[3] = rightBottomFront;

  aabb->getSubmeshes().front()->getVbo().getVertices()[4] = leftTopBack;
  aabb->getSubmeshes().front()->getVbo().getVertices()[5] = leftTopFront;

  aabb->getSubmeshes().front()->getVbo().getVertices()[6] = leftBottomBack;
  aabb->getSubmeshes().front()->getVbo().getVertices()[7] = leftBottomFront;

  // Right face
  aabb->getSubmeshes().front()->getEbo().getIndices()[0]  = 1;
  aabb->getSubmeshes().front()->getEbo().getIndices()[1]  = 0;
  aabb->getSubmeshes().front()->getEbo().getIndices()[2]  = 2;

  aabb->getSubmeshes().front()->getEbo().getIndices()[3]  = 1;
  aabb->getSubmeshes().front()->getEbo().getIndices()[4]  = 2;
  aabb->getSubmeshes().front()->getEbo().getIndices()[5]  = 3;

  // Left face
  aabb->getSubmeshes().front()->getEbo().getIndices()[6]  = 4;
  aabb->getSubmeshes().front()->getEbo().getIndices()[7]  = 5;
  aabb->getSubmeshes().front()->getEbo().getIndices()[8]  = 7;

  aabb->getSubmeshes().front()->getEbo().getIndices()[9]  = 4;
  aabb->getSubmeshes().front()->getEbo().getIndices()[10] = 7;
  aabb->getSubmeshes().front()->getEbo().getIndices()[11] = 6;

  // Top face
  aabb->getSubmeshes().front()->getEbo().getIndices()[12] = 4;
  aabb->getSubmeshes().front()->getEbo().getIndices()[13] = 0;
  aabb->getSubmeshes().front()->getEbo().getIndices()[14] = 1;

  aabb->getSubmeshes().front()->getEbo().getIndices()[15] = 4;
  aabb->getSubmeshes().front()->getEbo().getIndices()[16] = 1;
  aabb->getSubmeshes().front()->getEbo().getIndices()[17] = 5;

  // Bottom face
  aabb->getSubmeshes().front()->getEbo().getIndices()[18] = 7;
  aabb->getSubmeshes().front()->getEbo().getIndices()[19] = 3;
  aabb->getSubmeshes().front()->getEbo().getIndices()[20] = 2;

  aabb->getSubmeshes().front()->getEbo().getIndices()[21] = 7;
  aabb->getSubmeshes().front()->getEbo().getIndices()[22] = 2;
  aabb->getSubmeshes().front()->getEbo().getIndices()[23] = 6;

  // Front face
  aabb->getSubmeshes().front()->getEbo().getIndices()[24] = 5;
  aabb->getSubmeshes().front()->getEbo().getIndices()[25] = 1;
  aabb->getSubmeshes().front()->getEbo().getIndices()[26] = 3;

  aabb->getSubmeshes().front()->getEbo().getIndices()[27] = 5;
  aabb->getSubmeshes().front()->getEbo().getIndices()[28] = 3;
  aabb->getSubmeshes().front()->getEbo().getIndices()[29] = 7;

  // Back face
  aabb->getSubmeshes().front()->getEbo().getIndices()[30] = 0;
  aabb->getSubmeshes().front()->getEbo().getIndices()[31] = 4;
  aabb->getSubmeshes().front()->getEbo().getIndices()[32] = 6;

  aabb->getSubmeshes().front()->getEbo().getIndices()[33] = 0;
  aabb->getSubmeshes().front()->getEbo().getIndices()[34] = 6;
  aabb->getSubmeshes().front()->getEbo().getIndices()[35] = 2;

  aabb->load();

  return aabb;
}

} // namespace Raz
