#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <memory>
#include <string>
#include <vector>

#include "glew/include/GL/glew.h"

namespace Raz {

class Image {
public:
  Image() = default;
  explicit Image(const std::string& fileName) { read(fileName); }

  std::size_t getWidth() const { return m_width; }
  std::size_t getHeight() const { return m_height; }
  GLenum getColorspace() const { return m_colorspace; }
  const std::vector<uint8_t>& getData() const { return m_data; }
  const uint8_t* getDataPtr() const { return m_data.data(); }

private:
  void read(const std::string& fileName);
  void readPng(std::ifstream& fileName);

  std::size_t m_width {};
  std::size_t m_height {};
  GLenum m_colorspace {};
  std::vector<uint8_t> m_data;
};

} // namespace Raz

#endif // RAZ_IMAGE_HPP
