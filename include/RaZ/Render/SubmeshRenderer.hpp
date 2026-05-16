#pragma once

#ifndef RAZ_SUBMESHRENDERER_HPP
#define RAZ_SUBMESHRENDERER_HPP

#include "RaZ/Data/Submesh.hpp"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Utils/EnumUtils.hpp"

#include <functional>

namespace Raz {

enum class RenderMode : unsigned int {
  POINT   , ///< Renders the submesh as points.
  //LINE    , ///< Renders the submesh as lines.
  TRIANGLE, ///< Renders the submesh as triangles.
#if !defined(USE_OPENGL_ES)
  PATCH     ///< Renders the submesh as patches, used for tessellation. Requires OpenGL 4.3+; unavailable with OpenGL ES.
#endif
};

/// Flags selecting which vertex attributes are packed into the VBO and bound to shader locations.
/// \see SubmeshRenderer::load()
enum class VertexAttribute : uint8_t {
  Position  = 1,  ///< Vertex position (vec3, location 0).
  Texcoords = 2,  ///< Texture coordinates (vec2, location 1).
  Normal    = 4,  ///< Vertex normal (vec3, location 2).
  Tangent   = 8,  ///< Vertex tangent (vec3, location 3).
  Color     = 16, ///< Vertex color (vec4, location 4).
};
MAKE_ENUM_FLAG(VertexAttribute)

/// Default set of vertex attributes: position, texcoords, normal, and tangent.
inline constexpr VertexAttribute VertexAttributeDefault = VertexAttribute::Position
                                                        | VertexAttribute::Texcoords
                                                        | VertexAttribute::Normal
                                                        | VertexAttribute::Tangent;

class SubmeshRenderer {
public:
  SubmeshRenderer() = default;
  explicit SubmeshRenderer(const Submesh& submesh,
                           RenderMode renderMode = RenderMode::TRIANGLE,
                           VertexAttribute enabledAttribs = VertexAttributeDefault) { load(submesh, renderMode, enabledAttribs); }

  RenderMode getRenderMode() const { return m_renderMode; }
  VertexAttribute getEnabledAttributes() const { return m_enabledAttribs; }
  std::size_t getMaterialIndex() const { return m_materialIndex; }

  /// Sets a specific mode to render the submesh into.
  /// \param renderMode Render mode to apply.
  /// \param submesh Submesh to load the render mode's indices from.
  void setRenderMode(RenderMode renderMode, const Submesh& submesh);
  void setMaterialIndex(std::size_t materialIndex) { m_materialIndex = materialIndex; }

  /// Clones the submesh renderer.
  /// \warning This doesn't load anything onto the graphics card; the load() function must be called afterward with a Submesh for this.
  /// \return Cloned submesh renderer.
  SubmeshRenderer clone() const;
  /// Loads the submesh's data (vertices & indices) onto the graphics card.
  /// \param submesh Submesh to load the data from.
  /// \param renderMode Primitive type to render the submesh with.
  /// \param enabledAttribs Flags selecting which vertex attributes to pack and bind. Defaults to position, texcoords, normal, and tangent.
  void load(const Submesh& submesh,
            RenderMode renderMode = RenderMode::TRIANGLE,
            VertexAttribute enabledAttribs = VertexAttributeDefault);
  /// Draws the submesh in the scene.
  void draw() const;

private:
  void loadVertices(const Submesh& submesh);
  void loadIndices(const Submesh& submesh);

  VertexArray m_vao {};
  VertexBuffer m_vbo {};
  IndexBuffer m_ibo {};

  RenderMode m_renderMode = RenderMode::TRIANGLE;
  VertexAttribute m_enabledAttribs = VertexAttributeDefault;
  std::function<void(const VertexBuffer&, const IndexBuffer&)> m_renderFunc {};

  std::size_t m_materialIndex = 0;
};

} // namespace Raz

#endif // RAZ_SUBMESHRENDERER_HPP
