#include "RaZ/RaZ.hpp"

int main() {
  const Raz::Window window(800, 600, "Test");

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");

  const Raz::ShaderProgram program({ vertShader, fragShader });

  const Raz::Scene scene("../assets/meshes/cube.obj");

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (window.run()) {
    program.use();
    scene.render();
  }

  return EXIT_SUCCESS;
}
