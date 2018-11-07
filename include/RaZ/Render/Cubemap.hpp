#pragma once

#ifndef RAZ_CUBEMAP_HPP
#define RAZ_CUBEMAP_HPP

#include <memory>
#include <string>

#include "glew/include/GL/glew.h"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Image.hpp"

namespace Raz {

class Cubemap;
using CubemapPtr = std::unique_ptr<Cubemap>;

class Cubemap {
public:
  Cubemap();
  explicit Cubemap(const std::string& rightTexturePath, const std::string& leftTexturePath,
                   const std::string& topTexturePath, const std::string& bottomTexturePath,
                   const std::string& frontTexturePath, const std::string& backTexturePath)
    : Cubemap() { load(rightTexturePath, leftTexturePath, topTexturePath, bottomTexturePath, frontTexturePath, backTexturePath); }

  const ShaderProgram& getProgram() const { return m_program; }

  template <typename... Args>
  static CubemapPtr create(Args&&... args) { return std::make_unique<Cubemap>(std::forward<Args>(args)...); }

  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_viewProjUbo.sendData(viewProjMat, 0); }

  void load(const std::string& rightTexturePath, const std::string& leftTexturePath,
            const std::string& topTexturePath, const std::string& bottomTexturePath,
            const std::string& frontTexturePath, const std::string& backTexturePath);
  void bind() const { glBindTexture(GL_TEXTURE_CUBE_MAP, m_index); }
  void unbind() const { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }
  void draw(const Camera& camera) const;

private:
  GLuint m_index {};
  ShaderProgram m_program;
  UniformBuffer m_viewProjUbo = UniformBuffer(sizeof(Mat4f), 1);
};

} // namespace Raz

#endif // RAZ_CUBEMAP_HPP
