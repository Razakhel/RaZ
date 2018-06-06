namespace Raz {

template <typename T>
void UniformBuffer::sendData(T data, unsigned int offset) const {
  glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(data), &data);
}

template <typename T, std::size_t Size>
void UniformBuffer::sendData(const Vector<T, Size>& vec, unsigned int offset) const {
  glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(vec), vec.getDataPtr());
}


template <typename T, std::size_t W, std::size_t H>
void UniformBuffer::sendData(const Matrix<T, W, H>& mat, unsigned int offset) const {
  glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(mat), mat.getDataPtr());
}

} // namespace Raz
