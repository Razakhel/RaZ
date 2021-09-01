#pragma once

#ifndef RAZ_PIPELINEGL_HPP
#define RAZ_PIPELINEGL_HPP

#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Pipeline {
public:

private:
  Vec2f m_viewportSize {};
  Vec4f m_clearColor {};
};

} // namespace Raz

#endif // RAZ_PIPELINEGL_HPP
