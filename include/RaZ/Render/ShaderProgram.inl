namespace Raz {

template <typename T>
void ShaderProgram::sendUniform(const std::string& uniformName, T value) const {
  sendUniform(recoverUniformLocation(uniformName), value);
}

} // namespace Raz
