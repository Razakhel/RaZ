#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <array>

namespace Raz {

namespace {

inline void checkProgramUsed([[maybe_unused]] const ShaderProgram& program) {
#if defined(RAZ_CONFIG_DEBUG)
  if (!program.isUsed())
    Logger::error("The current shader program must be defined as used before sending uniforms to it.");
#endif
}

} // namespace

ShaderProgram::ShaderProgram()
  : m_index{ Renderer::createProgram() } {}

ShaderProgram::ShaderProgram(ShaderProgram&& program) noexcept
  : m_index{ std::exchange(program.m_index, std::numeric_limits<unsigned int>::max()) },
    m_uniforms{ std::move(program.m_uniforms) } {}

void ShaderProgram::link() const {
  Logger::debug("[ShaderProgram] Linking (ID: " + std::to_string(m_index) + ")...");
  Renderer::linkProgram(m_index);
  Logger::debug("[ShaderProgram] Linked");
}

bool ShaderProgram::isLinked() const {
  return Renderer::isProgramLinked(m_index);
}

void ShaderProgram::use() const {
  Renderer::useProgram(m_index);
}

bool ShaderProgram::isUsed() const {
  return (Renderer::getCurrentProgram() == m_index);
}

void ShaderProgram::updateShaders() const {
  Logger::debug("[ShaderProgram] Updating shaders...");

  loadShaders();
  compileShaders();
  link();

  Logger::debug("[ShaderProgram] Updated shaders");
}

void ShaderProgram::createUniform(const std::string& uniformName) {
  m_uniforms.emplace(uniformName, recoverUniformLocation(uniformName));
}

int ShaderProgram::recoverUniformLocation(const std::string& uniformName) const {
  const auto uniform = m_uniforms.find(uniformName);

  if (uniform != m_uniforms.cend())
    return uniform->second;

  return Renderer::recoverUniformLocation(m_index, uniformName.c_str());
}

void ShaderProgram::sendUniform(int uniformIndex, int value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, unsigned int value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, float value) const {
  checkProgramUsed(*this);
  Renderer::sendUniform(uniformIndex, value);
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec2i& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector2i(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec3i& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector3i(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec4i& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector4i(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec2u& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector2ui(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec3u& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector3ui(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec4u& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector4ui(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec2f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector2(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec3f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector3(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Vec4f& vec) const {
  checkProgramUsed(*this);
  Renderer::sendUniformVector4(uniformIndex, vec.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat2f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix2x2(uniformIndex, mat.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat3f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix3x3(uniformIndex, mat.getDataPtr());
}

void ShaderProgram::sendUniform(int uniformIndex, const Mat4f& mat) const {
  checkProgramUsed(*this);
  Renderer::sendUniformMatrix4x4(uniformIndex, mat.getDataPtr());
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& program) noexcept {
  std::swap(m_index, program.m_index);
  m_uniforms = std::move(program.m_uniforms);

  return *this;
}

ShaderProgram::~ShaderProgram() {
  if (m_index == std::numeric_limits<unsigned int>::max())
    return;

  Logger::debug("[ShaderProgram] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteProgram(m_index);
  Logger::debug("[ShaderProgram] Destroyed");
}

void RenderShaderProgram::setVertexShader(VertexShader&& vertShader) {
  Logger::debug("[RenderShaderProgram] Setting vertex shader (ID: " + std::to_string(vertShader.getIndex()) + ", path: '" + vertShader.getPath() + "')");

  if (Renderer::isShaderAttached(m_index, m_vertShader.getIndex()))
    Renderer::detachShader(m_index, m_vertShader.getIndex());

  m_vertShader = std::move(vertShader);
  m_vertShader.compile();

  Renderer::attachShader(m_index, m_vertShader.getIndex());
}

void RenderShaderProgram::setTessellationControlShader(TessellationControlShader&& tessCtrlShader) {
  Logger::debug("[RenderShaderProgram] Setting tessellation control shader (ID: "
                + std::to_string(tessCtrlShader.getIndex()) + ", path: '" + tessCtrlShader.getPath() + "')");

  if (m_tessCtrlShader && Renderer::isShaderAttached(m_index, m_tessCtrlShader->getIndex()))
    Renderer::detachShader(m_index, m_tessCtrlShader->getIndex());

  m_tessCtrlShader = std::move(tessCtrlShader);
  m_tessCtrlShader->compile();

  Renderer::attachShader(m_index, m_tessCtrlShader->getIndex());
}

void RenderShaderProgram::setTessellationEvaluationShader(TessellationEvaluationShader&& tessEvalShader) {
  Logger::debug("[RenderShaderProgram] Setting tessellation evaluation shader (ID: "
                + std::to_string(tessEvalShader.getIndex()) + ", path: '" + tessEvalShader.getPath() + "')");

  if (m_tessEvalShader && Renderer::isShaderAttached(m_index, m_tessEvalShader->getIndex()))
    Renderer::detachShader(m_index, m_tessEvalShader->getIndex());

  m_tessEvalShader = std::move(tessEvalShader);
  m_tessEvalShader->compile();

  Renderer::attachShader(m_index, m_tessEvalShader->getIndex());
}

void RenderShaderProgram::setGeometryShader(GeometryShader&& geomShader) {
  Logger::debug("[RenderShaderProgram] Setting geometry shader (ID: " + std::to_string(geomShader.getIndex()) + ", path: '" + geomShader.getPath() + "')");

  if (m_geomShader && Renderer::isShaderAttached(m_index, m_geomShader->getIndex()))
    Renderer::detachShader(m_index, m_geomShader->getIndex());

  m_geomShader = std::move(geomShader);
  m_geomShader->compile();

  Renderer::attachShader(m_index, m_geomShader->getIndex());
}

void RenderShaderProgram::setFragmentShader(FragmentShader&& fragShader) {
  Logger::debug("[RenderShaderProgram] Setting fragment shader (ID: " + std::to_string(fragShader.getIndex()) + ", path: '" + fragShader.getPath() + "')");

  if (Renderer::isShaderAttached(m_index, m_fragShader.getIndex()))
    Renderer::detachShader(m_index, m_fragShader.getIndex());

  m_fragShader = std::move(fragShader);
  m_fragShader.compile();

  Renderer::attachShader(m_index, m_fragShader.getIndex());
}

void RenderShaderProgram::setShaders(VertexShader&& vertShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setFragmentShader(std::move(fragShader));

  link();
}

void RenderShaderProgram::setShaders(VertexShader&& vertShader, GeometryShader&& geomShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setGeometryShader(std::move(geomShader));
  setFragmentShader(std::move(fragShader));

  link();
}

void RenderShaderProgram::setShaders(VertexShader&& vertShader, TessellationEvaluationShader&& tessEvalShader, FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setTessellationEvaluationShader(std::move(tessEvalShader));
  setFragmentShader(std::move(fragShader));

  link();
}
void RenderShaderProgram::setShaders(VertexShader&& vertShader,
                                     TessellationControlShader&& tessCtrlShader,
                                     TessellationEvaluationShader&& tessEvalShader,
                                     FragmentShader&& fragShader) {
  setVertexShader(std::move(vertShader));
  setTessellationControlShader(std::move(tessCtrlShader));
  setTessellationEvaluationShader(std::move(tessEvalShader));
  setFragmentShader(std::move(fragShader));

  link();
}

RenderShaderProgram RenderShaderProgram::clone() const {
  RenderShaderProgram program;

  program.setVertexShader(m_vertShader.clone());
  if (m_tessCtrlShader) program.setTessellationControlShader(m_tessCtrlShader->clone());
  if (m_tessEvalShader) program.setTessellationEvaluationShader(m_tessEvalShader->clone());
  if (m_geomShader) program.setGeometryShader(m_geomShader->clone());
  program.setFragmentShader(m_fragShader.clone());

  program.link();

  return program;
}

void RenderShaderProgram::loadShaders() const {
  Logger::debug("[RenderShaderProgram] Loading shaders...");

  m_vertShader.load();
  if (m_tessCtrlShader) m_tessCtrlShader->load();
  if (m_tessEvalShader) m_tessEvalShader->load();
  if (m_geomShader) m_geomShader->load();
  m_fragShader.load();

  Logger::debug("[RenderShaderProgram] Loaded shaders");
}

void RenderShaderProgram::compileShaders() const {
  Logger::debug("[RenderShaderProgram] Compiling shaders...");

  m_vertShader.compile();
  if (m_tessCtrlShader) m_tessCtrlShader->compile();
  if (m_tessEvalShader) m_tessEvalShader->compile();
  if (m_geomShader) m_geomShader->compile();
  m_fragShader.compile();

  Logger::debug("[RenderShaderProgram] Compiled shaders");
}

void RenderShaderProgram::destroyVertexShader() {
  Renderer::detachShader(m_index, m_vertShader.getIndex());
  m_vertShader.destroy();
}

void RenderShaderProgram::destroyTessellationControlShader() {
  if (!m_tessCtrlShader)
    return;

  Renderer::detachShader(m_index, m_tessCtrlShader->getIndex());
  m_tessCtrlShader->destroy();
  m_tessCtrlShader.reset();
}

void RenderShaderProgram::destroyTessellationEvaluationShader() {
  if (!m_tessEvalShader)
    return;

  Renderer::detachShader(m_index, m_tessEvalShader->getIndex());
  m_tessEvalShader->destroy();
  m_tessEvalShader.reset();
}

void RenderShaderProgram::destroyGeometryShader() {
  if (!m_geomShader)
    return;

  Renderer::detachShader(m_index, m_geomShader->getIndex());
  m_geomShader->destroy();
  m_geomShader.reset();
}

void RenderShaderProgram::destroyFragmentShader() {
  Renderer::detachShader(m_index, m_fragShader.getIndex());
  m_fragShader.destroy();
}

void ComputeShaderProgram::setShader(ComputeShader&& compShader) {
  Logger::debug("[ComputeShaderProgram] Setting shader (ID: " + std::to_string(compShader.getIndex()) + ", path: '" + compShader.getPath() + "')");

  if (Renderer::isShaderAttached(m_index, m_compShader.getIndex()))
    Renderer::detachShader(m_index, m_compShader.getIndex());

  m_compShader = std::move(compShader);
  m_compShader.compile();

  Renderer::attachShader(m_index, m_compShader.getIndex());

  link();
}

ComputeShaderProgram ComputeShaderProgram::clone() const {
  ComputeShaderProgram program;
  program.setShader(m_compShader.clone());
  return program;
}

void ComputeShaderProgram::loadShaders() const {
  Logger::debug("[ComputeShaderProgram] Loading shader...");
  m_compShader.load();
  Logger::debug("[ComputeShaderProgram] Loaded shader");
}

void ComputeShaderProgram::compileShaders() const {
  Logger::debug("[ComputeShaderProgram] Compiling shader...");
  m_compShader.compile();
  Logger::debug("[ComputeShaderProgram] Compiled shader");
}

void ComputeShaderProgram::execute(unsigned int groupCountX, unsigned int groupCountY, unsigned int groupCountZ) const {
  use();
  Renderer::dispatchCompute(groupCountX, groupCountY, groupCountZ);
  Renderer::setMemoryBarrier(BarrierType::ALL);
}

void ComputeShaderProgram::destroyShader() {
  Renderer::detachShader(m_index, m_compShader.getIndex());
  m_compShader.destroy();
}

} // namespace Raz
