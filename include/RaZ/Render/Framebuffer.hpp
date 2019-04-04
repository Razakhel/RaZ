#pragma once

#ifndef RAZ_FRAMEBUFFER_HPP
#define RAZ_FRAMEBUFFER_HPP

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

namespace Raz {

class Framebuffer {
public:
  Framebuffer();
  explicit Framebuffer(ShaderProgram& program);
  Framebuffer(unsigned int width, unsigned int height, ShaderProgram& program);

  const TexturePtr& getDepthBuffer() const { return m_depthBuffer; }
  const TexturePtr& getNormalBuffer() const { return m_normalBuffer; }
  const TexturePtr& getColorBuffer() const { return m_colorBuffer; }

  void initBuffers(const ShaderProgram& program) const;
  void bind() const;
  void unbind() const;
  void display(const ShaderProgram& program) const;
  void resize(unsigned int width, unsigned int height);
  void assignVertexShader(ShaderProgram& program) const;

  ~Framebuffer();

private:
  unsigned int m_index {};
  TexturePtr m_depthBuffer {};
  TexturePtr m_normalBuffer {};
  TexturePtr m_colorBuffer {};
};

} // namespace Raz

#endif // RAZ_FRAMEBUFFER_HPP
