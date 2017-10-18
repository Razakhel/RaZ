#include "RaZ/Render/Scene.hpp"

namespace Raz {

void Scene::render() const {
  for (const auto& model : m_models)
    model->draw();
}

} // namespace Raz
