#include "RaZ/Render/Mesh.hpp"

namespace Raz {

Mesh::Mesh(const Vec3f& leftPos, const Vec3f& topPos, const Vec3f& rightPos) {
  // TODO: check if vertices are defined counterclockwise

  Vertex left {};
  left.positions = leftPos;
  left.texcoords = Vec2f({ 0.f, 0.f });

  Vertex top {};
  top.positions = topPos;
  top.texcoords = Vec2f({ 0.5f, 1.f });

  Vertex right {};
  right.positions = rightPos;
  right.texcoords = Vec2f({ 1.f, 0.f });

  // Computing normals
  left.normals = (leftPos - topPos).cross(leftPos - rightPos).normalize();
  top.normals = (topPos - rightPos).cross(topPos - leftPos).normalize();
  right.normals = (rightPos - leftPos).cross(rightPos - topPos).normalize();

  m_submeshes.emplace_back(std::make_unique<Submesh>());

  m_submeshes.front()->getVbo().getVertices().resize(3);
  m_submeshes.front()->getEbo().getIndices().resize(3);

  m_submeshes.front()->getVbo().getVertices()[0] = left;
  m_submeshes.front()->getVbo().getVertices()[1] = top;
  m_submeshes.front()->getVbo().getVertices()[2] = right;

  m_submeshes.front()->getEbo().getIndices()[0] = 1;
  m_submeshes.front()->getEbo().getIndices()[1] = 0;
  m_submeshes.front()->getEbo().getIndices()[2] = 2;
}

Mesh::Mesh(const Vec3f& topLeftPos, const Vec3f& topRightPos, const Vec3f& bottomRightPos, const Vec3f& bottomLeftPos) {
  // TODO: check if vertices are defined counterclockwise

  Vertex topLeft {};
  topLeft.positions = topLeftPos;
  topLeft.texcoords = Vec2f({ 0.f, 1.f });

  Vertex topRight {};
  topRight.positions = topRightPos;
  topRight.texcoords = Vec2f({ 1.f, 1.f });

  Vertex bottomRight {};
  bottomRight.positions = bottomRightPos;
  bottomRight.texcoords = Vec2f({ 1.f, 0.f });

  Vertex bottomLeft {};
  bottomLeft.positions = bottomLeftPos;
  bottomLeft.texcoords = Vec2f({ 0.f, 0.f });

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display quads like a framebuffer
  topLeft.normals = (topLeftPos - topRightPos).cross(topLeftPos - bottomLeftPos).normalize();
  topRight.normals = (topRightPos - bottomRightPos).cross(topRightPos - topLeftPos).normalize();
  bottomRight.normals = (bottomRightPos - bottomLeftPos).cross(bottomRightPos - topRightPos).normalize();
  bottomLeft.normals = (bottomLeftPos - topLeftPos).cross(bottomLeftPos - bottomRightPos).normalize();

  m_submeshes.emplace_back(std::make_unique<Submesh>());

  m_submeshes.front()->getVbo().getVertices().resize(4);
  m_submeshes.front()->getEbo().getIndices().resize(6);

  m_submeshes.front()->getVbo().getVertices()[0] = topLeft;
  m_submeshes.front()->getVbo().getVertices()[1] = bottomLeft;
  m_submeshes.front()->getVbo().getVertices()[2] = bottomRight;
  m_submeshes.front()->getVbo().getVertices()[3] = topRight;

  m_submeshes.front()->getEbo().getIndices()[0] = 0;
  m_submeshes.front()->getEbo().getIndices()[1] = 1;
  m_submeshes.front()->getEbo().getIndices()[2] = 2;

  m_submeshes.front()->getEbo().getIndices()[3] = 0;
  m_submeshes.front()->getEbo().getIndices()[4] = 2;
  m_submeshes.front()->getEbo().getIndices()[5] = 3;
}

} // namespace Raz
