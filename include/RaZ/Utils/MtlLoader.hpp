#pragma once

#ifndef RAZ_MTLLOADER_HPP
#define RAZ_MTLLOADER_HPP

#include <string>
#include <unordered_map>

#include "RaZ/Render/Material.hpp"

namespace Raz {

namespace MtlLoader {

void importMtl(const std::string& mtlFilePath,
               std::vector<MaterialPtr>& materials,
               std::unordered_map<std::string, std::size_t>& materialCorrespIndices);

} // namespace MtlLoader

} // namespace Raz

#endif // RAZ_MTLLOADER_HPP
