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
  explicit Image(const std::string& fileName, bool reverse = false) { read(fileName, reverse); }

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  GLenum getColorspace() const { return m_colorspace; }
  const std::vector<uint8_t>& getData() const { return m_data; }
  const uint8_t* getDataPtr() const { return m_data.data(); }

  void read(const std::string& fileName, bool reverse = false);

private:
  void readPng(std::ifstream& fileName, bool reverse);

  unsigned int m_width {};
  unsigned int m_height {};
  GLenum m_colorspace {};
  std::vector<uint8_t> m_data {};
};

} // namespace Raz

#endif // RAZ_IMAGE_HPP
