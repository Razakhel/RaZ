#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"

#include <string>
#include <unordered_map>

namespace Raz {

/// ShaderProgram class, holding shaders & handling data transmission to the graphics card with uniforms.
class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram(VertexShader vertShader, FragmentShader fragShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader)); }
  ShaderProgram(VertexShader vertShader, FragmentShader fragShader, GeometryShader geomShader)
    : ShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader), std::move(geomShader)); }
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&& program) noexcept;

  unsigned int getIndex() const { return m_index; }

  void setVertexShader(VertexShader&& vertShader);
  void setFragmentShader(FragmentShader&& fragShader);
  void setGeometryShader(GeometryShader&& geomShader);
  void setShaders(VertexShader&& vertShader, FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, FragmentShader&& fragShader, GeometryShader&& geomShader);

  /// Loads all the shaders contained by the program.
  void loadShaders() const;
  /// Compiles all the shaders contained by the program.
  void compileShaders() const;
  /// Links the program to the graphics card.
  void link() const;
  /// Checks if the program has been successfully linked.
  /// \return True if the program is linked, false otherwise.
  bool isLinked() const;
  /// Tells the graphics card to use the program.
  void use() const;
  /// Loads & compiles all the shaders contained by the program, then links & uses it.
  void updateShaders() const;
  /// Creates a uniform & registers its location (ID) used by the program.
  /// \param uniformName Name of the uniform to be created.
  void createUniform(const std::string& uniformName);
  /// Gets the uniform's location (ID) corresponding to the given name.
  /// \param uniformName Name of the uniform to recover the location from.
  /// \return Location (ID) of the uniform.
  /// \note Location will be -1 if the name is incorrect or if the uniform isn't used in the shader(s) (will be optimized out).
  /// \warning The location will be recovered faster if previously registered with createUniform(). For reusability, create it first.
  int recoverUniformLocation(const std::string& uniformName) const;
  /// Sends data as uniform.
  /// \tparam T Type of the data.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Data to be sent as uniform.
  template <typename T> void sendUniform(int uniformIndex, T value) const;
  /// Sends a vector as uniform.
  /// \tparam T Type of the vector's data.
  /// \tparam Size Size of the vector.
  /// \param uniformIndex Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent as uniform.
  template <typename T, std::size_t Size> void sendUniform(int uniformIndex, const Vector<T, Size>& vec) const;
  /// Sends a matrix as uniform.
  /// \tparam T Type of the matrix's data.
  /// \tparam W Width of the matrix.
  /// \tparam H Height of the matrix.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent as uniform.
  template <typename T, std::size_t W, std::size_t H> void sendUniform(int uniformIndex, const Matrix<T, W, H>& mat) const;
  /// Sends data as uniform, which location will be retrieved with its name.
  /// \tparam T Type of the data.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param value Data to be sent as uniform.
  template <typename T> void sendUniform(const std::string& uniformName, T value) const;
  /// Destroys the vertex shader, detaching it from the program & deleting it.
  void destroyVertexShader();
  /// Destroys the fragment shader, detaching it from the program & deleting it.
  void destroyFragmentShader();
  /// Destroys the geometry shader (if any), detaching it from the program & deleting it.
  void destroyGeometryShader();

  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&& program) noexcept;

  ~ShaderProgram();

private:
  unsigned int m_index {};

  VertexShader m_vertShader {};
  FragmentShader m_fragShader {};
  std::unique_ptr<GeometryShader> m_geomShader {};

  std::unordered_map<std::string, int> m_uniforms {};
};

} // namespace Raz

#include "RaZ/Render/ShaderProgram.inl"

#endif // RAZ_SHADERPROGRAM_HPP
