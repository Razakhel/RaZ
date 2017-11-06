#include <chrono>
#include <iostream>

#include "RaZ/RaZ.hpp"

int main() {
  const Raz::Window window(800, 600, "Test");

  Raz::Scene scene;

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");

  Raz::Material defaultMaterial(vertShader, fragShader);
  const Raz::TexturePtr defaultTexture = std::make_shared<Raz::Texture>("../assets/textures/lena.png");
  defaultMaterial.setTexture(defaultTexture);

  const auto startTime = std::chrono::system_clock::now();

  const Raz::MeshPtr mesh = std::make_shared<Raz::Mesh>("../assets/meshes/cube.obj");

  const auto endTime = std::chrono::system_clock::now();

  std::cout << "Mesh loading duration: "
            << std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count()
            << " seconds." << std::endl;

  Raz::ModelPtr model = std::make_unique<Raz::Model>(mesh);
  model->setMaterial(defaultMaterial);

  auto cube = model.get();

  scene.addModel(std::move(model));

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glEnable(GL_CULL_FACE);

  while (window.run()) {
    //cube->translate(0.02f, 0.f, 0.f);
    cube->rotate(15.f, 0.01f, 0.01f, 0.01f);
    //cube->scale(1.0025f, 1.0025f, 1.0025f);

    scene.render();
  }

  return EXIT_SUCCESS;
}
