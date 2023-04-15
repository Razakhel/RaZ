#include "RaZ/Render/FilmGrainRenderProcess.hpp"
#include "RaZ/Render/RenderPass.hpp"

#include <string_view>

namespace Raz {

namespace {

constexpr std::string_view filmGrainSource = {
#include "film_grain.frag.embed"
};

} // namespace

FilmGrainRenderProcess::FilmGrainRenderProcess(RenderGraph& renderGraph)
  : MonoPassRenderProcess(renderGraph, FragmentShader::loadFromSource(filmGrainSource), "Film grain") { setStrength(0.05f); }

void FilmGrainRenderProcess::setInputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setInputBuffer(std::move(colorBuffer), "uniBuffer");
}

void FilmGrainRenderProcess::setOutputBuffer(Texture2DPtr colorBuffer) {
  MonoPassRenderProcess::setOutputBuffer(std::move(colorBuffer), 0);
}

void FilmGrainRenderProcess::setStrength(float strength) const {
  m_pass.getProgram().setAttribute(strength, "uniStrength");
  m_pass.getProgram().sendAttributes();
}

} // namespace Raz
