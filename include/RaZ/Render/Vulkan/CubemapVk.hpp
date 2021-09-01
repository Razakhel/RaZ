#pragma once

#ifndef RAZ_CUBEMAPVK_HPP
#define RAZ_CUBEMAPVK_HPP

#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/UniformBuffer.hpp"

#include <memory>

namespace Raz {

class FilePath;

/// Cubemap class representing an environment map surrounding the scene (also known as a skybox).
class Cubemap {
public:
  Cubemap();
  explicit Cubemap(const FilePath& rightTexturePath, const FilePath& leftTexturePath,
                   const FilePath& topTexturePath, const FilePath& bottomTexturePath,
                   const FilePath& frontTexturePath, const FilePath& backTexturePath)
    : Cubemap() { load(rightTexturePath, leftTexturePath, topTexturePath, bottomTexturePath, frontTexturePath, backTexturePath); }
  Cubemap(const Cubemap&) = delete;
  Cubemap(Cubemap&& cubemap) noexcept;

  unsigned int getIndex() const { return m_index; }
  const ShaderProgram& getProgram() const { return m_program; }

  /// Imports 6 textures and loads them onto the graphics card.
  /// \param rightTexturePath Path to the texture located on the right of the cube.
  /// \param leftTexturePath Path to the texture located on the left of the cube.
  /// \param topTexturePath Path to the texture located on the top of the cube.
  /// \param bottomTexturePath Path to the texture located on the bottom of the cube.
  /// \param frontTexturePath Path to the texture located on the front of the cube.
  /// \param backTexturePath Path to the texture located on the back of the cube.
  void load(const FilePath& rightTexturePath, const FilePath& leftTexturePath,
            const FilePath& topTexturePath, const FilePath& bottomTexturePath,
            const FilePath& frontTexturePath, const FilePath& backTexturePath) const;
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

  Cubemap& operator=(const Cubemap&) = delete;
  Cubemap& operator=(Cubemap&& cubemap) noexcept;

  ~Cubemap();

private:
  unsigned int m_index {};
  ShaderProgram m_program {};
  UniformBuffer m_viewProjUbo = UniformBuffer(sizeof(Mat4f), 1);
};

} // namespace Raz

#endif // RAZ_CUBEMAPVK_HPP
