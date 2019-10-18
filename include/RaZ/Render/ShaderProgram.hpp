#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Renderer.hpp"
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
  /// \note Location will be -1 if the name is incorrect or if the uniform isn't used in the shader(s) (will be optimized out).
  /// \warning The location will be recovered faster if previously registered with createUniform(). For reusability, create it first.
  /// \param uniformName Name of the uniform to recover the location from.
  /// \return Location (ID) of the uniform.
  int recoverUniformLocation(const std::string& uniformName) const;
  /// Sends an integer as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Integer to be sent.
  void sendUniform(int uniformIndex, int value) const { Renderer::sendUniform(uniformIndex, value); }
  /// Sends an unsigned integer as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Unsigned integer to be sent.
  void sendUniform(int uniformIndex, unsigned int value) const { Renderer::sendUniform(uniformIndex, value); }
  /// Sends a floating-point value as uniform.
  /// \param uniformIndex Index of the uniform to send the data to.
  /// \param value Floating-point value to be sent.
  void sendUniform(int uniformIndex, float value) const { Renderer::sendUniform(uniformIndex, value); }
  /// Sends a floating-point 2D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int uniformIndex, const Vec2f& vec) const { Renderer::sendUniformVector2(uniformIndex, vec.getDataPtr()); }
  /// Sends a floating-point 3D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int uniformIndex, const Vec3f& vec) const { Renderer::sendUniformVector3(uniformIndex, vec.getDataPtr()); }
  /// Sends a floating-point 4D vector as uniform.
  /// \param uniformIndex Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int uniformIndex, const Vec4f& vec) const { Renderer::sendUniformVector4(uniformIndex, vec.getDataPtr()); }
  /// Sends a floating-point 2x2 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int uniformIndex, const Mat2f& mat) const { Renderer::sendUniformMatrix2x2(uniformIndex, mat.getDataPtr()); }
  /// Sends a floating-point 3x3 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int uniformIndex, const Mat3f& mat) const { Renderer::sendUniformMatrix3x3(uniformIndex, mat.getDataPtr()); }
  /// Sends a floating-point 4x4 matrix as uniform.
  /// \param uniformIndex Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int uniformIndex, const Mat4f& mat) const { Renderer::sendUniformMatrix4x4(uniformIndex, mat.getDataPtr()); }
  /// Sends an integer as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param value Integer to be sent.
  void sendUniform(const std::string& uniformName, int value) const { sendUniform(recoverUniformLocation(uniformName), value); }
  /// Sends an unsigned integer as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param value Unsigned integer to be sent.
  void sendUniform(const std::string& uniformName, unsigned int value) const { sendUniform(recoverUniformLocation(uniformName), value); }
  /// Sends a floating-point value as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param value Floating-point value to be sent.
  void sendUniform(const std::string& uniformName, float value) const { sendUniform(recoverUniformLocation(uniformName), value); }
  /// Sends a floating-point 2D vector as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& uniformName, const Vec2f& vec) const { sendUniform(recoverUniformLocation(uniformName), vec); }
  /// Sends a floating-point 3D vector as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& uniformName, const Vec3f& vec) const { sendUniform(recoverUniformLocation(uniformName), vec); }
  /// Sends a floating-point 4D vector as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& uniformName, const Vec4f& vec) const { sendUniform(recoverUniformLocation(uniformName), vec); }
  /// Sends a floating-point 2x2 matrix as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& uniformName, const Mat2f& mat) const { sendUniform(recoverUniformLocation(uniformName), mat); }
  /// Sends a floating-point 3x3 matrix as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& uniformName, const Mat3f& mat) const { sendUniform(recoverUniformLocation(uniformName), mat); }
  /// Sends a floating-point 4x4 matrix as uniform, which location will be retrieved with its name.
  /// \param uniformName Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& uniformName, const Mat4f& mat) const { sendUniform(recoverUniformLocation(uniformName), mat); }
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

#endif // RAZ_SHADERPROGRAM_HPP
