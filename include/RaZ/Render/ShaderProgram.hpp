#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#include <string>
#include <unordered_map>

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"

namespace Raz {

class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram(VertexShader vertShader, FragmentShader fragShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader)); }
  ShaderProgram(VertexShader vertShader, FragmentShader fragShader, GeometryShader geomShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader), std::move(geomShader)); }

  unsigned int getIndex() const { return m_index; }

  void setVertexShader(VertexShader vertShader);
  void setFragmentShader(FragmentShader fragShader);
  void setGeometryShader(GeometryShader geomShader);
  void setShaders(VertexShader vertShader, FragmentShader fragShader);
  void setShaders(VertexShader vertShader, FragmentShader fragShader, GeometryShader geomShader);

  void loadShaders() const;
  void compileShaders() const;
  void link() const;
  void use() const;
  void updateShaders() const;
  void createUniform(const std::string& uniformName);
  int recoverUniformLocation(const std::string& uniformName) const;
  template <typename T> void sendUniform(int uniformIndex, T value) const;
  template <typename T, std::size_t Size> void sendUniform(int uniformIndex, const Vector<T, Size>& vec) const;
  template <typename T, std::size_t W, std::size_t H> void sendUniform(int uniformIndex, const Matrix<T, W, H>& mat) const;
  template <typename T> void sendUniform(const std::string& uniformName, T value) const;
  void destroyVertexShader();
  void destroyFragmentShader();
  void destroyGeometryShader();

private:
  unsigned int m_index;

  VertexShader m_vertShader {};
  FragmentShader m_fragShader {};
  std::unique_ptr<GeometryShader> m_geomShader {};

  std::unordered_map<std::string, int> m_uniforms;
};

} // namespace Raz

#include "RaZ/Render/ShaderProgram.inl"

#endif // RAZ_SHADERPROGRAM_HPP
