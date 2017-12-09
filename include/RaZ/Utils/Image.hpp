#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <memory>
#include <string>
#include <vector>

namespace Raz {

class Image {
public:
  Image() = default;
  explicit Image(const std::string& fileName) { read(fileName); }

  std::size_t getWidth() const { return m_width; }
  std::size_t getHeight() const { return m_height; }
  const std::vector<uint8_t>& getData() const { return m_data; }
  const uint8_t* getDataPtr() const { return m_data.data(); }

  void read(const std::string& fileName);

private:
  void readJpeg(std::ifstream& fileName);
  void readPng(std::ifstream& fileName);
  void readTga(std::ifstream& fileName);
  void readBmp(std::ifstream& fileName);
  void readBpg(std::ifstream& fileName);

  std::size_t m_width {};
  std::size_t m_height {};
  std::vector<uint8_t> m_data;
};

using ImagePtr = std::shared_ptr<Image>;

} // namespace Raz

#endif // RAZ_IMAGE_HPP
