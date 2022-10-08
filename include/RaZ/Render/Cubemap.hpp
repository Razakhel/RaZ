#pragma once

#ifndef RAZ_CUBEMAP_HPP
#define RAZ_CUBEMAP_HPP

#include "RaZ/Data/OwnerValue.hpp"

#include <limits>

namespace Raz {

class Image;
class RenderShaderProgram;

/// Cubemap class representing an environment map surrounding the scene (also known as a skybox).
class Cubemap {
public:
  Cubemap();
  explicit Cubemap(const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back)
    : Cubemap() { load(right, left, top, bottom, front, back); }
  Cubemap(const Cubemap&) = delete;
  Cubemap(Cubemap&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }
  const RenderShaderProgram& getProgram() const;

  /// Applies the given images to the cubemap.
  /// \param right Image which will be on the right of the cube.
  /// \param left Image which will be on the left of the cube.
  /// \param top Image which will be on the top of the cube.
  /// \param bottom Image which will be on the bottom of the cube.
  /// \param front Image which will be on the front of the cube.
  /// \param back Image which will be on the back of the cube.
  void load(const Image& right, const Image& left, const Image& top, const Image& bottom, const Image& front, const Image& back) const;
  /// Binds the cubemap texture.
  void bind() const;
  /// Unbinds the cubemap texture.
  void unbind() const;
  /// Draws the cubemap around the scene.
  void draw() const;

  Cubemap& operator=(const Cubemap&) = delete;
  Cubemap& operator=(Cubemap&&) noexcept = default;

  ~Cubemap();

private:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};
};

} // namespace Raz

#endif // RAZ_CUBEMAP_HPP
