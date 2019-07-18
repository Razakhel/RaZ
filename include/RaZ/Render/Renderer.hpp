#pragma once

#ifndef RAZ_RENDERER_HPP
#define RAZ_RENDERER_HPP

#include "RaZ/Render/Mesh.hpp"

namespace Raz {

enum class BufferType : unsigned int {
  UNIFORM_BUFFER = 35345 // GL_UNIFORM_BUFFER
};

enum class ShaderType : unsigned int {
  VERTEX   = 35633, // GL_VERTEX_SHADER
  FRAGMENT = 35632, // GL_FRAGMENT_SHADER
  GEOMETRY = 36313  // GL_GEOMETRY_SHADER
};

class Renderer {
public:
  Renderer() = delete;
  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) noexcept = delete;

  static void initialize();
  static bool isInitialized() { return s_isInitialized; }
  static void enable(unsigned int code);
  static void disable(unsigned int code);
  static void bindBuffer(unsigned int type, unsigned int index);
  static void unbindBuffer(unsigned int type) { bindBuffer(type, 0); }
  static void bindBufferBase(unsigned int type, unsigned int bindingIndex, unsigned int bufferIndex);
  static void bindTexture(unsigned int type, unsigned int index);
  static void unbindTexture(unsigned int type) { bindTexture(type, 0); }
  static void resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height);
  static void generateBuffers(unsigned int count, unsigned int* index);
  static void sendBufferSubData(BufferType type, ptrdiff_t offset, ptrdiff_t dataSize, const void* data);
  template <typename T> static void sendBufferSubData(BufferType type, ptrdiff_t offset, const T& data) { sendBufferSubData(type, offset, sizeof(T), &data); }
  static void deleteBuffers(unsigned int count, unsigned int* index);
  static unsigned int createProgram();
  static int getProgramInfo(unsigned int index, unsigned int infoType);
  static bool isProgramLinked(unsigned int index);
  static void linkProgram(unsigned int index);
  static void useProgram(unsigned int index);
  static void deleteProgram(unsigned int index);
  static unsigned int createShader(ShaderType type);
  static int getShaderInfo(unsigned int index, unsigned int infoType);
  static bool isShaderCompiled(unsigned int index);
  static void attachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void detachShader(unsigned int programIndex, unsigned int shaderIndex);
  static void deleteShader(unsigned int index);
  static int recoverUniformLocation(unsigned int programIndex, const char* uniformName);
  static void activateTexture(unsigned int index);
  static void checkErrors();

  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) noexcept = delete;

  ~Renderer() = delete;

private:
  static inline bool s_isInitialized = false;
};

} // namespace Raz

#endif //RAZ_RENDERER_HPP
