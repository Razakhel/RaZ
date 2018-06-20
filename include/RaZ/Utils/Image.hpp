#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <memory>
#include <string>
#include <vector>

#include "glew/include/GL/glew.h"

namespace Raz {

enum class ImageColorspace { GRAY = GL_RED,
                             GRAY_ALPHA = GL_RG,
                             RGB = GL_RGB,
                             RGBA = GL_RGBA,
                             DEPTH = GL_DEPTH_COMPONENT };

class Image {
public:
  Image() = default;
  explicit Image(const std::string& fileName, bool reverse = false) { read(fileName, reverse); }

  unsigned int getWidth() const { return m_width; }
  unsigned int getHeight() const { return m_height; }
  ImageColorspace getColorspace() const { return m_colorspace; }
  const std::vector<uint8_t>& getData() const { return m_data; }
  const uint8_t* getDataPtr() const { return m_data.data(); }

  void read(const std::string& filePath, bool reverse = false);
  void save(const std::string& filePath, bool reverse = false) const;

private:
  void readPng(std::ifstream& file, bool reverse);
  void savePng(std::ofstream& file, bool reverse) const;

  unsigned int m_width {};
  unsigned int m_height {};
  ImageColorspace m_colorspace {};
  uint8_t m_channelCount {};
  uint8_t m_bitDepth {};
  std::vector<uint8_t> m_data {};
};

using ImagePtr = std::unique_ptr<Image>;

} // namespace Raz

#endif // RAZ_IMAGE_HPP
