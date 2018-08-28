#include "RaZ/Render/Mesh.hpp"

namespace Raz {

Mesh::Mesh(const Triangle& triangle) : Mesh() {
  // TODO: check if vertices are defined counterclockwise

  Vertex firstVert {};
  firstVert.position  = triangle.firstPos;
  firstVert.texcoords = Vec2f({ 0.f, 0.f });

  Vertex secondVert {};
  secondVert.position  = triangle.secondPos;
  secondVert.texcoords = Vec2f({ 0.5f, 1.f });

  Vertex thirdVert {};
  thirdVert.position  = triangle.thirdPos;
  thirdVert.texcoords = Vec2f({ 1.f, 0.f });

  // Computing normals
  firstVert.normal  = (triangle.firstPos - triangle.secondPos).cross(triangle.firstPos - triangle.thirdPos).normalize();
  secondVert.normal = (triangle.secondPos - triangle.thirdPos).cross(triangle.secondPos - triangle.firstPos).normalize();
  thirdVert.normal  = (triangle.thirdPos - triangle.firstPos).cross(triangle.thirdPos - triangle.secondPos).normalize();

  std::vector<Vertex>& vertices = m_submeshes.front()->getVbo().getVertices();
  std::vector<unsigned int>& indices = m_submeshes.front()->getEbo().getIndices();

  vertices.resize(3);
  indices.resize(3);

  vertices[0] = firstVert;
  vertices[1] = secondVert;
  vertices[2] = thirdVert;

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  load();
}

Mesh::Mesh(const Quad& quad) : Mesh() {
  Vertex leftTop {};
  leftTop.position  = quad.leftTopPos;
  leftTop.texcoords = Vec2f({ 0.f, 1.f });

  Vertex rightTop {};
  rightTop.position  = quad.rightTopPos;
  rightTop.texcoords = Vec2f({ 1.f, 1.f });

  Vertex rightBottom {};
  rightBottom.position  = quad.rightBottomPos;
  rightBottom.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftBottom {};
  leftBottom.position  = quad.leftBottomPos;
  leftBottom.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display quads like a framebuffer
  leftTop.normal     = (quad.leftTopPos - quad.rightTopPos).cross(quad.leftTopPos - quad.leftBottomPos).normalize();
  rightTop.normal    = (quad.rightTopPos - quad.rightBottomPos).cross(quad.rightTopPos - quad.leftTopPos).normalize();
  rightBottom.normal = (quad.rightBottomPos - quad.leftBottomPos).cross(quad.rightBottomPos - quad.rightTopPos).normalize();
  leftBottom.normal  = (quad.leftBottomPos - quad.leftTopPos).cross(quad.leftBottomPos - quad.rightBottomPos).normalize();

  std::vector<Vertex>& vertices = m_submeshes.front()->getVbo().getVertices();
  std::vector<unsigned int>& indices = m_submeshes.front()->getEbo().getIndices();

  vertices.resize(4);
  indices.resize(6);

  vertices[0] = leftTop;
  vertices[1] = leftBottom;
  vertices[2] = rightBottom;
  vertices[3] = rightTop;

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  load();
}

Mesh::Mesh(const AABB& box) : Mesh() {
  const float rightPos  = box.rightTopFrontPos[0];
  const float leftPos   = box.leftBottomBackPos[0];
  const float topPos    = box.rightTopFrontPos[1];
  const float bottomPos = box.leftBottomBackPos[1];
  const float frontPos  = box.rightTopFrontPos[2];
  const float backPos   = box.leftBottomBackPos[2];

  // TODO: texcoords should not exist for simple display cubes like a skybox

  Vertex rightTopBack {};
  rightTopBack.position  = Vec3f({ rightPos, topPos, backPos });
  rightTopBack.texcoords = Vec2f({ 0.f, 1.f });

  Vertex rightTopFront {};
  rightTopFront.position  = box.rightTopFrontPos;
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
  leftBottomBack.position  = box.leftBottomBackPos;
  leftBottomBack.texcoords = Vec2f({ 1.f, 0.f });

  Vertex leftBottomFront {};
  leftBottomFront.position  = Vec3f({ leftPos, bottomPos, frontPos });
  leftBottomFront.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display cubes like a skybox
  // TODO: compute normals

  std::vector<Vertex>& vertices = m_submeshes.front()->getVbo().getVertices();
  std::vector<unsigned int>& indices = m_submeshes.front()->getEbo().getIndices();

  vertices.resize(8);
  indices.resize(36);

  vertices[0] = rightTopBack;
  vertices[1] = rightTopFront;

  vertices[2] = rightBottomBack;
  vertices[3] = rightBottomFront;

  vertices[4] = leftTopBack;
  vertices[5] = leftTopFront;

  vertices[6] = leftBottomBack;
  vertices[7] = leftBottomFront;

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
