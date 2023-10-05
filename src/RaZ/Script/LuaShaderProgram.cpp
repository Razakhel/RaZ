#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerShaderProgramTypes() {
  sol::state& state = getState();

#if !defined(USE_WEBGL)
  {
    sol::usertype<ComputeShaderProgram> computeShaderProgram = state.new_usertype<ComputeShaderProgram>("ComputeShaderProgram",
                                                                                                        sol::constructors<ComputeShaderProgram()>(),
                                                                                                        sol::base_classes, sol::bases<ShaderProgram>());
    computeShaderProgram["getShader"]     = [] (const ComputeShaderProgram& p) { return &p.getShader(); };
    computeShaderProgram["setShader"]     = [] (ComputeShaderProgram& p, ComputeShader& s) { p.setShader(std::move(s)); };
    computeShaderProgram["clone"]         = &ComputeShaderProgram::clone;
    computeShaderProgram["execute"]       = sol::overload([] (const ComputeShaderProgram& p, unsigned int x) { p.execute(x); },
                                                          [] (const ComputeShaderProgram& p, unsigned int x, unsigned int y) { p.execute(x, y); },
                                                          PickOverload<unsigned int, unsigned int, unsigned int>(&ComputeShaderProgram::execute));
    computeShaderProgram["destroyShader"] = &ComputeShaderProgram::destroyShader;
  }
#endif

  {
    sol::usertype<RenderShaderProgram> renderShaderProgram = state.new_usertype<RenderShaderProgram>("RenderShaderProgram",
                                                                                                     sol::constructors<RenderShaderProgram()>(),
                                                                                                     sol::base_classes, sol::bases<ShaderProgram>());
    renderShaderProgram["getVertexShader"]                     = [] (const RenderShaderProgram& p) { return &p.getVertexShader(); };
#if !defined(USE_OPENGL_ES)
    renderShaderProgram["hasTessellationControlShader"]        = &RenderShaderProgram::hasTessellationControlShader;
    renderShaderProgram["getTessellationControlShader"]        = [] (const RenderShaderProgram& p) { return &p.getTessellationControlShader(); };
    renderShaderProgram["hasTessellationEvaluationShader"]     = &RenderShaderProgram::hasTessellationEvaluationShader;
    renderShaderProgram["getTessellationEvaluationShader"]     = [] (const RenderShaderProgram& p) { return &p.getTessellationEvaluationShader(); };
    renderShaderProgram["hasGeometryShader"]                   = &RenderShaderProgram::hasGeometryShader;
    renderShaderProgram["getGeometryShader"]                   = [] (const RenderShaderProgram& p) { return &p.getGeometryShader(); };
#endif
    renderShaderProgram["getFragmentShader"]                   = [] (const RenderShaderProgram& p) { return &p.getFragmentShader(); };
    renderShaderProgram["setVertexShader"]                     = [] (RenderShaderProgram& p, VertexShader& s) { p.setVertexShader(std::move(s)); };
#if !defined(USE_OPENGL_ES)
    renderShaderProgram["setTessellationControlShader"]        = [] (RenderShaderProgram& p,
                                                                     TessellationControlShader& s) { p.setTessellationControlShader(std::move(s)); };
    renderShaderProgram["setTessellationEvaluationShader"]     = [] (RenderShaderProgram& p,
                                                                     TessellationEvaluationShader& s) { p.setTessellationEvaluationShader(std::move(s)); };
    renderShaderProgram["setGeometryShader"]                   = [] (RenderShaderProgram& p, GeometryShader& s) { p.setGeometryShader(std::move(s)); };
#endif
    renderShaderProgram["setFragmentShader"]                   = [] (RenderShaderProgram& p, FragmentShader& s) { p.setFragmentShader(std::move(s)); };
    renderShaderProgram["setShaders"]                          = sol::overload([] (RenderShaderProgram& p, VertexShader& v,
                                                                                   FragmentShader& f) { p.setShaders(std::move(v), std::move(f)); }
#if !defined(USE_OPENGL_ES)
                                                                               ,
                                                                               [] (RenderShaderProgram& p, VertexShader& v, GeometryShader& g,
                                                                                   FragmentShader& f) { p.setShaders(std::move(v), std::move(g),
                                                                                                                     std::move(f)); },
                                                                               [] (RenderShaderProgram& p, VertexShader& v, TessellationEvaluationShader& te,
                                                                                   FragmentShader& f) { p.setShaders(std::move(v), std::move(te),
                                                                                                                     std::move(f)); },
                                                                               [] (RenderShaderProgram& p, VertexShader& v, TessellationControlShader& tc,
                                                                                   TessellationEvaluationShader& te,
                                                                                   FragmentShader& f) { p.setShaders(std::move(v), std::move(tc),
                                                                                                                     std::move(te), std::move(f)); }
#endif
                                                                               );
    renderShaderProgram["clone"]                               = &RenderShaderProgram::clone;
    renderShaderProgram["destroyVertexShader"]                 = &RenderShaderProgram::destroyVertexShader;
#if !defined(USE_OPENGL_ES)
    renderShaderProgram["destroyTessellationControlShader"]    = &RenderShaderProgram::destroyTessellationControlShader;
    renderShaderProgram["destroyTessellationEvaluationShader"] = &RenderShaderProgram::destroyTessellationEvaluationShader;
    renderShaderProgram["destroyGeometryShader"]               = &RenderShaderProgram::destroyGeometryShader;
#endif
    renderShaderProgram["destroyFragmentShader"]               = &RenderShaderProgram::destroyFragmentShader;
  }

  {
    sol::table imageTextureUsage    = state["ImageTextureUsage"].get_or_create<sol::table>();
    imageTextureUsage["READ"]       = ImageTextureUsage::READ;
    imageTextureUsage["WRITE"]      = ImageTextureUsage::WRITE;
    imageTextureUsage["READ_WRITE"] = ImageTextureUsage::READ_WRITE;

    sol::usertype<ShaderProgram> shaderProgram = state.new_usertype<ShaderProgram>("ShaderProgram", sol::no_constructor);
    shaderProgram["hasAttribute"]           = [] (const ShaderProgram& p, const std::string& n) { return p.hasAttribute(n); };
    shaderProgram["getAttributeCount"]      = &ShaderProgram::getAttributeCount;
    shaderProgram["hasTexture"]             = sol::overload(PickOverload<const Texture&>(&ShaderProgram::hasTexture),
                                                            PickOverload<const std::string&>(&ShaderProgram::hasTexture));
    shaderProgram["getTextureCount"]        = &ShaderProgram::getTextureCount;
    shaderProgram["getTexture"]             = sol::overload([] (const ShaderProgram& p, std::size_t i) { return &p.getTexture(i); },
                                                            [] (const ShaderProgram& p, const std::string& n) { return &p.getTexture(n); });
#if !defined(USE_WEBGL)
    shaderProgram["hasImageTexture"]        = sol::overload(PickOverload<const Texture&>(&ShaderProgram::hasImageTexture),
                                                            PickOverload<const std::string&>(&ShaderProgram::hasImageTexture));
    shaderProgram["getImageTextureCount"]   = &ShaderProgram::getImageTextureCount;
    shaderProgram["getImageTexture"]        = sol::overload([] (const ShaderProgram& p, std::size_t i) { return &p.getImageTexture(i); },
                                                            [] (const ShaderProgram& p, const std::string& n) { return &p.getImageTexture(n); });
#endif
    shaderProgram["setIntAttribute"]        = &ShaderProgram::setAttribute<int>;
    shaderProgram["setUintAttribute"]       = &ShaderProgram::setAttribute<unsigned int>;
    shaderProgram["setFloatAttribute"]      = &ShaderProgram::setAttribute<float>;
    shaderProgram["setAttribute"]           = sol::overload(&ShaderProgram::setAttribute<const Vec2i&>,
                                                            &ShaderProgram::setAttribute<const Vec3i&>,
                                                            &ShaderProgram::setAttribute<const Vec4i&>,
                                                            &ShaderProgram::setAttribute<const Vec2u&>,
                                                            &ShaderProgram::setAttribute<const Vec3u&>,
                                                            &ShaderProgram::setAttribute<const Vec4u&>,
                                                            &ShaderProgram::setAttribute<const Vec2f&>,
                                                            &ShaderProgram::setAttribute<const Vec3f&>,
                                                            &ShaderProgram::setAttribute<const Vec4f&>,
                                                            &ShaderProgram::setAttribute<const Mat2f&>,
                                                            &ShaderProgram::setAttribute<const Mat3f&>,
                                                            &ShaderProgram::setAttribute<const Mat4f&>);
    // Sol does not seem to be able to bind shared pointers from derived classes to a shared pointer of the base class
    //   (e.g., Texture2DPtr cannot be given directly to setTexture(), which takes a TexturePtr)
    shaderProgram["setTexture"]             = sol::overload(
#if !defined(USE_OPENGL_ES)
                                                            [] (ShaderProgram& p, Texture1DPtr t, const std::string& n) { p.setTexture(std::move(t), n); },
#endif
                                                            [] (ShaderProgram& p, Texture2DPtr t, const std::string& n) { p.setTexture(std::move(t), n); },
                                                            [] (ShaderProgram& p, Texture3DPtr t, const std::string& n) { p.setTexture(std::move(t), n); });
#if !defined(USE_WEBGL)
    shaderProgram["setImageTexture"]        = sol::overload(
#if !defined(USE_OPENGL_ES)
                                                            [] (ShaderProgram& p, Texture1DPtr t,
                                                                const std::string& n) { p.setImageTexture(std::move(t), n, ImageTextureUsage::READ_WRITE); },
                                                            [] (ShaderProgram& p, Texture1DPtr t, const std::string& n,
                                                                ImageTextureUsage u) { p.setImageTexture(std::move(t), n, u); },
#endif
                                                            [] (ShaderProgram& p, Texture2DPtr t,
                                                                const std::string& n) { p.setImageTexture(std::move(t), n, ImageTextureUsage::READ_WRITE); },
                                                            [] (ShaderProgram& p, Texture2DPtr t, const std::string& n,
                                                                ImageTextureUsage u) { p.setImageTexture(std::move(t), n, u); },
                                                            [] (ShaderProgram& p, Texture3DPtr t,
                                                                const std::string& n) { p.setImageTexture(std::move(t), n, ImageTextureUsage::READ_WRITE); },
                                                            [] (ShaderProgram& p, Texture3DPtr t, const std::string& n,
                                                                ImageTextureUsage u) { p.setImageTexture(std::move(t), n, u); });
#endif
    shaderProgram["loadShaders"]            = &ShaderProgram::loadShaders;
    shaderProgram["compileShaders"]         = &ShaderProgram::compileShaders;
    shaderProgram["link"]                   = &ShaderProgram::link;
    shaderProgram["isLinked"]               = &ShaderProgram::isLinked;
    shaderProgram["updateShaders"]          = &ShaderProgram::updateShaders;
    shaderProgram["use"]                    = &ShaderProgram::use;
    shaderProgram["isUsed"]                 = &ShaderProgram::isUsed;
    shaderProgram["sendAttributes"]         = &ShaderProgram::sendAttributes;
    shaderProgram["removeAttribute"]        = &ShaderProgram::removeAttribute;
    shaderProgram["clearAttributes"]        = &ShaderProgram::clearAttributes;
    shaderProgram["initTextures"]           = &ShaderProgram::initTextures;
    shaderProgram["bindTextures"]           = &ShaderProgram::bindTextures;
    shaderProgram["removeTexture"]          = sol::overload(PickOverload<const Texture&>(&ShaderProgram::removeTexture),
                                                            PickOverload<const std::string&>(&ShaderProgram::removeTexture));
    shaderProgram["clearTextures"]          = &ShaderProgram::clearTextures;
#if !defined(USE_WEBGL)
    shaderProgram["initImageTextures"]      = &ShaderProgram::initImageTextures;
    shaderProgram["bindImageTextures"]      = &ShaderProgram::bindImageTextures;
    shaderProgram["removeImageTexture"]     = sol::overload(PickOverload<const Texture&>(&ShaderProgram::removeImageTexture),
                                                            PickOverload<const std::string&>(&ShaderProgram::removeImageTexture));
    shaderProgram["clearImageTextures"]     = &ShaderProgram::clearImageTextures;
#endif
    shaderProgram["recoverUniformLocation"] = &ShaderProgram::recoverUniformLocation;
  }
}

} // namespace Raz
