#include "RaZ/Data/Image.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/Camera.hpp"
#include "RaZ/Render/GraphicObjects.hpp"
#include "RaZ/Render/Light.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerRenderTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Camera> camera = state.new_usertype<Camera>("Camera",
                                                              sol::constructors<Camera(),
                                                                                Camera(unsigned int, unsigned int),
                                                                                Camera(unsigned int, unsigned int, Radiansf),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float, float),
                                                                                Camera(unsigned int, unsigned int, Radiansf, float, float, ProjectionType)>(),
                                                              sol::base_classes, sol::bases<Component>());
    camera["fieldOfView"]                    = sol::property(&Camera::getFieldOfView, &Camera::setFieldOfView);
    camera["orthographicBound"]              = sol::property(&Camera::getOrthographicBound, &Camera::setOrthographicBound);
    camera["cameraType"]                     = sol::property(&Camera::getCameraType, &Camera::setCameraType);
    camera["getViewMatrix"]                  = &Camera::getViewMatrix;
    camera["getInverseViewMatrix"]           = &Camera::getInverseViewMatrix;
    camera["getProjectionMatrix"]            = &Camera::getProjectionMatrix;
    camera["getInverseProjectionMatrix"]     = &Camera::getInverseProjectionMatrix;
    camera["setProjectionType"]              = &Camera::setProjectionType;
    camera["setTarget"]                      = &Camera::setTarget;
    camera["computeViewMatrix"]              = &Camera::computeViewMatrix;
    camera["computeLookAt"]                  = &Camera::computeLookAt;
    camera["computeInverseViewMatrix"]       = &Camera::computeInverseViewMatrix;
    camera["computePerspectiveMatrix"]       = &Camera::computePerspectiveMatrix;
    camera["computeOrthographicMatrix"]      = sol::overload(PickOverload<float, float, float, float, float, float>(&Camera::computeOrthographicMatrix),
                                                             PickOverload<>(&Camera::computeOrthographicMatrix));
    camera["computeProjectionMatrix"]        = &Camera::computeProjectionMatrix;
    camera["computeInverseProjectionMatrix"] = &Camera::computeInverseProjectionMatrix;
    camera["resizeViewport"]                 = &Camera::resizeViewport;
    camera["unproject"]                      = sol::overload(PickOverload<const Vec4f&>(&Camera::unproject),
                                                             PickOverload<const Vec3f&>(&Camera::unproject),
                                                             PickOverload<const Vec2f&>(&Camera::unproject));

    state.new_enum<CameraType>("CameraType", {
      { "FREE_FLY", CameraType::FREE_FLY },
      { "LOOK_AT",  CameraType::LOOK_AT }
    });

    state.new_enum<ProjectionType>("ProjectionType", {
      { "PERSPECTIVE",  ProjectionType::PERSPECTIVE },
      { "ORTHOGRAPHIC", ProjectionType::ORTHOGRAPHIC }
    });
  }

  {
    sol::usertype<Cubemap> cubemap = state.new_usertype<Cubemap>("Cubemap",
                                                                 sol::constructors<Cubemap(),
                                                                                   Cubemap(const Image&, const Image&,
                                                                                           const Image&, const Image&,
                                                                                           const Image&, const Image&)>());
    cubemap["getIndex"]   = &Cubemap::getIndex;
    cubemap["getProgram"] = [] (const Cubemap& c) { return &c.getProgram(); };
    cubemap["load"]       = &Cubemap::load;
    cubemap["bind"]       = &Cubemap::bind;
    cubemap["unbind"]     = &Cubemap::unbind;
    cubemap["draw"]       = &Cubemap::draw;
  }

  {
    sol::usertype<Framebuffer> framebuffer = state.new_usertype<Framebuffer>("Framebuffer",
                                                                             sol::constructors<Framebuffer()>());
    framebuffer["getIndex"]            = &Framebuffer::getIndex;
    framebuffer["isEmpty"]             = &Framebuffer::isEmpty;
    framebuffer["hasDepthBuffer"]      = &Framebuffer::hasDepthBuffer;
    framebuffer["getDepthBuffer"]      = [] (const Framebuffer& f) { return &f.getDepthBuffer(); };
    framebuffer["getColorBufferCount"] = &Framebuffer::getColorBufferCount;
    framebuffer["getColorBuffer"]      = [] (const Framebuffer& f, std::size_t i) { return &f.getColorBuffer(i); };
    framebuffer["recoverVertexShader"] = &Framebuffer::recoverVertexShader;
    framebuffer["setDepthBuffer"]      = &Framebuffer::setDepthBuffer;
    framebuffer["addColorBuffer"]      = &Framebuffer::addColorBuffer;
    framebuffer["removeTextureBuffer"] = &Framebuffer::removeTextureBuffer;
    framebuffer["clearDepthBuffer"]    = &Framebuffer::clearDepthBuffer;
    framebuffer["clearColorBuffers"]   = &Framebuffer::clearColorBuffers;
    framebuffer["clearTextureBuffers"] = &Framebuffer::clearTextureBuffers;
    framebuffer["resizeBuffers"]       = &Framebuffer::resizeBuffers;
    framebuffer["mapBuffers"]          = &Framebuffer::mapBuffers;
    framebuffer["bind"]                = &Framebuffer::bind;
    framebuffer["unbind"]              = &Framebuffer::unbind;
    framebuffer["display"]             = &Framebuffer::display;
  }

  // GraphicObjects
  {
    {
      sol::usertype<IndexBuffer> indexBuffer = state.new_usertype<IndexBuffer>("IndexBuffer",
                                                                               sol::constructors<IndexBuffer()>());
      indexBuffer["getIndex"]           = &IndexBuffer::getIndex;
      indexBuffer["bind"]               = &IndexBuffer::bind;
      indexBuffer["unbind"]             = &IndexBuffer::unbind;
      indexBuffer["lineIndexCount"]     = &IndexBuffer::lineIndexCount;
      indexBuffer["triangleIndexCount"] = &IndexBuffer::triangleIndexCount;
    }

    {
      sol::usertype<VertexArray> vertexArray = state.new_usertype<VertexArray>("VertexArray",
                                                                               sol::constructors<VertexArray()>());
      vertexArray["getIndex"] = &VertexArray::getIndex;
      vertexArray["bind"]     = &VertexArray::bind;
      vertexArray["unbind"]   = &VertexArray::unbind;
    }

    {
      sol::usertype<VertexBuffer> vertexBuffer = state.new_usertype<VertexBuffer>("VertexBuffer",
                                                                                  sol::constructors<VertexBuffer()>());
      vertexBuffer["getIndex"]    = &VertexBuffer::getIndex;
      vertexBuffer["bind"]        = &VertexBuffer::bind;
      vertexBuffer["unbind"]      = &VertexBuffer::unbind;
      vertexBuffer["vertexCount"] = &VertexBuffer::vertexCount;
    }
  }

  {
    sol::usertype<Light> light = state.new_usertype<Light>("Light",
                                                           sol::constructors<Light(LightType, float),
                                                                             Light(LightType, float, const Color&),
                                                                             Light(LightType, const Vec3f&, float),
                                                                             Light(LightType, const Vec3f&, float, const Color&),
                                                                             Light(LightType, const Vec3f&, float, Radiansf),
                                                                             Light(LightType, const Vec3f&, float, Radiansf, const Color&)>(),
                                                           sol::base_classes, sol::bases<Component>());
    light["type"]      = sol::property(&Light::getType, &Light::setType);
    light["direction"] = sol::property(&Light::getDirection, &Light::setDirection);
    light["energy"]    = sol::property(&Light::getEnergy, &Light::setEnergy);
    light["color"]     = sol::property(&Light::getColor, &Light::setColor);
    light["angle"]     = sol::property(&Light::getAngle, &Light::setAngle);

    state.new_enum<LightType>("LightType", {
      { "POINT",       LightType::POINT },
      { "DIRECTIONAL", LightType::DIRECTIONAL },
      { "SPOT",        LightType::SPOT }
    });
  }

  {
    sol::usertype<Material> material = state.new_usertype<Material>("Material",
                                                                    sol::constructors<Material(),
                                                                                      Material(MaterialType)>());
    material["getProgram"] = PickNonConstOverload<>(&Material::getProgram);
    material["isEmpty"]    = &Material::isEmpty;
    material["clone"]      = &Material::clone;
    material["loadType"]   = &Material::loadType;

    state.new_enum<std::string>("MaterialAttribute", {
      { "BaseColor",    MaterialAttribute::BaseColor },
      { "Emissive",     MaterialAttribute::Emissive },
      { "Metallic",     MaterialAttribute::Metallic },
      { "Roughness",    MaterialAttribute::Roughness },
      { "Ambient",      MaterialAttribute::Ambient },
      { "Specular",     MaterialAttribute::Specular },
      { "Transparency", MaterialAttribute::Transparency }
    });

    state.new_enum<std::string>("MaterialTexture", {
      { "BaseColor",    MaterialTexture::BaseColor },
      { "Emissive",     MaterialTexture::Emissive },
      { "Ambient",      MaterialTexture::Ambient },
      { "Normal",       MaterialTexture::Normal },
      { "Metallic",     MaterialTexture::Metallic },
      { "Roughness",    MaterialTexture::Roughness },
      { "Specular",     MaterialTexture::Specular },
      { "Transparency", MaterialTexture::Transparency },
      { "Bump",         MaterialTexture::Bump }
    });

    state.new_enum<MaterialType>("MaterialType", {
      { "COOK_TORRANCE",     MaterialType::COOK_TORRANCE },
      { "BLINN_PHONG",       MaterialType::BLINN_PHONG },
      { "SINGLE_TEXTURE_2D", MaterialType::SINGLE_TEXTURE_2D },
      { "SINGLE_TEXTURE_3D", MaterialType::SINGLE_TEXTURE_3D }
    });
  }

  {
    sol::usertype<UniformBuffer> uniformBuffer = state.new_usertype<UniformBuffer>("UniformBuffer",
                                                                                   sol::constructors<UniformBuffer(unsigned int),
                                                                                                     UniformBuffer(unsigned int, UniformBufferUsage)>());
    uniformBuffer["getIndex"]         = &UniformBuffer::getIndex;
    uniformBuffer["bindUniformBlock"] = sol::overload(PickOverload<const ShaderProgram&, unsigned int, unsigned int>(&UniformBuffer::bindUniformBlock),
                                                      PickOverload<const ShaderProgram&, const std::string&, unsigned int>(&UniformBuffer::bindUniformBlock));
    uniformBuffer["bindBase"]         = &UniformBuffer::bindBase;
    uniformBuffer["bindRange"]        = &UniformBuffer::bindRange;
    uniformBuffer["bind"]             = &UniformBuffer::bind;
    uniformBuffer["unbind"]           = &UniformBuffer::unbind;
    uniformBuffer["sendIntData"]      = &UniformBuffer::sendData<int>;
    uniformBuffer["sendUintData"]     = &UniformBuffer::sendData<unsigned int>;
    uniformBuffer["sendFloatData"]    = &UniformBuffer::sendData<float>;
    uniformBuffer["sendData"]         = sol::overload(&UniformBuffer::sendData<const Vec2i&>,
                                                      &UniformBuffer::sendData<const Vec3i&>,
                                                      &UniformBuffer::sendData<const Vec4i&>,
                                                      &UniformBuffer::sendData<const Vec2u&>,
                                                      &UniformBuffer::sendData<const Vec3u&>,
                                                      &UniformBuffer::sendData<const Vec4u&>,
                                                      &UniformBuffer::sendData<const Vec2f&>,
                                                      &UniformBuffer::sendData<const Vec3f&>,
                                                      &UniformBuffer::sendData<const Vec4f&>,
                                                      &UniformBuffer::sendData<const Mat2f&>,
                                                      &UniformBuffer::sendData<const Mat3f&>,
                                                      &UniformBuffer::sendData<const Mat4f&>);

    state.new_enum<UniformBufferUsage>("UniformBufferUsage", {
      { "STATIC",  UniformBufferUsage::STATIC },
      { "DYNAMIC", UniformBufferUsage::DYNAMIC },
      { "STREAM",  UniformBufferUsage::STREAM }
    });
  }
}

} // namespace Raz
