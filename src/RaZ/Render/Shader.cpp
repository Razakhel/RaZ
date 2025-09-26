#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include "tracy/Tracy.hpp"

namespace Raz {

void Shader::import(FilePath filePath) {
  m_path = std::move(filePath);
  load();
}

void Shader::load() const {
  if (m_path.isEmpty()) // Shader imported directly from source, no path available
    return;

  Logger::debug("[Shader] Loading (ID: {}, path: '{}')...", m_index.get(), m_path);
  loadSource(FileUtils::readFileToString(m_path));
  Logger::debug("[Shader] Loaded");
}

void Shader::compile() const {
  Logger::debug("[Shader] Compiling (ID: {})...", m_index.get());
  Renderer::compileShader(m_index);
  Logger::debug("[Shader] Compiled");
}

bool Shader::isCompiled() const noexcept {
  return Renderer::isShaderCompiled(m_index);
}

void Shader::loadSource(const std::string& source) const {
  ZoneScopedN("Shader::loadSource");

  Logger::debug("[Shader] Loading source (ID: {})...", m_index.get());

  // Removing spaces in front so that we can directly check the header tags
  std::string shaderSource = StrUtils::trimLeftCopy(source);

  // If the #version tag is missing, add it with the current version
  if (!StrUtils::startsWith(shaderSource, "#version")) {
    std::string header = std::format("#version {}{}0", Renderer::getMajorVersion(), Renderer::getMinorVersion());

#if defined(USE_OPENGL_ES)
    header += " es";

    if (!StrUtils::startsWith(shaderSource, "precision"))
      header += "\nprecision highp float;\nprecision highp int;";
#endif

    shaderSource = header + "\n#line 0\n" + shaderSource;
  }

  Renderer::sendShaderSource(m_index, shaderSource);

  Logger::debug("[Shader] Loaded source");
}

void Shader::destroy() {
  ZoneScopedN("Shader::destroy");

  if (!m_index.isValid())
    return;

  Logger::debug("[Shader] Destroying (ID: {})...", m_index.get());
  Renderer::deleteShader(m_index);
  m_index.reset();
  Logger::debug("[Shader] Destroyed");
}

VertexShader::VertexShader() {
  Logger::debug("[Shader] Creating vertex shader...");
  m_index = Renderer::createShader(ShaderType::VERTEX);
  Logger::debug("[Shader] Created vertex shader (ID: {})", m_index.get());
}

VertexShader VertexShader::loadFromSource(const std::string& source) {
  VertexShader vertShader;
  vertShader.loadSource(source);
  return vertShader;
}

VertexShader VertexShader::clone() const {
  VertexShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}

#if !defined(USE_OPENGL_ES)
TessellationControlShader::TessellationControlShader() {
  Logger::debug("[Shader] Creating tessellation control shader...");
  m_index = Renderer::createShader(ShaderType::TESSELLATION_CONTROL);
  Logger::debug("[Shader] Created tessellation control shader (ID: {})", m_index.get());
}

TessellationControlShader TessellationControlShader::loadFromSource(const std::string& source) {
  TessellationControlShader tessCtrlShader;
  tessCtrlShader.loadSource(source);
  return tessCtrlShader;
}

TessellationControlShader TessellationControlShader::clone() const {
  TessellationControlShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}

TessellationEvaluationShader::TessellationEvaluationShader() {
  Logger::debug("[Shader] Creating tessellation evaluation shader...");
  m_index = Renderer::createShader(ShaderType::TESSELLATION_EVALUATION);
  Logger::debug("[Shader] Created tessellation evaluation shader (ID: {})", m_index.get());
}

TessellationEvaluationShader TessellationEvaluationShader::loadFromSource(const std::string& source) {
  TessellationEvaluationShader tessEvalShader;
  tessEvalShader.loadSource(source);
  return tessEvalShader;
}

TessellationEvaluationShader TessellationEvaluationShader::clone() const {
  TessellationEvaluationShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}

GeometryShader::GeometryShader() {
  Logger::debug("[Shader] Creating geometry shader...");
  m_index = Renderer::createShader(ShaderType::GEOMETRY);
  Logger::debug("[Shader] Created geometry shader (ID: {})", m_index.get());
}

GeometryShader GeometryShader::loadFromSource(const std::string& source) {
  GeometryShader geomShader;
  geomShader.loadSource(source);
  return geomShader;
}

GeometryShader GeometryShader::clone() const {
  GeometryShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}
#endif

FragmentShader::FragmentShader() {
  Logger::debug("[Shader] Creating fragment shader...");
  m_index = Renderer::createShader(ShaderType::FRAGMENT);
  Logger::debug("[Shader] Created fragment shader (ID: {})", m_index.get());
}

FragmentShader FragmentShader::loadFromSource(const std::string& source) {
  FragmentShader fragShader;
  fragShader.loadSource(source);
  return fragShader;
}

FragmentShader FragmentShader::clone() const {
  FragmentShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}

#if !defined(USE_WEBGL)
ComputeShader::ComputeShader() {
  Logger::debug("[Shader] Creating compute shader...");
  m_index = Renderer::createShader(ShaderType::COMPUTE);
  Logger::debug("[Shader] Created compute shader (ID: {})", m_index.get());
}

ComputeShader ComputeShader::loadFromSource(const std::string& source) {
  ComputeShader compShader;
  compShader.loadSource(source);
  return compShader;
}

ComputeShader ComputeShader::clone() const {
  ComputeShader res;

  if (!m_path.isEmpty())
    res.import(m_path);
  else
    res.loadSource(Renderer::recoverShaderSource(m_index));

  return res;
}
#endif

} // namespace Raz
