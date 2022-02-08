namespace Raz {

template <typename RenderProcessT, typename... Args>
RenderProcessT& RenderGraph::addRenderProcess(Args&&... args) {
  static_assert(std::is_base_of_v<RenderProcess, RenderProcessT>, "Error: The added render process must be derived from RenderProcess.");
  static_assert(!std::is_same_v<RenderProcess, RenderProcessT>, "Error: The added render process must not be of specific type 'RenderProcess'.");

  return static_cast<RenderProcessT&>(*m_renderProcesses.emplace_back(std::make_unique<RenderProcessT>(*this, std::forward<Args>(args)...)));
}

} // namespace Raz
