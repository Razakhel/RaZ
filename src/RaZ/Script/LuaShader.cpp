#include "RaZ/Render/Shader.hpp"
#include "RaZ/Render/ShaderProgram.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerShaderTypes() {
  sol::state& state = getState();

  // Shaders
  {
    {
      sol::usertype<ComputeShader> computeShader = state.new_usertype<ComputeShader>("ComputeShader",
                                                                                     sol::constructors<ComputeShader(),
                                                                                                       ComputeShader(FilePath)>(),
                                                                                     sol::base_classes, sol::bases<Shader>());
      computeShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&ComputeShader::loadFromSource),
                                                      PickOverload<const char*>(&ComputeShader::loadFromSource));
      computeShader["clone"]          = &ComputeShader::clone;
    }

    {
      sol::usertype<FragmentShader> fragmentShader = state.new_usertype<FragmentShader>("FragmentShader",
                                                                                        sol::constructors<FragmentShader(),
                                                                                                          FragmentShader(FilePath)>(),
                                                                                        sol::base_classes, sol::bases<Shader>());
      fragmentShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&FragmentShader::loadFromSource),
                                                       PickOverload<const char*>(&FragmentShader::loadFromSource));
      fragmentShader["clone"]          = &FragmentShader::clone;
    }

    {
      sol::usertype<GeometryShader> geometryShader = state.new_usertype<GeometryShader>("GeometryShader",
                                                                                        sol::constructors<GeometryShader(),
                                                                                                          GeometryShader(FilePath)>(),
                                                                                        sol::base_classes, sol::bases<Shader>());
      geometryShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&GeometryShader::loadFromSource),
                                                       PickOverload<const char*>(&GeometryShader::loadFromSource));
      geometryShader["clone"]          = &GeometryShader::clone;
    }

    {
      sol::usertype<Shader> shader = state.new_usertype<Shader>("Shader", sol::no_constructor);
      shader["getIndex"]   = &Shader::getIndex;
      shader["getPath"]    = &Shader::getPath;
      shader["isValid"]    = &Shader::isValid;
      shader["import"]     = &Shader::import;
      shader["load"]       = &Shader::load;
      shader["compile"]    = &Shader::compile;
      shader["isCompiled"] = &Shader::isCompiled;
      shader["destroy"]    = &Shader::destroy;
    }

    {
      sol::usertype<TessellationControlShader> tessCtrlShader = state.new_usertype<TessellationControlShader>("TessellationControlShader",
                                                                                                              sol::constructors<
                                                                                                                TessellationControlShader(),
                                                                                                                TessellationControlShader(FilePath)
                                                                                                              >(),
                                                                                                              sol::base_classes, sol::bases<Shader>());
      tessCtrlShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&TessellationControlShader::loadFromSource),
                                                       PickOverload<const char*>(&TessellationControlShader::loadFromSource));
      tessCtrlShader["clone"]          = &TessellationControlShader::clone;
    }

    {
      sol::usertype<TessellationEvaluationShader> tessEvalShader = state.new_usertype<TessellationEvaluationShader>("TessellationEvaluationShader",
                                                                                                                    sol::constructors<
                                                                                                                      TessellationEvaluationShader(),
                                                                                                                      TessellationEvaluationShader(FilePath)
                                                                                                                    >(),
                                                                                                                    sol::base_classes, sol::bases<Shader>());
      tessEvalShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&TessellationEvaluationShader::loadFromSource),
                                                       PickOverload<const char*>(&TessellationEvaluationShader::loadFromSource));
      tessEvalShader["clone"]          = &TessellationEvaluationShader::clone;
    }

    {
      sol::usertype<VertexShader> vertexShader = state.new_usertype<VertexShader>("VertexShader",
                                                                                  sol::constructors<VertexShader(),
                                                                                                    VertexShader(FilePath)>(),
                                                                                  sol::base_classes, sol::bases<Shader>());
      vertexShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&VertexShader::loadFromSource),
                                                     PickOverload<const char*>(&VertexShader::loadFromSource));
      vertexShader["clone"]          = &VertexShader::clone;
    }
  }

  // ShaderProgram
  {
#if !defined(USE_OPENGL_ES)
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
      renderShaderProgram["hasTessellationControlShader"]        = &RenderShaderProgram::hasTessellationControlShader;
      renderShaderProgram["getTessellationControlShader"]        = [] (const RenderShaderProgram& p) { return &p.getTessellationControlShader(); };
      renderShaderProgram["hasTessellationEvaluationShader"]     = &RenderShaderProgram::hasTessellationEvaluationShader;
      renderShaderProgram["getTessellationEvaluationShader"]     = [] (const RenderShaderProgram& p) { return &p.getTessellationEvaluationShader(); };
      renderShaderProgram["hasGeometryShader"]                   = &RenderShaderProgram::hasGeometryShader;
      renderShaderProgram["getGeometryShader"]                   = [] (const RenderShaderProgram& p) { return &p.getGeometryShader(); };
      renderShaderProgram["getFragmentShader"]                   = [] (const RenderShaderProgram& p) { return &p.getFragmentShader(); };
      renderShaderProgram["setVertexShader"]                     = [] (RenderShaderProgram& p, VertexShader& s) { p.setVertexShader(std::move(s)); };
      renderShaderProgram["setTessellationControlShader"]        = [] (RenderShaderProgram& p,
                                                                       TessellationControlShader& s) { p.setTessellationControlShader(std::move(s)); };
      renderShaderProgram["setTessellationEvaluationShader"]     = [] (RenderShaderProgram& p,
                                                                       TessellationEvaluationShader& s) { p.setTessellationEvaluationShader(std::move(s)); };
      renderShaderProgram["setGeometryShader"]                   = [] (RenderShaderProgram& p, GeometryShader& s) { p.setGeometryShader(std::move(s)); };
      renderShaderProgram["setFragmentShader"]                   = [] (RenderShaderProgram& p, FragmentShader& s) { p.setFragmentShader(std::move(s)); };
      renderShaderProgram["setShaders"]                          = sol::overload([] (RenderShaderProgram& p, VertexShader& v,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, GeometryShader& g,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(g),
                                                                                                                       std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, TessellationEvaluationShader& te,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(te),
                                                                                                                       std::move(f)); },
                                                                                 [] (RenderShaderProgram& p, VertexShader& v, TessellationControlShader& tc,
                                                                                     TessellationEvaluationShader& te,
                                                                                     FragmentShader& f) { p.setShaders(std::move(v), std::move(tc),
                                                                                                                       std::move(te), std::move(f)); });
      renderShaderProgram["clone"]                               = &RenderShaderProgram::clone;
      renderShaderProgram["destroyVertexShader"]                 = &RenderShaderProgram::destroyVertexShader;
      renderShaderProgram["destroyTessellationControlShader"]    = &RenderShaderProgram::destroyTessellationControlShader;
      renderShaderProgram["destroyTessellationEvaluationShader"] = &RenderShaderProgram::destroyTessellationEvaluationShader;
      renderShaderProgram["destroyGeometryShader"]               = &RenderShaderProgram::destroyGeometryShader;
      renderShaderProgram["destroyFragmentShader"]               = &RenderShaderProgram::destroyFragmentShader;
    }

    {
      sol::usertype<ShaderProgram> shaderProgram = state.new_usertype<ShaderProgram>("ShaderProgram", sol::no_constructor);
      shaderProgram["hasAttribute"]           = [] (const ShaderProgram& p, const std::string& n) { return p.hasAttribute(n); };
      shaderProgram["getAttributeCount"]      = &ShaderProgram::getAttributeCount;
      shaderProgram["hasTexture"]             = sol::overload(PickOverload<const Texture&>(&ShaderProgram::hasTexture),
                                                              PickOverload<const std::string&>(&ShaderProgram::hasTexture));
      shaderProgram["getTextureCount"]        = &ShaderProgram::getTextureCount;
      shaderProgram["getTexture"]             = sol::overload([] (const ShaderProgram& p, std::size_t i) { return &p.getTexture(i); },
                                                              [] (const ShaderProgram& p, const std::string& n) { return &p.getTexture(n); });
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
      shaderProgram["recoverUniformLocation"] = &ShaderProgram::recoverUniformLocation;
    }
  }
}

} // namespace Raz
