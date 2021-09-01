#include "RaZ/Render/Vulkan/PipelineVk.hpp"
#include "RaZ/Utils/FilePath.hpp"

namespace Raz {

Pipeline::Pipeline(VertexShader&& vertexShader, FragmentShader&& fragmentShader) : m_program(std::move(vertexShader), std::move(fragmentShader)) {
  create();
}

void Pipeline::create() {


  m_program.destroyVertexShader();
  m_program.destroyFragmentShader();
}

Pipeline::~Pipeline() {
  //Renderer::destroyPipeline();
}

} // namespace Raz
