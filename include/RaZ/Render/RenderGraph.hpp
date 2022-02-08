#pragma once

#ifndef RAZ_RENDERGRAPH_HPP
#define RAZ_RENDERGRAPH_HPP

#include "RaZ/Data/Graph.hpp"
#include "RaZ/Render/RenderPass.hpp"
#include "RaZ/Render/RenderProcess.hpp"

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

  /// Adds a render process to the graph.
  /// \tparam RenderProcessT Type of the process to add; must be derived from RenderProcess.
  /// \tparam Args Types of the arguments to be forwared to the render process.
  /// \param args Arguments to be forwarded to the render process.
  /// \return Reference to the newly added render process.
  template <typename RenderProcessT, typename... Args> RenderProcessT& addRenderProcess(Args&&... args);
  void resizeViewport(unsigned int width, unsigned int height);
  void updateShaders() const;
  /// Executes the render graph, executing all passes starting with the geometry's.
  /// \param renderSystem Render system executing the render graph.
  void execute(RenderSystem& renderSystem);

  RenderGraph& operator=(const RenderGraph&) = delete;
  RenderGraph& operator=(RenderGraph&&) noexcept = delete;

private:
  void execute(const RenderPass& renderPass);

  RenderPass m_geometryPass {};
  std::vector<std::unique_ptr<RenderProcess>> m_renderProcesses {};
  std::unordered_set<const RenderPass*> m_executedPasses {};
};

} // namespace Raz

#include "RaZ/Render/RenderGraph.inl"

#endif // RAZ_RENDERGRAPH_HPP
