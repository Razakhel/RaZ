#pragma once

#ifndef RAZ_SHADERPROGRAM_HPP
#define RAZ_SHADERPROGRAM_HPP

#include "RaZ/Data/OwnerValue.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/Texture.hpp"

#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Raz {

/// ShaderProgram class, holding shaders & handling data transmission to the graphics card with uniforms.
class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) noexcept = default;

  unsigned int getIndex() const { return m_index; }
  /// Checks if an attribute has been set with the given uniform name.
  /// \param uniformName Uniform name to be checked.
  /// \return True if an attribute exists with the given name, false otherwise.
  bool hasAttribute(const std::string& uniformName) const noexcept;
  /// Checks if an attribute has been set with the given uniform name and type.
  /// \tparam T Type to be checked.
  /// \param uniformName Uniform name to be checked.
  /// \return True if an attribute exists with the given name and type, false otherwise.
  template <typename T> bool hasAttribute(const std::string& uniformName) const noexcept;
  std::size_t getAttributeCount() const noexcept { return m_attributes.size(); }
  /// Fetches an attribute's value from its uniform name.
  /// \tparam T Type of the attribute to get. It MUST be the same type the uniform has been set with.
  /// \param uniformName Uniform name of the attribute to get.
  /// \return Attribute found.
  template <typename T> const T& getAttribute(const std::string& uniformName) const noexcept;
  /// Checks if there is a texture entry with the given texture.
  /// \param texture Texture to find.
  /// \return True if an entry has been found, false otherwise.
  bool hasTexture(const Texture& texture) const noexcept;
  /// Checks if there is a texture entry with the given uniform name.
  /// \param uniformName Uniform name to find.
  /// \return True if an entry has been found, false otherwise.
  bool hasTexture(const std::string& uniformName) const noexcept;
  const std::vector<std::pair<TexturePtr, std::string>>& getTextures() const noexcept { return m_textures; }
  std::size_t getTextureCount() const noexcept { return m_textures.size(); }
  const Texture& getTexture(std::size_t index) const noexcept { return *m_textures[index].first; }
  const Texture& getTexture(const std::string& uniformName) const;

  /// Sets an attribute to be sent to the shaders. If the uniform name already exists, replaces the attribute's value.
  /// \tparam T Type of the attribute to set. Must be a type handled by ShaderProgram::sendUniform().
  /// \param attribVal Attribute to set.
  /// \param uniformName Uniform name of the attribute to set.
  template <typename T> void setAttribute(T&& attribVal, std::string uniformName);
  /// Sets a texture to be bound to the shaders. If the uniform name already exists, replaces the texture.
  /// \param texture Texture to set.
  /// \param uniformName Uniform name to bind the texture to.
  void setTexture(TexturePtr texture, std::string uniformName);

  /// Loads all the shaders contained by the program.
  virtual void loadShaders() const = 0;
  /// Compiles all the shaders contained by the program.
  virtual void compileShaders() const = 0;
  /// Links the program to the graphics card.
  /// \note Linking a program resets all its attributes' values and textures' bindings; you may want to call sendAttributes() & initTextures() afterwards.
  void link() const;
  /// Checks if the program has been successfully linked.
  /// \return True if the program is linked, false otherwise.
  bool isLinked() const;
  /// Marks the program as used.
  void use() const;
  /// Checks if the program is currently defined as used.
  bool isUsed() const;
  /// Loads & compiles all the shaders contained by the program, links it and initializes its attributes & textures.
  void updateShaders() const;
  /// Sends the program's attributes as uniforms.
  void sendAttributes() const;
  /// Removes an attribute given its uniform name.
  /// \param uniformName Uniform name of the attribute to remove.
  void removeAttribute(const std::string& uniformName);
  /// Removes all attributes in the program.
  void clearAttributes() { m_attributes.clear(); }
  /// Sets the program's textures' binding points.
  void initTextures() const;
  /// Binds the program's textures.
  void bindTextures() const;
  /// Removes all entries associated with the given texture.
  /// \param texture Texture to remove the entries for.
  void removeTexture(const Texture& texture);
  /// Removes the entry associated with the given uniform name.
  /// \param uniformName Uniform name to remove the entry for.
  void removeTexture(const std::string& uniformName);
  /// Removes all textures associated to the program.
  void clearTextures() { m_textures.clear(); }
  /// Gets the uniform's location (ID) corresponding to the given name.
  /// \note Location will be -1 if the name is incorrect or if the uniform isn't used in the shader(s) (will be optimized out).
  /// \param name Name of the uniform to recover the location from.
  /// \return Location (ID) of the uniform.
  int recoverUniformLocation(const std::string& name) const;
  /// Sends an integer as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param value Integer to be sent.
  void sendUniform(int index, int value) const;
  /// Sends an unsigned integer as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param value Unsigned integer to be sent.
  void sendUniform(int index, unsigned int value) const;
  /// Sends a floating-point value as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param value Floating-point value to be sent.
  void sendUniform(int index, float value) const;
  /// Sends a list of integers as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Integers to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(int index, const int* values, std::size_t count) const;
  /// Sends a list of unsigned integers as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Unsigned integers to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(int index, const unsigned int* values, std::size_t count) const;
  /// Sends a list of floating-point values as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Floating-point values to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(int index, const float* values, std::size_t count) const;
  /// Sends a list of integers as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Integers to be sent.
  void sendUniform(int index, const std::vector<int>& values) const { sendUniform(index, values.data(), values.size()); }
  /// Sends a list of unsigned integers as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Unsigned integers to be sent.
  void sendUniform(int index, const std::vector<unsigned int>& values) const { sendUniform(index, values.data(), values.size()); }
  /// Sends a list of floating-point values as uniform.
  /// \param index Index of the uniform to send the data to.
  /// \param values Floating-point values to be sent.
  void sendUniform(int index, const std::vector<float>& values) const { sendUniform(index, values.data(), values.size()); }
  /// Sends an integer 2D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec2i& vec) const;
  /// Sends an integer 3D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec3i& vec) const;
  /// Sends an integer 4D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec4i& vec) const;
  /// Sends an unsigned integer 2D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec2u& vec) const;
  /// Sends an unsigned integer 3D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec3u& vec) const;
  /// Sends an unsigned integer 4D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec4u& vec) const;
  /// Sends a floating-point 2D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec2f& vec) const;
  /// Sends a floating-point 3D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec3f& vec) const;
  /// Sends a floating-point 4D vector as uniform.
  /// \param index Index of the uniform to send the vector's data to.
  /// \param vec Vector to be sent.
  void sendUniform(int index, const Vec4f& vec) const;
  /// Sends a floating-point 2x2 matrix as uniform.
  /// \param index Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int index, const Mat2f& mat) const;
  /// Sends a floating-point 3x3 matrix as uniform.
  /// \param index Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int index, const Mat3f& mat) const;
  /// Sends a floating-point 4x4 matrix as uniform.
  /// \param index Index of the uniform to send the matrix's data to.
  /// \param mat Matrix to be sent.
  void sendUniform(int index, const Mat4f& mat) const;
  /// Sends an integer as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param value Integer to be sent.
  void sendUniform(const std::string& name, int value) const { sendUniform(recoverUniformLocation(name), value); }
  /// Sends an unsigned integer as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param value Unsigned integer to be sent.
  void sendUniform(const std::string& name, unsigned int value) const { sendUniform(recoverUniformLocation(name), value); }
  /// Sends a floating-point value as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param value Floating-point value to be sent.
  void sendUniform(const std::string& name, float value) const { sendUniform(recoverUniformLocation(name), value); }
  /// Sends a list of integers as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Integers to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(const std::string& name, const int* values, std::size_t count) const { sendUniform(recoverUniformLocation(name), values, count); }
  /// Sends a list of unsigned integers as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Unsigned integers to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(const std::string& name, const unsigned int* values, std::size_t count) const { sendUniform(recoverUniformLocation(name), values, count); }
  /// Sends a list of floating-point values as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Floating-point values to be sent.
  /// \param count Number of values to be sent.
  void sendUniform(const std::string& name, const float* values, std::size_t count) const { sendUniform(recoverUniformLocation(name), values, count); }
  /// Sends a list of integers as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Integers to be sent.
  void sendUniform(const std::string& name, const std::vector<int>& values) const { sendUniform(recoverUniformLocation(name), values); }
  /// Sends a list of unsigned integers as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Unsigned integers to be sent.
  void sendUniform(const std::string& name, const std::vector<unsigned int>& values) const { sendUniform(recoverUniformLocation(name), values); }
  /// Sends a list of floating-point values as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param values Floating-point values to be sent.
  void sendUniform(const std::string& name, const std::vector<float>& values) const { sendUniform(recoverUniformLocation(name), values); }
  /// Sends an integer 2D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec2i& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends an integer 3D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec3i& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends an integer 4D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec4i& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends an unsigned integer 2D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec2u& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends an unsigned integer 3D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec3u& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends an unsigned integer 4D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec4u& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends a floating-point 2D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec2f& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends a floating-point 3D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec3f& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends a floating-point 4D vector as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param vec Vector to be sent.
  void sendUniform(const std::string& name, const Vec4f& vec) const { sendUniform(recoverUniformLocation(name), vec); }
  /// Sends a floating-point 2x2 matrix as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& name, const Mat2f& mat) const { sendUniform(recoverUniformLocation(name), mat); }
  /// Sends a floating-point 3x3 matrix as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& name, const Mat3f& mat) const { sendUniform(recoverUniformLocation(name), mat); }
  /// Sends a floating-point 4x4 matrix as uniform, which location will be retrieved with its name.
  /// \param name Name of the uniform to retrieve the location from.
  /// \param mat Matrix to be sent.
  void sendUniform(const std::string& name, const Mat4f& mat) const { sendUniform(recoverUniformLocation(name), mat); }

  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&) noexcept = default;

  virtual ~ShaderProgram();

protected:
  OwnerValue<unsigned int, std::numeric_limits<unsigned int>::max()> m_index {};

  using Attribute = std::variant<int, unsigned int, float,
                                 Vec2i, Vec3i, Vec4i, Vec2u, Vec3u, Vec4u, Vec2f, Vec3f, Vec4f,
                                 Mat2f, Mat3f, Mat4f,
                                 std::vector<int>, std::vector<unsigned int>, std::vector<float>>;
  std::unordered_map<std::string, Attribute> m_attributes {};

  std::vector<std::pair<TexturePtr, std::string>> m_textures {};
};

class RenderShaderProgram final : public ShaderProgram {
public:
  RenderShaderProgram() noexcept : ShaderProgram() {}
  RenderShaderProgram(VertexShader&& vertShader, FragmentShader&& fragShader) noexcept
    : RenderShaderProgram() { setShaders(std::move(vertShader), std::move(fragShader)); }
  RenderShaderProgram(VertexShader&& vertShader, FragmentShader&& fragShader, GeometryShader&& geomShader) noexcept
    : RenderShaderProgram() { setShaders(std::move(vertShader), std::move(geomShader), std::move(fragShader)); }

  const VertexShader& getVertexShader() const noexcept { return m_vertShader; }
  bool hasTessellationControlShader() const noexcept { return m_tessCtrlShader.has_value(); }
  const TessellationControlShader& getTessellationControlShader() const noexcept { assert(hasTessellationControlShader()); return *m_tessCtrlShader; }
  bool hasTessellationEvaluationShader() const noexcept { return m_tessEvalShader.has_value(); }
  const TessellationEvaluationShader& getTessellationEvaluationShader() const noexcept { assert(hasTessellationEvaluationShader()); return *m_tessEvalShader; }
  bool hasGeometryShader() const noexcept { return m_geomShader.has_value(); }
  const GeometryShader& getGeometryShader() const noexcept { assert(hasGeometryShader()); return *m_geomShader; }
  const FragmentShader& getFragmentShader() const noexcept { return m_fragShader; }

  void setVertexShader(VertexShader&& vertShader);
  void setTessellationControlShader(TessellationControlShader&& tessCtrlShader);
  void setTessellationEvaluationShader(TessellationEvaluationShader&& tessEvalShader);
  void setGeometryShader(GeometryShader&& geomShader);
  void setFragmentShader(FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, GeometryShader&& geomShader, FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader, TessellationEvaluationShader&& tessEvalShader, FragmentShader&& fragShader);
  void setShaders(VertexShader&& vertShader,
                  TessellationControlShader&& tessCtrlShader,
                  TessellationEvaluationShader&& tessEvalShader,
                  FragmentShader&& fragShader);

  RenderShaderProgram clone() const;
  /// Loads all the shaders contained by the program.
  void loadShaders() const override;
  /// Compiles all the shaders contained by the program.
  void compileShaders() const override;
  /// Destroys the vertex shader, detaching it from the program & deleting it.
  void destroyVertexShader();
  /// Destroys the tessellation control shader (if any), detaching it from the program & deleting it.
  void destroyTessellationControlShader();
  /// Destroys the tessellation evaluation shader (if any), detaching it from the program & deleting it.
  void destroyTessellationEvaluationShader();
  /// Destroys the geometry shader (if any), detaching it from the program & deleting it.
  void destroyGeometryShader();
  /// Destroys the fragment shader, detaching it from the program & deleting it.
  void destroyFragmentShader();

private:
  VertexShader m_vertShader {};
  std::optional<TessellationControlShader> m_tessCtrlShader {};
  std::optional<TessellationEvaluationShader> m_tessEvalShader {};
  std::optional<GeometryShader> m_geomShader {};
  FragmentShader m_fragShader {};
};

class ComputeShaderProgram final : public ShaderProgram {
public:
  ComputeShaderProgram() : ShaderProgram() {}
  explicit ComputeShaderProgram(ComputeShader&& compShader) : ComputeShaderProgram() { setShader(std::move(compShader)); }

  const ComputeShader& getShader() const noexcept { return m_compShader; }

  void setShader(ComputeShader&& compShader);

  ComputeShaderProgram clone() const;
  /// Loads the compute shader contained by the program.
  void loadShaders() const override;
  /// Compiles the compute shader contained by the program.
  void compileShaders() const override;
  void execute(unsigned int groupCountX, unsigned int groupCountY = 1, unsigned int groupCountZ = 1) const;
  /// Destroys the compute shader, detaching it from the program & deleting it.
  void destroyShader();

private:
  ComputeShader m_compShader {};
};

} // namespace Raz

#include "RaZ/Render/ShaderProgram.inl"

#endif // RAZ_SHADERPROGRAM_HPP
