#pragma once

#ifndef RAZ_MODELLOADER_HPP
#define RAZ_MODELLOADER_HPP

#include "RaZ/Render/Model.hpp"

namespace Raz {

namespace ModelLoader {

ModelPtr importModel(const std::string& filePath);

} // namespace ModelLoader

} // namespace Raz

#endif // RAZ_MODELLOADER_HPP
