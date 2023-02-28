#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"

#include <fstream>

namespace Raz::FileUtils {

bool isReadable(const FilePath& filePath) {
  return std::ifstream(filePath).good();
}

std::string readFile(const FilePath& filePath) {
  std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);

  if (!file)
    throw std::runtime_error("Error: Could not open the file '" + filePath + '\'');

  // Note that tellg() does not necessarily return a size, but rather a mark pointing at a specific place in the file
  // When opening a file in binary, however, it is currently pretty much always represented as a byte offset
  // See: https://stackoverflow.com/a/22986486/3292304
  const auto fileSize = file.tellg();

  if (fileSize == -1)
    throw std::runtime_error("Error: Failed to get the size of the file '" + filePath + '\'');

  // Returning at the beginning of the file to read it
  file.seekg(0, std::ios::beg);

  std::string fileContent;
  fileContent.resize(static_cast<std::size_t>(fileSize));
  file.read(fileContent.data(), static_cast<std::streamsize>(fileSize));

  return fileContent;
}

} // namespace Raz
