#pragma once

#ifndef RAZ_RENDERGRAPH_HPP
#define RAZ_RENDERGRAPH_HPP

#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Graph.hpp"

namespace Raz {

class Entity;
class RenderSystem;

class RenderGraph : public Graph<RenderPass> {
public:
  RenderGraph() = default;
  RenderGraph(const RenderGraph&) = delete;
  RenderGraph(RenderGraph&&) noexcept = delete;

  bool isValid() const;
  const RenderPass& getGeometryPass() const { return m_geometryPass; }
  RenderPass& getGeometryPass() { return m_geometryPass; }

  const Texture& addTextureBuffer(unsigned int width, unsigned int height, int bindingIndex, ImageColorspace colorspace);
  void resizeViewport(unsigned int width, unsigned int height);
  void updateShaders() const;
  /// Executes the render graph, launching all passes followed by their respective children, starting with the geometry pass.
  /// \param renderSystem Render system executing the render graph.
  void execute(RenderSystem& renderSystem) const;

  RenderGraph& operator=(const RenderGraph&) = delete;
  RenderGraph& operator=(RenderGraph&&) noexcept = delete;

private:
  RenderPass m_geometryPass {};
  std::vector<std::unique_ptr<Texture>> m_buffers {};
};

} // namespace Raz

#endif // RAZ_RENDERGRAPH_HPP
