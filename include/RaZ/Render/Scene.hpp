#pragma once

#ifndef RAZ_SCENE_HPP
#define RAZ_SCENE_HPP

#include "RaZ/Render/Mesh.hpp"

namespace Raz {

class Scene {
public:
  Scene() = default;
  Scene(const std::string& fileName) { import(fileName); }
  Scene(std::vector<MeshPtr>&& meshes) : m_meshes(std::move(meshes)) {}

  const std::vector<MeshPtr>& getMeshes() const { return m_meshes; }
  std::vector<MeshPtr>& getMeshes() { return m_meshes; }

  void import(const std::string& fileName);
  void render() const;

private:
  std::vector<MeshPtr> m_meshes;
};

} // namespace Raz

#endif // RAZ_SCENE_HPP
