#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Shader.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <limits>

namespace Raz {

Shader::Shader(Shader&& shader) noexcept
  : m_index{ std::exchange(shader.m_index, std::numeric_limits<unsigned int>::max()) }, m_path{ std::move(shader.m_path) } {}

bool Shader::isValid() const noexcept {
  return (m_index != std::numeric_limits<unsigned int>::max());
}

void Shader::import(FilePath filePath) {
  m_path = std::move(filePath);
  load();
}

void Shader::load() const {
  if (m_path.getPath().empty()) // Shader imported directly from source, no path available
    return;

  Logger::debug("[Shader] Loading (ID: " + std::to_string(m_index) + ", path: '" + m_path + "')...");

  loadSource(FileUtils::readFile(m_path));

  Logger::debug("[Shader] Loaded");
}

void Shader::compile() const {
  Logger::debug("[Shader] Compiling (ID: " + std::to_string(m_index) + ")...");
  Renderer::compileShader(m_index);
  Logger::debug("[Shader] Compiled");
}

bool Shader::isCompiled() const noexcept {
  return Renderer::isShaderCompiled(m_index);
}

void Shader::loadSource(const std::string& source) const {
  Logger::debug("[Shader] Loading source (ID: " + std::to_string(m_index) + ")...");

  // Removing spaces in front so that we can directly check the header tags
  std::string shaderSource = StrUtils::trimLeftCopy(source);

  // If the #version tag is missing, add it with the current version
  if (!StrUtils::startsWith(shaderSource, "#version")) {
    std::string header = "#version " + std::to_string(Renderer::getMajorVersion()) + std::to_string(Renderer::getMinorVersion()) + '0';

#if defined(USE_OPENGL_ES)
    header += " es";

    if (!StrUtils::startsWith(shaderSource, "precision"))
      header += "\nprecision highp float;\nprecision highp int;";
#endif

    shaderSource = header + '\n' + shaderSource;
  }

  Renderer::sendShaderSource(m_index, shaderSource);

  Logger::debug("[Shader] Loaded source");
}

void Shader::destroy() {
  if (!isValid())
    return;

  Logger::debug("[Shader] Destroying (ID: " + std::to_string(m_index) + ")...");
  Renderer::deleteShader(m_index);
  m_index = std::numeric_limits<unsigned int>::max();
  Logger::debug("[Shader] Destroyed");
}

Shader& Shader::operator=(Shader&& shader) noexcept {
  std::swap(m_index, shader.m_index);
  m_path = std::move(shader.m_path);

  return *this;
}

VertexShader::VertexShader() {
  Logger::debug("[Shader] Creating vertex shader...");
  m_index = Renderer::createShader(ShaderType::VERTEX);
  Logger::debug("[Shader] Created vertex shader (ID: " + std::to_string(m_index) + ')');
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

TessellationControlShader::TessellationControlShader() {
  Logger::debug("[Shader] Creating tessellation control shader...");
  m_index = Renderer::createShader(ShaderType::TESSELLATION_CONTROL);
  Logger::debug("[Shader] Created tessellation control shader (ID: " + std::to_string(m_index) + ')');
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
  Logger::debug("[Shader] Created tessellation evaluation shader (ID: " + std::to_string(m_index) + ')');
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
  Logger::debug("[Shader] Created geometry shader (ID: " + std::to_string(m_index) + ')');
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

FragmentShader::FragmentShader() {
  Logger::debug("[Shader] Creating fragment shader...");
  m_index = Renderer::createShader(ShaderType::FRAGMENT);
  Logger::debug("[Shader] Created fragment shader (ID: " + std::to_string(m_index) + ')');
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

ComputeShader::ComputeShader() {
  Logger::debug("[Shader] Creating compute shader...");
  m_index = Renderer::createShader(ShaderType::COMPUTE);
  Logger::debug("[Shader] Created compute shader (ID: " + std::to_string(m_index) + ')');
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

} // namespace Raz
