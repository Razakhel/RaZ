namespace Raz {

template <typename T>
T Image::recoverPixel(std::size_t widthIndex, std::size_t heightIndex) const {
  assert("Error: Recovering a pixel of a single value requires an image having a single channel." && m_channelCount == 1);

  if constexpr (std::is_same_v<T, uint8_t>)
    return recoverByteValue(widthIndex, heightIndex, 0);
  else if constexpr (std::is_same_v<T, float>)
    return recoverFloatValue(widthIndex, heightIndex, 0);
  else
    static_assert(!std::is_same_v<T, T>, "Error: The given pixel's type to be recovered is unsupported.");
}

template <typename T, std::size_t N>
Vector<T, N> Image::recoverPixel(std::size_t widthIndex, std::size_t heightIndex) const {
  static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, float>, "Error: The given pixel's type to be recovered is unsupported.");
  assert("Error: Recovering multiple values for a pixel requires an image having the same channel count." && m_channelCount == N);
  assert("Error: Recovering a byte pixel requires the image to be of a byte type." && (!std::is_same_v<T, uint8_t> || m_dataType == ImageDataType::BYTE));
  assert("Error: Recovering a float pixel requires the image to be of a float type." && (!std::is_same_v<T, float> || m_dataType == ImageDataType::FLOAT));

  const T* imgData = static_cast<T*>(m_data->getDataPtr()) + computeIndex(widthIndex, heightIndex);

  Vector<T, N> res;

  for (std::size_t i = 0; i < N; ++i)
    res[i] = imgData[i];

  return res;
}

template <typename T>
void Image::setPixel(std::size_t widthIndex, std::size_t heightIndex, T val) {
  assert("Error: Recovering a pixel of a single value requires an image having a single channel." && m_channelCount == 1);

  if constexpr (std::is_same_v<T, uint8_t>)
    setByteValue(widthIndex, heightIndex, 0, val);
  else if constexpr (std::is_same_v<T, float>)
    setFloatValue(widthIndex, heightIndex, 0, val);
  else
    static_assert(!std::is_same_v<T, T>, "Error: The given pixel's type to be set is unsupported.");
}

template <typename T, std::size_t N>
void Image::setPixel(std::size_t widthIndex, std::size_t heightIndex, const Vector<T, N>& values) {
  static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, float>, "Error: The given pixel's type to be set is unsupported.");
  assert("Error: Setting multiple values for a pixel requires an image having the same channel count." && m_channelCount == N);
  assert("Error: Setting a byte pixel requires the image to be of a byte type." && (!std::is_same_v<T, uint8_t> || m_dataType == ImageDataType::BYTE));
  assert("Error: Setting a float pixel requires the image to be of a float type." && (!std::is_same_v<T, float> || m_dataType == ImageDataType::FLOAT));

  T* imgData = static_cast<T*>(m_data->getDataPtr()) + computeIndex(widthIndex, heightIndex);

  for (std::size_t i = 0; i < N; ++i)
    imgData[i] = values[i];
}


} // namespace Raz
