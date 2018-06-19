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
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + filePath + "'");
  }

  return model;
}

} // namespace Raz
