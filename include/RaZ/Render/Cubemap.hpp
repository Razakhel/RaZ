#pragma once

#ifndef RAZ_CUBEMAP_HPP
#define RAZ_CUBEMAP_HPP

#include "RaZ/Render/ShaderProgram.hpp"

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
  const RenderShaderProgram& getProgram() const;

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
  /// Binds the cubemap texture.
  void bind() const;
  /// Unbinds the cubemap texture.
  void unbind() const;
  /// Draws the cubemap around the scene.
  void draw() const;

  Cubemap& operator=(const Cubemap&) = delete;
  Cubemap& operator=(Cubemap&& cubemap) noexcept;

  ~Cubemap();

private:
  unsigned int m_index {};
};

} // namespace Raz

#endif // RAZ_CUBEMAP_HPP
