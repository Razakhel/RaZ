#pragma once

#ifndef RAZ_TONEMAPPINGRENDERPROCESS_HPP
#define RAZ_TONEMAPPINGRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

class ReinhardToneMapping final : public MonoPassRenderProcess {
public:
  explicit ReinhardToneMapping(RenderGraph& renderGraph);

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  void setMaxWhiteValue(float strength) const;
};

} // namespace Raz

#endif // RAZ_TONEMAPPINGRENDERPROCESS_HPP
