#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Render/Mesh.hpp"

namespace Raz {

void Mesh::load(const VertexShader& vertShader, const FragmentShader& fragShader) {
  m_program.attachShaders({ vertShader, fragShader });

  m_vao.bind();
  getEbo().bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(getEbo().getVerticesIndices().front()) * getEbo().getVerticesIndices().size(),
               getEbo().getVerticesIndices().data(),
               GL_STATIC_DRAW);

  m_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(m_vbo.getVertices().front()) * m_vbo.getVertices().size(),
               m_vbo.getVertices().data(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  m_vao.unbind();
  getEbo().unbind();
  m_vbo.unbind();

  /*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  texture.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), , 0, GL_RGB, GL_UNSIGNED_BYTE, img.getData().data());*/
}

void Mesh::draw() const {
  m_program.use();

  //m_texture.bind();
  m_vao.bind();

  glDrawElements(GL_TRIANGLES, getFaceCount(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::translate(float x, float y, float z) {
  const Mat4f mat({{ 1.f, 0.f, 0.f,   x },
                   { 0.f, 1.f, 0.f,   y },
                   { 0.f, 0.f, 1.f,   z },
                   { 0.f, 0.f, 0.f, 1.f }});

  const GLint location = glGetUniformLocation(m_program.getIndex(), "uniTransform");
  glUniformMatrix4fv(location, 1, GL_TRUE, mat.getData().data());
}

} // namespace Raz
