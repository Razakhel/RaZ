#include "RaZ/Render/Cubemap.hpp"
#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

Cubemap::Cubemap() {
  glGenTextures(1, &m_index);

  const std::string vertSource = R"(
    #version 330 core

    layout (location = 0) in vec3 vertPosition;

    layout (std140) uniform uboCubemapMatrix {
      mat4 viewProjMat;
    };

    out vec3 fragTexcoords;

    void main() {
      fragTexcoords = vertPosition;

      vec4 pos = viewProjMat * vec4(vertPosition, 1.0);
      gl_Position = pos.xyww;
    }
  )";

  const std::string fragSource = R"(
    #version 330 core

    in vec3 fragTexcoords;

    uniform samplerCube uniSkybox;

    layout (location = 0) out vec4 fragColor;

    void main() {
      fragColor = texture(uniSkybox, fragTexcoords);
    }
  )";

  m_program.setVertexShader(VertexShader::loadFromSource(vertSource));
  m_program.setFragmentShader(FragmentShader::loadFromSource(fragSource));
  m_program.link();

  m_program.sendUniform("uniSkybox", 0);

  m_viewProjUbo.bindUniformBlock(m_program, "uboCubemapMatrix", 1);
  m_viewProjUbo.bindBufferBase(1);
}

void Cubemap::load(const std::string& rightTexturePath, const std::string& leftTexturePath,
                   const std::string& topTexturePath, const std::string& bottomTexturePath,
                   const std::string& frontTexturePath, const std::string& backTexturePath) {
  bind();

  Image img(rightTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  img.read(leftTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  img.read(topTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  img.read(bottomTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  img.read(frontTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  img.read(backTexturePath, true);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
               0,
               static_cast<int>(img.getColorspace()),
               static_cast<int>(img.getWidth()),
               static_cast<int>(img.getHeight()),
               0,
               static_cast<unsigned int>(img.getColorspace()),
               GL_UNSIGNED_BYTE,
               img.getDataPtr());

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  unbind();
}

void Cubemap::draw(const CameraPtr& camera) const {
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_FRONT);

  m_program.use();

  Texture::activate(0);
  bind();

  m_viewProjUbo.bind();
  sendViewProjectionMatrix(camera->getProjectionMatrix() * Mat4f(Mat3f(camera->getViewMatrix())));

  Mesh::drawCube();

  glCullFace(GL_BACK);
  glDepthFunc(GL_LESS);
}

} // namespace Raz
