#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace Raz {

class Image {
public:
  Image() = default;
  Image(const std::string& fileName) { read(fileName); }

  const std::vector<uint8_t> getData() const { return m_data; }
  std::vector<uint8_t> getData() { return m_data; }

  void read(const std::string& fileName);

private:
  void readJpeg(std::ifstream& fileName);
  void readPng(std::ifstream& fileName);
  void readTga(std::ifstream& fileName);
  void readBmp(std::ifstream& fileName);
  void readBpg(std::ifstream& fileName);

  std::vector<uint8_t> m_data;
};

} // namespace Raz

#endif // RAZ_IMAGE_HPP
