#include "RaZ/RaZ.hpp"

int main() {
  const Raz::Window window(800, 600, "Test");

  const Raz::VertexShader vertShader("../shaders/vert.glsl");
  const Raz::FragmentShader fragShader("../shaders/frag.glsl");

  const Raz::ShaderProgram program({ vertShader, fragShader });

  const Raz::Mesh mesh("../assets/meshes/queen.off");

  /*const std::vector<float> vertices = { -1.f, -1.f, 0.f,
                                         1.f, -1.f, 0.f,
                                         1.f,  1.f, 0.f,
                                        -1.f,  1.f, 0.f };

  const std::vector<unsigned int> indices = { 0, 1, 3,
                                              1, 2, 3 };

  const Raz::Mesh mesh(vertices, indices);*/

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (window.run()) {
    program.use();
    mesh.draw();
  }

  return EXIT_SUCCESS;
}
