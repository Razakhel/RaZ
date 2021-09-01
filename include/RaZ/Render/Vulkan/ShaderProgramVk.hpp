#pragma once

#ifndef RAZ_SHADERPROGRAMVK_HPP
#define RAZ_SHADERPROGRAMVK_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/UniformBuffer.hpp"

#include <optional>
#include <string>
#include <unordered_map>

namespace Raz {

class UniformBuffer;

/// ShaderProgram class, holding shaders & handling data transmission to the graphics card with uniforms.
class ShaderProgram {
public:
  ShaderProgram() = default;
  ShaderProgram(VertexShader&& vertShader, FragmentShader&& fragShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader)); }
  ShaderProgram(VertexShader&& vertShader, FragmentShader&& fragShader, GeometryShader&& geomShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(geomShader), std::move(fragShader)); }
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&& program) noexcept;

  void setVertexShader(VertexShader&& vertShader);
  void setGeometryShader(GeometryShader&& geomShader);
  void setFragmentShader(FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, GeometryShader&& geomShader, FragmentShader&& fragShader);

  /// Loads all the shaders contained by the program.
  void loadShaders();
  /// Reloads all the shaders contained by the program.
  void updateShaders() { loadShaders(); }
  void use() const {}
  /// Destroys the vertex shader.
  void destroyVertexShader();
  /// Destroys the geometry shader (if any).
  void destroyGeometryShader();
  /// Destroys the fragment shader.
  void destroyFragmentShader();

  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&& program) noexcept;

  ~ShaderProgram();

private:
  VertexShader m_vertShader {};
  std::optional<GeometryShader> m_geomShader {};
  FragmentShader m_fragShader {};

  std::vector<UniformBuffer> m_uniformBuffers {};
};

} // namespace Raz

#endif // RAZ_SHADERPROGRAMVK_HPP
