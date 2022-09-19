#pragma once

#ifndef RAZ_BOXBLURRENDERPROCESS_HPP
#define RAZ_BOXBLURRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class BoxBlurRenderProcess final : public MonoPassRenderProcess {
public:
  explicit BoxBlurRenderProcess(RenderGraph& renderGraph);

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setStrength(unsigned int strength);
};

} // namespace Raz

#endif // RAZ_BOXBLURRENDERPROCESS_HPP
