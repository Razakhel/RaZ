#include <GL/glew.h>

#include "RaZ/RaZ.hpp"

int main() {
  Raz::Window window(800, 600, "Test");

  Raz::VertexShader vertShader("../shaders/vert.glsl");
  Raz::FragmentShader fragShader("../shaders/frag.glsl");

  Raz::ShaderProgram program({ vertShader, fragShader });

  Raz::Mesh mesh("../meshes/queen.off");

  glBindVertexArray(mesh.getVao().getIndex());

  glBindBuffer(GL_ARRAY_BUFFER, mesh.getVbo().getIndex());
  glBufferData(GL_ARRAY_BUFFER, mesh.getVertexCount(), mesh.getVbo().getVertices().data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getEbo().getIndex());
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getEbo().getIndices().size(), mesh.getEbo().getIndices().data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Uncommenting this call will display scene in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!window.shouldClose()) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program.getIndex());
    glBindVertexArray(mesh.getVao().getIndex());
    glDrawElements(GL_TRIANGLES, mesh.getEbo().getIndices().size(), GL_UNSIGNED_INT, nullptr);

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}
