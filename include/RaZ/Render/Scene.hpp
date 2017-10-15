#pragma once

#ifndef RAZ_SCENE_HPP
#define RAZ_SCENE_HPP

#include "RaZ/Render/Mesh.hpp"

namespace Raz {

class Scene {
public:
  Scene() = default;
  Scene(const std::string& fileName) { import(fileName); }
  Scene(std::initializer_list<Mesh> meshes) : m_meshes(meshes) {}

  const std::vector<Mesh>& getMeshes() const { return m_meshes; }
  std::vector<Mesh>& getMeshes() { return m_meshes; }

  void import(const std::string& fileName);
  void render() const;

private:
  std::vector<Mesh> m_meshes;
};

} // namespace Raz

#endif // RAZ_SCENE_HPP
