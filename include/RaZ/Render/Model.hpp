#pragma once

#ifndef RAZ_MODEL_HPP
#define RAZ_MODEL_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Material.hpp"

namespace Raz {

class Model;

using ModelPtr = std::unique_ptr<Model>;

class Model : public Transform {
public:
  explicit Model(MeshPtr mesh) : m_mesh{ std::move(mesh) } {}

  const MeshPtr& getMesh() const { return m_mesh; }

  static ModelPtr import(const std::string& filePath);
  void save(const std::string& filePath) const;
  void load(const ShaderProgram& program) const { m_mesh->load(&program); }
  void draw(const ShaderProgram& program) const { m_mesh->draw(&program); }

private:
  static ModelPtr importObj(std::ifstream& file, const std::string& filePath);
  static ModelPtr importOff(std::ifstream& file);

  void saveObj(std::ofstream& file, const std::string& filePath) const;

  MeshPtr m_mesh;
};

} // namespace Raz

#endif // RAZ_MODEL_HPP
