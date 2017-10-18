#include "RaZ/RaZ.hpp"

int main() {
  const Raz::Window window(800, 600, "Test");

  Raz::Scene scene;

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");
  const Raz::Material defaultMaterial(vertShader, fragShader);

  const Raz::MeshPtr cube = std::make_shared<Raz::Mesh>("../assets/meshes/cube.obj");
  Raz::ModelPtr cubeModel = std::make_unique<Raz::Model>(cube);
  cubeModel->setMaterial(defaultMaterial);

  scene.addModel(std::move(cubeModel));

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (window.run()) {
    scene.getModels().front()->translate(0.0005f, 0.0005f, 0.f);

    scene.render();
  }

  return EXIT_SUCCESS;
}
