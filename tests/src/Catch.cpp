#include "Catch.hpp"

#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"

bool IsNearlyEqualToImage::match(const Raz::Image& base) const {
  if (base.getWidth() != m_comparison.getWidth() || base.getHeight() != m_comparison.getHeight())
    throw std::invalid_argument("Error: Images must have the same dimensions in order to be compared.");

  if (base.getChannelCount() != m_comparison.getChannelCount())
    throw std::invalid_argument("Error: Images must have the same channel count in order to be compared.");

  if (base.getColorspace() != m_comparison.getColorspace())
    throw std::invalid_argument("Error: Images must have the same colorspace in order to be compared.");

  if (base.getDataType() != m_comparison.getDataType())
    throw std::invalid_argument("Error: Images must be of the same data type in order to be compared.");

  Raz::Image diffImg((!m_diffImgPath.isEmpty() ? base.getWidth() : 0),
                     (!m_diffImgPath.isEmpty() ? base.getHeight() : 0),
                     base.getColorspace(),
                     base.getDataType());

  if (base.getDataType() == Raz::ImageDataType::FLOAT)
    matchFloat(base, diffImg);
  else
    matchByte(base, diffImg);

  if (!m_diffImgPath.isEmpty() && m_diffValueCount != 0)
    Raz::ImageFormat::save(m_diffImgPath, diffImg);

  return (m_diffValueCount == 0);
}

std::string IsNearlyEqualToImage::describe() const {
  std::ostringstream stream;

  stream << "Images have " << std::to_string(m_diffValueCount) << " non-matching pixel value(s)";

  if (!m_diffImgPath.isEmpty())
    stream << "\nAn image containing the pixels difference has been saved to '" + m_diffImgPath.toUtf8() + '\'';

  return stream.str();
}

void IsNearlyEqualToImage::matchByte(const Raz::Image& base, Raz::Image& diffImg) const {
  for (std::size_t i = 0; i < base.getWidth() * base.getHeight() * base.getChannelCount(); ++i) {
    const int basePixel = static_cast<const uint8_t*>(base.getDataPtr())[i];
    const int compPixel = static_cast<const uint8_t*>(m_comparison.getDataPtr())[i];
    const int absDiff   = std::abs(basePixel - compPixel);

    if (absDiff > 10) {
      ++m_diffValueCount;

      if (!m_diffImgPath.isEmpty())
        static_cast<uint8_t*>(diffImg.getDataPtr())[i] = 255;
    }
  }
}

void IsNearlyEqualToImage::matchFloat(const Raz::Image& base, Raz::Image& diffImg) const {
  for (std::size_t i = 0; i < base.getWidth() * base.getHeight() * base.getChannelCount(); ++i) {
    const float basePixel = static_cast<const float*>(base.getDataPtr())[i];
    const float compPixel = static_cast<const float*>(m_comparison.getDataPtr())[i];
    const float absDiff   = std::abs(basePixel - compPixel);

    if (absDiff > 0.01f) {
      ++m_diffValueCount;

      if (!m_diffImgPath.isEmpty())
        static_cast<float*>(diffImg.getDataPtr())[i] = 1.f;
    }
  }
}
