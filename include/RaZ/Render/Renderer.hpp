#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#include "RaZ/Render/Mesh.hpp"

namespace Raz {

enum class ShaderType {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void checkErrors();
  static void enable(unsigned int code);
  static void disable(unsigned int code);
  static void bindBuffer(unsigned int type, unsigned int index);
  static void unbindBuffer(unsigned int type) { bindBuffer(type, 0); }
  static void bindTexture(unsigned int type, unsigned int index);
  static void unbindTexture(unsigned int type) { bindTexture(type, 0); }
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static void generateBuffers(unsigned int count, unsigned int* index);
  static void deleteBuffers(unsigned int count, unsigned int* index);
  static unsigned int createShader(ShaderType type);
  static int getProgramInfo(unsigned int index, unsigned int infoType);
  static bool isProgramLinked(unsigned int index);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static int recoverUniformLocation(unsigned int programIndex, const char* uniformName);
  static void activateTexture(unsigned int index);

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;
};

} // namespace Raz

#endif //RAZ_RENDERER_HPP
