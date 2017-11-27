#include "RaZ/Render/Texture.hpp"

namespace Raz {

void Texture::load(const std::string& fileName) {
  m_image.read(fileName);

  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               m_image.getWidth(),
               m_image.getHeight(),
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               m_image.getData().data());
  glGenerateMipmap(GL_TEXTURE_2D);
  unbind();
}

} // namespace Raz
