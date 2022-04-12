#pragma once

#ifndef RAZ_RENDERGRAPH_HPP
#define RAZ_RENDERGRAPH_HPP

#include "RaZ/Data/Graph.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <unordered_set>

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

  void resizeViewport(unsigned int width, unsigned int height);
  void updateShaders() const;
  /// Executes the render graph, launching all passes followed by their respective children, starting with the geometry pass.
  /// \param renderSystem Render system executing the render graph.
  void execute(RenderSystem& renderSystem);

  RenderGraph& operator=(const RenderGraph&) = delete;
  RenderGraph& operator=(RenderGraph&&) noexcept = delete;

private:
  void execute(const RenderPass& renderPass);

  RenderPass m_geometryPass {};
  std::unordered_set<const RenderPass*> m_executedPasses {};
};

} // namespace Raz

#endif // RAZ_RENDERGRAPH_HPP
