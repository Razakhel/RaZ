#pragma once

#ifndef RAZ_PIPELINEVK_HPP
#define RAZ_PIPELINEVK_HPP

#include "RaZ/Math/Vector.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"

namespace Raz {

struct PipelineOptions {

};

class Pipeline {
public:
  Pipeline(VertexShader&& vertexShader, FragmentShader&& fragmentShader);

  void create();

  ~Pipeline();

private:
  VkPipeline m_pipeline {};

  ShaderProgram m_program {};

  Vec2u m_viewportSize {};
  Vec4f m_clearColor {};
};

} // namespace Raz

#endif // RAZ_PIPELINEVK_HPP
