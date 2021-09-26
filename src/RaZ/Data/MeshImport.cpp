#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <fstream>

namespace Raz {

void Mesh::import(const FilePath& filePath) {
  // Resetting the mesh to an empty state before importing
  m_submeshes.clear();
  m_submeshes.resize(1);

  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Couldn't open the mesh file '" + filePath + "'");

  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (format == "off")
    importOff(file);
  else
    throw std::invalid_argument("Error: '" + format + "' mesh format is not supported");
}

} // namespace Raz
