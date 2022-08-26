#pragma once

#ifndef RAZ_BOXBLURRENDERPROCESS_HPP
#define RAZ_BOXBLURRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class BoxBlurRenderProcess final : public MonoPassRenderProcess {
public:
  BoxBlurRenderProcess(RenderGraph& renderGraph);

  void setInputBuffer(TexturePtr colorBuffer);
  void setStrength(unsigned int strength);
};

} // namespace Raz

#endif // RAZ_BOXBLURRENDERPROCESS_HPP
