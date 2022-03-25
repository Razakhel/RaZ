#pragma once

#ifndef RAZ_MATERIAL_HPP
#define RAZ_MATERIAL_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Render/Texture.hpp"

#include <unordered_map>
#include <variant>

namespace Raz {

enum class MaterialType {
  COOK_TORRANCE, ///< Cook-Torrance (PBR) material.
  BLINN_PHONG    ///< Blinn-Phong material.
};

class Material {
public:
  Material() = default;
  /// Creates a material with a predefined type.
  /// \param type Material type to apply.
  explicit Material(MaterialType type) { loadType(type); }
  Material(Material&&) noexcept = default;

  const RenderShaderProgram& getProgram() const noexcept { return m_program; }
  RenderShaderProgram& getProgram() noexcept { return m_program; }
  /// Checks if the material has any attribute or texture.
  /// \return True if the material has neither an attribute nor a texture, false otherwise.
  bool isEmpty() const noexcept { return m_attributes.empty() && m_textures.empty(); }
  bool hasAttribute(const std::string& uniformName) const noexcept;
  std::size_t getAttributeCount() const noexcept { return m_attributes.size(); }
  template <typename T> const T& getAttribute(const std::string& uniformName) const noexcept {
    assert("Error: The given attribute uniform name does not exist." && hasAttribute(uniformName));
    assert("Error: The fetched attribute is not of the asked type." && std::holds_alternative<T>(m_attributes.find(uniformName)->second));
    return std::get<T>(m_attributes.find(uniformName)->second);
  }
  /// Checks if there is a texture entry with the given texture.
  /// \param texture Texture to find.
  /// \return True if an entry has been found, false otherwise.
  bool hasTexture(const Texture& texture) const noexcept;
  /// Checks if there is a texture entry with the given uniform name.
  /// \param uniformName Uniform name to find.
  /// \return True if an entry has been found, false otherwise.
  bool hasTexture(const std::string& uniformName) const noexcept;
  std::size_t getTextureCount() const noexcept { return m_textures.size(); }
  const Texture& getTexture(std::size_t index) const noexcept { return *m_textures[index].first; }
  const Texture& getTexture(const std::string& uniformName) const;

  /// Sets an attribute to be sent to the shaders. If the uniform name already exists, replaces the attribute's value.
  /// \tparam T Type of the attribute to set. Must be a type handled by ShaderProgram::sendUniform().
  /// \param attribVal Attribute to set.
  /// \param uniformName Uniform name of the attribute to set.
  template <typename T> void setAttribute(T&& attribVal, std::string uniformName) { m_attributes[std::move(uniformName)] = std::forward<T>(attribVal); }
  /// Sets a texture to be bound to the shaders. If the uniform name already exists, replace the texture.
  /// \param texture Texture to set.
  /// \param uniformName Uniform name to bind the texture to.
  void setTexture(TexturePtr texture, std::string uniformName);

  Material clone() const { return *this; }
  /// Loads a predefined material type, setting default shaders & adding all needed attributes & textures if they do not exist yet.
  /// \param type Material type to apply.
  void loadType(MaterialType type);
  void sendAttributes() const;
  /// Removes an attribute given its uniform name.
  /// \param uniformName Uniform name of the attribute to remove.
  void removeAttribute(const std::string& uniformName);
  /// Removes all attributes in the material.
  void clearAttributes() { m_attributes.clear(); }
  void initTextures() const;
  void bindTextures() const;
  /// Removes all entries associated with the given texture.
  /// \param texture Texture to remove the entries for.
  void removeTexture(const Texture& texture);
  /// Removes the entry associated with the given uniform name.
  /// \param uniformName Uniform name to remove the entry for.
  void removeTexture(const std::string& uniformName);
  void clearTextures() { m_textures.clear(); }

  Material& operator=(Material&&) noexcept = default;

  virtual ~Material() = default;

private:
  Material(const Material& material) : m_program{ material.m_program.clone() }, m_attributes{ material.m_attributes }, m_textures{ material.m_textures } {}

  RenderShaderProgram m_program {};

  using Attribute = std::variant<int, unsigned int, float, Vec2i, Vec3i, Vec4i, Vec2u, Vec3u, Vec4u, Vec2f, Vec3f, Vec4f, Mat2f, Mat3f, Mat4f>;
  std::unordered_map<std::string, Attribute> m_attributes {};

  std::vector<std::pair<TexturePtr, std::string>> m_textures {};
};

} // namespace Raz

#endif // RAZ_MATERIAL_HPP
