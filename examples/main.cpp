#include "RaZ/RaZ.hpp"

int main() {
  const Raz::Window window(800, 600, "Test");

  Raz::Scene scene;

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");
  const Raz::Material defaultMaterial(vertShader, fragShader);

  //const Raz::MeshPtr cubeMesh = std::make_shared<Raz::Mesh>("../assets/meshes/cube.obj");
  const Raz::MeshPtr queenMesh = std::make_shared<Raz::Mesh>("../assets/meshes/queen.off");
  //Raz::ModelPtr cubeModel = std::make_unique<Raz::Model>(cubeMesh);
  Raz::ModelPtr queenModel = std::make_unique<Raz::Model>(queenMesh);
  //cubeModel->setMaterial(defaultMaterial);
  queenModel->setMaterial(defaultMaterial);

  //auto cube = cubeModel.get();
  auto queen = queenModel.get();

  //scene.addModel(std::move(cubeModel));
  scene.addModel(std::move(queenModel));

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (window.run()) {
    //cube->translate(0.04f, 0.002f, 0.f);
    //cube->rotate(5.f, 0.f, 1.f, 0.f);

    //queen->translate(0.02f, 0.f, 0.f);
    queen->rotate(5.f, 0.f, 0.5f, 0.f);
    queen->scale(1.0025f, 1.0025f, 1.0025f);

    scene.render();
  }

  return EXIT_SUCCESS;
}
