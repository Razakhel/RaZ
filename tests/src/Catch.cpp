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

  if (m_diffValueCount > 0)
    m_avgDiff /= static_cast<float>(m_diffValueCount);

  if (!m_diffImgPath.isEmpty() && m_diffValueCount != 0)
    Raz::ImageFormat::save(m_diffImgPath, diffImg, true);

  return (m_avgDiff < (base.getDataType() == Raz::ImageDataType::FLOAT ? m_ratioTol : 255.f * m_ratioTol));
}

std::string IsNearlyEqualToImage::describe() const {
  std::ostringstream stream;

  stream << "Images have " << std::to_string(m_diffValueCount) << " non-matching pixel value(s)\n"
         << "  Minimum difference: " << std::to_string(m_minDiff) << '\n'
         << "  Maximum difference: " << std::to_string(m_maxDiff) << '\n'
         << "  Average difference: " << std::to_string(m_avgDiff);

  return stream.str();
}

void IsNearlyEqualToImage::matchByte(const Raz::Image& base, Raz::Image& diffImg) const {
  for (std::size_t i = 0; i < base.getWidth() * base.getHeight() * base.getChannelCount(); ++i) {
    const int basePixel = static_cast<const uint8_t*>(base.getDataPtr())[i];
    const int compPixel = static_cast<const uint8_t*>(m_comparison.getDataPtr())[i];
    const int absDiff   = std::abs(basePixel - compPixel);

    if (absDiff > 0) {
      ++m_diffValueCount;
      m_minDiff  = std::min(m_minDiff, static_cast<float>(absDiff));
      m_maxDiff  = std::max(m_maxDiff, static_cast<float>(absDiff));
      m_avgDiff += static_cast<float>(absDiff);

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

    if (absDiff > 0.f) {
      ++m_diffValueCount;
      m_minDiff  = std::min(m_minDiff, absDiff);
      m_maxDiff  = std::max(m_maxDiff, absDiff);
      m_avgDiff += absDiff;

      if (!m_diffImgPath.isEmpty())
        static_cast<float*>(diffImg.getDataPtr())[i] = 1.f;
    }
  }
}
