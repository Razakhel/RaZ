#include <fstream>

#include "RaZ/Render/Model.hpp"
#include "RaZ/Utils/FileUtils.hpp"

namespace Raz {

ModelPtr Model::import(const std::string& filePath) {
  ModelPtr model;
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = FileUtils::extractFileExtension(filePath);

    if (format == "obj" || format == "OBJ")
      model = importObj(file, filePath);
    else if (format == "off" || format == "OFF")
      model = importOff(file);
    else if (format == "fbx" || format == "FBX")
#if defined(FBX_ENABLED)
      model = importFbx(filePath);
#else
      throw std::runtime_error("Error: FBX SDK not found; check that you allowed its usage when building RaZ");
#endif
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + filePath + "'");
  }

  return model;
}

void Model::save(const std::string& filePath) const {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);
  const std::string format = FileUtils::extractFileExtension(filePath);

  if (file) {
    if (format == "obj" || format == "OBJ")
      saveObj(file, filePath);
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Unable to create a file as '" + filePath + "'; path to file must exist");
  }
}

} // namespace Raz
