#pragma once

#ifndef RAZ_CUBEMAP_HPP
#define RAZ_CUBEMAP_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/UniformBuffer.hpp"
#include "RaZ/Utils/Image.hpp"

#include <memory>
#include <string>

namespace Raz {

class Cubemap;
using CubemapPtr = std::unique_ptr<Cubemap>;

/// Cubemap class representing an environment map surrounding the scene (also known as a skybox).
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

  /// Imports 6 textures and loads them onto the graphics card.
  /// \param rightTexturePath Path to the texture located on the right of the cube.
  /// \param leftTexturePath Path to the texture located on the left of the cube.
  /// \param topTexturePath Path to the texture located on the top of the cube.
  /// \param bottomTexturePath Path to the texture located on the bottom of the cube.
  /// \param frontTexturePath Path to the texture located on the front of the cube.
  /// \param backTexturePath Path to the texture located on the back of the cube.
  void load(const std::string& rightTexturePath, const std::string& leftTexturePath,
            const std::string& topTexturePath, const std::string& bottomTexturePath,
            const std::string& frontTexturePath, const std::string& backTexturePath);
  /// Sends the view-projection matrix onto the graphics card.
  /// \param viewProjMat View-projection matrix to be sent.
  void sendViewProjectionMatrix(const Mat4f& viewProjMat) const { m_viewProjUbo.sendData(viewProjMat, 0); }
  /// Binds the cubemap texture.
  void bind() const;
  /// Unbinds the cubemap texture.
  void unbind() const;
  /// Draws the cubemap around the scene.
  /// \param camera Camera component from which will be taken the view & projection matrices.
  void draw(const Camera& camera) const;

private:
  unsigned int m_index {};
  ShaderProgram m_program {};
  UniformBuffer m_viewProjUbo = UniformBuffer(sizeof(Mat4f), 1);
};

} // namespace Raz

#endif // RAZ_CUBEMAP_HPP
