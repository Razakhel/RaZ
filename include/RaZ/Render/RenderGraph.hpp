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
  friend RenderSystem;

public:
  RenderGraph();
  RenderGraph(const RenderGraph&) = delete;
  RenderGraph(RenderGraph&&) noexcept = delete;

  bool isValid() const;
  const RenderPass& getGeometryPass() const { return m_geometryPass; }
  RenderPass& getGeometryPass() { return m_geometryPass; }

  void setFinalBuffer(Texture2DPtr texture);
  void setGammaStrength(float gammaStrength);

  /// Adds a render process to the graph.
  /// \tparam RenderProcessT Type of the process to add; must be derived from RenderProcess.
  /// \tparam Args Types of the arguments to be forwared to the render process.
  /// \param args Arguments to be forwarded to the render process.
  /// \return Reference to the newly added render process.
  template <typename RenderProcessT, typename... Args> RenderProcessT& addRenderProcess(Args&&... args);
  void resizeViewport(unsigned int width, unsigned int height);
  void updateShaders() const;

  RenderGraph& operator=(const RenderGraph&) = delete;
  RenderGraph& operator=(RenderGraph&&) noexcept = delete;

private:
  /// Executes the render graph, executing all passes starting with the geometry's.
  /// \param renderSystem Render system executing the render graph.
  void execute(RenderSystem& renderSystem);
  /// Executes the geometry pass.
  /// \param renderSystem Render system executing the render graph.
  void executeGeometryPass(RenderSystem& renderSystem) const;
  /// Executes a render pass, which in turn recursively executes its parents if they have not already been in the current frame.
  /// \param renderPass Render pass to be executed.
  void executePass(const RenderPass& renderPass);

  RenderPass m_geometryPass {};
  RenderPass m_gammaCorrectionPass {};
  std::vector<std::unique_ptr<RenderProcess>> m_renderProcesses {};
  std::unordered_set<const RenderPass*> m_executedPasses {};
};

} // namespace Raz

#include "RaZ/Render/RenderGraph.inl"

#endif // RAZ_RENDERGRAPH_HPP
