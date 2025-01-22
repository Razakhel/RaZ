#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"

#include "tracy/Tracy.hpp"

#include <fstream>

namespace Raz::FileUtils {

namespace {

template <typename T>
T readFile(const FilePath& filePath) {
  ZoneScopedN("[FileUtils]::readFile");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  std::ifstream file(filePath, std::ios::binary | std::ios::ate);

  if (!file)
    throw std::runtime_error("[FileUtils] Could not open the file '" + filePath + '\'');

  // Note that tellg() does not necessarily return a size, but rather a mark pointing at a specific place in the file
  // When opening a file in binary however, it is currently pretty much always represented as a byte offset
  // See: https://stackoverflow.com/a/22986486/3292304
  const auto fileSize = file.tellg();

  if (fileSize == -1)
    throw std::runtime_error("[FileUtils] Failed to get the size of the file '" + filePath + '\'');

  // Returning at the beginning of the file to read it
  file.seekg(0, std::ios::beg);

  T fileContent;
  fileContent.resize(static_cast<std::size_t>(fileSize));
  file.read(reinterpret_cast<char*>(fileContent.data()), static_cast<std::streamsize>(fileSize));

  return fileContent;
}

} // namespace

bool isReadable(const FilePath& filePath) {
  return std::ifstream(filePath).good();
}

std::vector<unsigned char> readFileToArray(const FilePath& filePath) {
  ZoneScopedN("FileUtils::readFileToArray");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());
  return readFile<std::vector<unsigned char>>(filePath);
}

std::string readFileToString(const FilePath& filePath) {
  ZoneScopedN("FileUtils::readFileToString");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());
  return readFile<std::string>(filePath);
}

} // namespace Raz
