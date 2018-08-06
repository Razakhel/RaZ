#pragma once

#ifndef RAZ_IMAGE_HPP
#define RAZ_IMAGE_HPP

#include <memory>
#include <string>
#include <vector>

#include "glew/include/GL/glew.h"

namespace Raz {

enum class ImageDataType : uint8_t { BYTE = 0,
                                     FLOAT };

struct ImageData {
  virtual void* getDataPtr() = 0;
  virtual const void* getDataPtr() const = 0;

  virtual bool isEmpty() const = 0;
  virtual ImageDataType getDataType() const = 0;

  virtual ~ImageData() = default;
};

using ImageDataPtr = std::unique_ptr<ImageData>;

struct ImageDataB : public ImageData {
  ImageDataType getDataType() const override { return ImageDataType::BYTE; }
  void* getDataPtr() override { return data.data(); }
  const void* getDataPtr() const override { return data.data(); }

  bool isEmpty() const override { return data.empty(); }

  std::vector<uint8_t> data;
};

struct ImageDataF : public ImageData {
  ImageDataType getDataType() const override { return ImageDataType::FLOAT; }
  void* getDataPtr() override { return data.data(); }
  const void* getDataPtr() const override { return data.data(); }

  bool isEmpty() const override { return data.empty(); }

  std::vector<float> data;
};

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
  ImageDataType getDataType() const { return m_data->getDataType(); }
  const void* getDataPtr() const { return m_data->getDataPtr(); }

  bool isEmpty() const { return (!m_data || m_data->isEmpty()); }
  void read(const std::string& filePath, bool reverse = false);
  void save(const std::string& filePath, bool reverse = false) const;

private:
  void readPng(std::ifstream& file, bool reverse);
  void savePng(std::ofstream& file, bool reverse) const;
  void readTga(std::ifstream& fileStream);
  //void saveTga(std::ofstream& file) const;

  unsigned int m_width {};
  unsigned int m_height {};
  ImageColorspace m_colorspace {};
  uint8_t m_channelCount {};
  uint8_t m_bitDepth {};
  ImageDataPtr m_data {};
};

using ImagePtr = std::unique_ptr<Image>;

} // namespace Raz

#endif // RAZ_IMAGE_HPP
