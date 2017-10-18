#pragma once

#ifndef RAZ_SCENE_HPP
#define RAZ_SCENE_HPP

#include "RaZ/Render/Model.hpp"

namespace Raz {

class Scene {
public:
  Scene() = default;
  Scene(std::vector<ModelPtr>&& models) : m_models(std::move(models)) {}

  const std::vector<ModelPtr>& getModels() const { return m_models; }
  std::vector<ModelPtr>& getModels() { return m_models; }

  void addModel(ModelPtr model) { m_models.push_back(std::move(model)); }
  void render() const;

private:
  std::vector<ModelPtr> m_models;
};

} // namespace Raz

#endif // RAZ_SCENE_HPP
