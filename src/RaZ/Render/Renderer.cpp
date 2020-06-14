#include "GL/glew.h"
#include "RaZ/Render/Renderer.hpp"

#include <array>
#include <cassert>
#include <iostream>

namespace Raz {

namespace {

#ifdef RAZ_USE_GL4
inline void GLAPIENTRY callbackDebugLog(GLenum source,
                                        GLenum type,
                                        unsigned int id,
                                        GLenum severity,
                                        int /* length */,
                                        const char* message,
                                        const void* /* userParam */) {
  std::cerr << "OpenGL Debug - ";

  switch (source) {
    case GL_DEBUG_SOURCE_API:             std::cerr << "Source: OpenGL\t"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Windows\t"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader compiler\t"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third party\t"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application\t"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other\t"; break;
    default: break;
  }

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error\t"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated behavior\t"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined behavior\t"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability\t"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance\t"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other\t"; break;
    default: break;
  }

  std::cerr << "ID: " << id << "\t";

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:   std::cerr << "Severity: High\t"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Severity: Medium\t"; break;
    case GL_DEBUG_SEVERITY_LOW:    std::cerr << "Severity: Low\t"; break;
    default: break;
  }

  std::cerr << "Message: " << message << std::endl;
}
#endif

inline constexpr const char* recoverGlErrorStr(unsigned int errorCode) {
  switch (errorCode) {
    case GL_INVALID_ENUM:                  return "Unrecognized error code (Invalid enum)";
    case GL_INVALID_VALUE:                 return "Numeric argument out of range (Invalid value)";
    case GL_INVALID_OPERATION:             return "Operation illegal in current state (Invalid operation)";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Framebuffer object is incomplete (Invalid framebuffer operation)";
    case GL_STACK_OVERFLOW:                return "Stack overflow";
    case GL_STACK_UNDERFLOW:               return "Stack underflow";
    case GL_OUT_OF_MEMORY:                 return "Not enough memory left (Out of memory)";
#ifdef RAZ_USE_GL4
    case GL_CONTEXT_LOST:                  return "OpenGL context has been lost due to a graphics card reset (Context lost)";
#endif
    case GL_NO_ERROR:                      return "No error";
    default:                               return "Unknown error";
  }
}

} // namespace

void Renderer::initialize() {
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    std::cerr << "Error: Failed to initialize GLEW." << std::endl;
  } else {
    s_isInitialized = true;

#if !defined(RAZ_PLATFORM_MAC) && defined(RAZ_USE_GL4) // Setting the debug message callback provokes a crash on macOS
    glDebugMessageCallback(&callbackDebugLog, nullptr);
    enable(Capability::DEBUG_OUTPUT_SYNCHRONOUS);
#endif
  }
}

void Renderer::enable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glEnable(static_cast<unsigned int>(capability));

  printConditionalErrors();
}

void Renderer::disable(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDisable(static_cast<unsigned int>(capability));

  printConditionalErrors();
}

bool Renderer::isEnabled(Capability capability) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isEnabled = (glIsEnabled(static_cast<unsigned int>(capability)) == GL_TRUE);

  printConditionalErrors();

  return isEnabled;
}

void Renderer::getParameter(StateParameter parameter, unsigned char* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetBooleanv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetIntegerv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, int64_t* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetInteger64v(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetFloatv(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, double* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetDoublev(static_cast<unsigned int>(parameter), values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, unsigned char* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetBooleani_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetIntegeri_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

void Renderer::getParameter(StateParameter parameter, unsigned int index, int64_t* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetInteger64i_v(static_cast<unsigned int>(parameter), index, values);

  printConditionalErrors();
}

unsigned int Renderer::getActiveTexture() {
  int texture {};
  getParameter(StateParameter::ACTIVE_TEXTURE, &texture);

  return static_cast<unsigned int>(texture - GL_TEXTURE0);
}

unsigned int Renderer::getCurrentProgram() {
  int program {};
  getParameter(StateParameter::CURRENT_PROGRAM, &program);

  return static_cast<unsigned int>(program);
}

void Renderer::clearColor(float red, float green, float blue, float alpha) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClearColor(red, green, blue, alpha);

  printConditionalErrors();
}

void Renderer::clear(MaskType mask) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glClear(static_cast<unsigned int>(mask));

  printConditionalErrors();
}

void Renderer::setDepthFunction(DepthFunction func) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDepthFunc(static_cast<unsigned int>(func));

  printConditionalErrors();
}

void Renderer::setFaceCulling(FaceOrientation orientation) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glCullFace(static_cast<unsigned int>(orientation));

  printConditionalErrors();
}

#if !defined(USE_OPENGL_ES)
void Renderer::setPolygonMode(FaceOrientation orientation, PolygonMode mode) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glPolygonMode(static_cast<unsigned int>(orientation), static_cast<unsigned int>(mode));

  printConditionalErrors();
}
#endif

void Renderer::setPixelStorage(PixelStorage storage, unsigned int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glPixelStorei(static_cast<unsigned int>(storage), static_cast<int>(value));

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = Renderer::recoverErrors();

  if (errorCodes[ErrorCode::INVALID_VALUE])
    std::cerr << "Renderer::setPixelStorage - " << value << " is not a valid alignment value. Only 1, 2, 4 & 8 are accepted.\n";

  std::cerr << std::flush;
#endif
}

void Renderer::recoverFrame(unsigned int width, unsigned int height, TextureFormat format, TextureDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glReadPixels(0, 0, static_cast<int>(width), static_cast<int>(height), static_cast<unsigned int>(format), static_cast<unsigned int>(dataType), data);

  printConditionalErrors();
}

void Renderer::generateBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenBuffers(count, indices);

  printConditionalErrors();
}

void Renderer::bindBuffer(BufferType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBuffer(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

void Renderer::bindBufferBase(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferBase(static_cast<unsigned int>(type), bindingIndex, bufferIndex);

  printConditionalErrors();
}

void Renderer::bindBufferRange(BufferType type, unsigned int bindingIndex, unsigned int bufferIndex, std::ptrdiff_t offset, std::ptrdiff_t size) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindBufferRange(static_cast<unsigned int>(type), bindingIndex, bufferIndex, offset, size);

  printConditionalErrors();
}

void Renderer::sendBufferData(BufferType type, std::ptrdiff_t size, const void* data, BufferDataUsage usage) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferData(static_cast<unsigned int>(type), size, data, static_cast<unsigned int>(usage));

  printConditionalErrors();
}

void Renderer::sendBufferSubData(BufferType type, std::ptrdiff_t offset, std::ptrdiff_t dataSize, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBufferSubData(static_cast<unsigned int>(type), offset, dataSize, data);

  printConditionalErrors();
}

void Renderer::deleteBuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteBuffers(count, indices);

  printConditionalErrors();
}

void Renderer::generateTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenTextures(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::activateTexture(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glActiveTexture(GL_TEXTURE0 + index);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteri(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterf(static_cast<unsigned int>(type), static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameteriv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(TextureType type, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexParameterfv(static_cast<unsigned int>(type), static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

#ifdef RAZ_USE_GL4
void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteri(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterf(textureIndex, static_cast<unsigned int>(param), value);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameteriv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}

void Renderer::setTextureParameter(unsigned int textureIndex, TextureParam param, const float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTextureParameterfv(textureIndex, static_cast<unsigned int>(param), values);

  printConditionalErrors();
}
#endif

void Renderer::sendImageData2D(TextureType type,
                               unsigned int mipmapLevel,
                               TextureInternalFormat internalFormat,
                               unsigned int width, unsigned int height,
                               TextureFormat format,
                               TextureDataType dataType, const void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glTexImage2D(static_cast<unsigned int>(type),
               static_cast<int>(mipmapLevel),
               static_cast<int>(internalFormat),
               static_cast<int>(width),
               static_cast<int>(height),
               0,
               static_cast<unsigned int>(format),
               static_cast<unsigned int>(dataType),
               data);

  printConditionalErrors();
}

void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, int* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameteriv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

  printConditionalErrors();
}

void Renderer::recoverTextureAttribute(TextureType type, unsigned int mipmapLevel, TextureAttribute attribute, float* values) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexLevelParameterfv(static_cast<unsigned int>(type), static_cast<int>(mipmapLevel), static_cast<unsigned int>(attribute), values);

  printConditionalErrors();
}

int Renderer::recoverTextureWidth(TextureType type, unsigned int mipmapLevel) {
  int width {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::WIDTH, &width);

  return width;
}

int Renderer::recoverTextureHeight(TextureType type, unsigned int mipmapLevel) {
  int height {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::HEIGHT, &height);

  return height;
}

int Renderer::recoverTextureDepth(TextureType type, unsigned int mipmapLevel) {
  int depth {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::DEPTH, &depth);

  return depth;
}

TextureInternalFormat Renderer::recoverTextureInternalFormat(TextureType type, unsigned int mipmapLevel) {
  int format {};
  recoverTextureAttribute(type, mipmapLevel, TextureAttribute::INTERNAL_FORMAT, &format);

  return static_cast<TextureInternalFormat>(format);
}

void Renderer::recoverTextureData(TextureType type, unsigned int mipmapLevel, TextureFormat format, TextureDataType dataType, void* data) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetTexImage(static_cast<unsigned int>(type),
                static_cast<int>(mipmapLevel),
                static_cast<unsigned int>(format),
                static_cast<unsigned int>(dataType),
                data);

  printConditionalErrors();
}

void Renderer::generateMipmap(TextureType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateMipmap(static_cast<unsigned int>(type));

  printConditionalErrors();
}

#ifdef RAZ_USE_GL4
void Renderer::generateMipmap(unsigned int textureIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenerateTextureMipmap(textureIndex);

  printConditionalErrors();
}
#endif

void Renderer::bindTexture(TextureType type, unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindTexture(static_cast<unsigned int>(type), index);

  printConditionalErrors();
}

void Renderer::deleteTextures(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteTextures(static_cast<int>(count), indices);

  printConditionalErrors();
}

void Renderer::resizeViewport(int xOrigin, int yOrigin, unsigned int width, unsigned int height) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glViewport(xOrigin, yOrigin, static_cast<int>(width), static_cast<int>(height));

  printConditionalErrors();
}

unsigned int Renderer::createProgram() {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int programIndex = glCreateProgram();

  printConditionalErrors();

  return programIndex;
}

void Renderer::getProgramParameter(unsigned int index, ProgramParameter parameter, int* parameters) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGetProgramiv(index, static_cast<unsigned int>(parameter), parameters);

  printConditionalErrors();
}

bool Renderer::isProgramLinked(unsigned int index) {
  int linkStatus {};
  getProgramParameter(index, ProgramParameter::LINK_STATUS, &linkStatus);

  return (linkStatus == GL_TRUE);
}

unsigned int Renderer::recoverActiveUniformCount(unsigned int index) {
  int uniformCount {};
  getProgramParameter(index, ProgramParameter::ACTIVE_UNIFORMS, &uniformCount);

  return static_cast<unsigned int>(uniformCount);
}

void Renderer::linkProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glLinkProgram(index);

  if (!isProgramLinked(index)) {
    char infoLog[512];

    glGetProgramInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    std::cerr << "Error: Shader program link failed (ID " << index << ").\n" << infoLog << std::endl;
  }

  printConditionalErrors();
}

void Renderer::useProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUseProgram(index);

  printConditionalErrors();
}

void Renderer::deleteProgram(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteProgram(index);

  printConditionalErrors();
}

unsigned int Renderer::createShader(ShaderType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int shaderIndex = glCreateShader(static_cast<unsigned int>(type));

  printConditionalErrors();

  return shaderIndex;
}

int Renderer::getShaderStatus(unsigned int index, ShaderStatus status) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int res {};
  glGetShaderiv(index, static_cast<unsigned int>(status), &res);

  printConditionalErrors();

  return res;
}

bool Renderer::isShaderCompiled(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const bool isCompiled = (getShaderStatus(index, ShaderStatus::COMPILE) == GL_TRUE);

  printConditionalErrors();

  return isCompiled;
}

void Renderer::sendShaderSource(unsigned int index, const char* source, int length) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glShaderSource(index, 1, &source, &length);

  printConditionalErrors();
}

void Renderer::compileShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glCompileShader(index);

  if (!isShaderCompiled(index)) {
    char infoLog[512];

    glGetShaderInfoLog(index, static_cast<int>(std::size(infoLog)), nullptr, infoLog);
    std::cerr << "Error: Shader compilation failed (ID " << index << ").\n" << infoLog << std::endl;
  }

  printConditionalErrors();
}

void Renderer::attachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glAttachShader(programIndex, shaderIndex);

  printConditionalErrors();
}

void Renderer::detachShader(unsigned int programIndex, unsigned int shaderIndex) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDetachShader(programIndex, shaderIndex);

  printConditionalErrors();
}

void Renderer::deleteShader(unsigned int index) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteShader(index);

  printConditionalErrors();
}

int Renderer::recoverUniformLocation(unsigned int programIndex, const char* uniformName) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const int location = glGetUniformLocation(programIndex, uniformName);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  printErrors();

  if (location == -1)
    std::cerr << "Warning: Uniform '" << uniformName << "' unrecognized." << std::endl;
#endif

  return location;
}

void Renderer::recoverUniformInfo(unsigned int programIndex, unsigned int uniformIndex, UniformType& type, std::string& name, int* size) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  int nameLength {};
  int uniformSize {};
  unsigned int uniformType {};
  std::array<char, 256> uniformName {};

  glGetActiveUniform(programIndex, uniformIndex, static_cast<int>(uniformName.size()), &nameLength, &uniformSize, &uniformType, uniformName.data());

  type = static_cast<UniformType>(uniformType);

  name.resize(static_cast<std::size_t>(nameLength));
  std::copy(uniformName.cbegin(), uniformName.cbegin() + nameLength, name.begin());

  if (size)
    *size = uniformSize;

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes.isEmpty())
    return;

  std::cerr << "Renderer::recoverUniformInfo - ";

  if (errorCodes[ErrorCode::INVALID_OPERATION])
    std::cerr << "Tried to fetch program information from a non-program object.\n";

  if (errorCodes[ErrorCode::INVALID_VALUE]) {
    const unsigned int uniCount = recoverActiveUniformCount(programIndex);

    if (uniformIndex >= uniCount)
      std::cerr << "The given uniform index (" << uniformIndex << ") is greater than or equal to the program's active uniform count (" << uniCount << ").\n";
    else
      std::cerr << "The given program index has not been created by OpenGL.\n";
  }

  std::cerr << std::flush;
#endif
}

UniformType Renderer::recoverUniformType(unsigned int programIndex, unsigned int uniformIndex) {
  UniformType type {};
  std::string name;
  recoverUniformInfo(programIndex, uniformIndex, type, name);

  return type;
}

std::string Renderer::recoverUniformName(unsigned int programIndex, unsigned int uniformIndex) {
  UniformType type {};
  std::string name;
  recoverUniformInfo(programIndex, uniformIndex, type, name);

  return name;
}

void Renderer::sendUniform(int uniformIndex, int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1i(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniform(int uniformIndex, unsigned int value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1ui(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniform(int uniformIndex, float value) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1f(uniformIndex, value);

  printConditionalErrors();
}

void Renderer::sendUniformVector1(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform1fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector2(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform2fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector3(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform3fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformVector4(int uniformIndex, const float* values, int count) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniform4fv(uniformIndex, count, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix2x2(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix2fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix3x3(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix3fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::sendUniformMatrix4x4(int uniformIndex, const float* values, int count, bool transpose) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glUniformMatrix4fv(uniformIndex, count, transpose, values);

  printConditionalErrors();
}

void Renderer::generateFramebuffers(int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glGenFramebuffers(count, indices);

  printConditionalErrors();
}

FramebufferStatus Renderer::getFramebufferStatus(FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  const unsigned int status = glCheckFramebufferStatus(static_cast<unsigned int>(type));

  printConditionalErrors();

  return static_cast<FramebufferStatus>(status);
}

void Renderer::setFramebufferTexture2D(FramebufferAttachment attachment,
                                       TextureType textureType, unsigned int textureIndex, int mipmapLevel,
                                       FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glFramebufferTexture2D(static_cast<unsigned int>(type),
                         static_cast<unsigned int>(attachment),
                         static_cast<unsigned int>(textureType),
                         textureIndex,
                         mipmapLevel);

  printConditionalErrors();
}

void Renderer::setDrawBuffers(unsigned int count, const DrawBuffer* buffers) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDrawBuffers(count, reinterpret_cast<const unsigned int*>(buffers));

  printConditionalErrors();
}

void Renderer::bindFramebuffer(unsigned int index, FramebufferType type) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glBindFramebuffer(static_cast<unsigned int>(type), index);

#if !defined(NDEBUG) && !defined(SKIP_RENDERER_ERRORS)
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes[ErrorCode::INVALID_OPERATION])
    std::cerr << "Renderer::bindFramebuffer - Bound object is not a valid framebuffer.\n";

  std::cerr << std::flush;
#endif
}

void Renderer::deleteFramebuffers(unsigned int count, unsigned int* indices) {
  assert("Error: The Renderer must be initialized before calling its functions." && isInitialized());

  glDeleteFramebuffers(count, indices);

  printConditionalErrors();
}

ErrorCodes Renderer::recoverErrors() {
  static constexpr auto recoverErrorCodeIndex = [] (ErrorCode code) -> uint8_t {
    return (static_cast<uint8_t>(static_cast<unsigned int>(code) - static_cast<unsigned int>(ErrorCode::INVALID_ENUM)));
  };

  ErrorCodes errorCodes;

  while (true) {
    const unsigned int errorCode = glGetError();

    if (errorCode == GL_NO_ERROR)
      break;

    const uint8_t errorCodeIndex = recoverErrorCodeIndex(static_cast<ErrorCode>(errorCode));

    // An error code cannot be returned twice in a row; if it is, the error checking should be stopped
    if (errorCodes.codes[errorCodeIndex])
      break;

    errorCodes.codes[errorCodeIndex] = true;
  }

  return errorCodes;
}

void Renderer::printErrors() {
  const ErrorCodes errorCodes = recoverErrors();

  if (errorCodes.isEmpty())
    return;

  for (uint8_t errorIndex = 0; errorIndex < static_cast<uint8_t>(errorCodes.codes.size()); ++errorIndex) {
    if (errorCodes.codes[errorIndex]) {
      const unsigned int errorValue = errorIndex + static_cast<unsigned int>(ErrorCode::INVALID_ENUM);
      std::cerr << "OpenGL error - " << recoverGlErrorStr(errorValue) << " (code " << errorValue << ")\n";
    }
  }

  std::cerr << std::flush;
}

} // namespace Raz
