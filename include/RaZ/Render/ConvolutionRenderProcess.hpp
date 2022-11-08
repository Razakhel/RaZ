#pragma once

#ifndef RAZ_CONVOLUTIONRENDERPROCESS_HPP
#define RAZ_CONVOLUTIONRENDERPROCESS_HPP

#include "RaZ/Render/MonoPassRenderProcess.hpp"

namespace Raz {

template <typename T, std::size_t W, std::size_t H>
class Matrix;
using Mat3f = Matrix<float, 3, 3>;

class ConvolutionRenderProcess final : public MonoPassRenderProcess {
public:
  ConvolutionRenderProcess(RenderGraph& renderGraph, const Mat3f& kernel, std::string passName = {});

  void setInputBuffer(Texture2DPtr colorBuffer);
  void setOutputBuffer(Texture2DPtr colorBuffer);
  void setKernel(const Mat3f& kernel) const;
};

} // namespace Raz

#endif // RAZ_CONVOLUTIONRENDERPROCESS_HPP
