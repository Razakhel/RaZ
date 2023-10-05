#include "RaZ/Render/Shader.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerShaderTypes() {
  sol::state& state = getState();

#if !defined(USE_WEBGL)
  {
    sol::usertype<ComputeShader> computeShader = state.new_usertype<ComputeShader>("ComputeShader",
                                                                                   sol::constructors<ComputeShader(),
                                                                                                     ComputeShader(FilePath)>(),
                                                                                   sol::base_classes, sol::bases<Shader>());
    computeShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&ComputeShader::loadFromSource),
                                                    PickOverload<const char*>(&ComputeShader::loadFromSource));
    computeShader["clone"]          = &ComputeShader::clone;
  }
#endif

  {
    sol::usertype<FragmentShader> fragmentShader = state.new_usertype<FragmentShader>("FragmentShader",
                                                                                      sol::constructors<FragmentShader(),
                                                                                                        FragmentShader(FilePath)>(),
                                                                                      sol::base_classes, sol::bases<Shader>());
    fragmentShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&FragmentShader::loadFromSource),
                                                     PickOverload<const char*>(&FragmentShader::loadFromSource));
    fragmentShader["clone"]          = &FragmentShader::clone;
  }

#if !defined(USE_OPENGL_ES)
  {
    sol::usertype<GeometryShader> geometryShader = state.new_usertype<GeometryShader>("GeometryShader",
                                                                                      sol::constructors<GeometryShader(),
                                                                                                        GeometryShader(FilePath)>(),
                                                                                      sol::base_classes, sol::bases<Shader>());
    geometryShader["loadFromSource"] = sol::overload(PickOverload<const std::string&>(&GeometryShader::loadFromSource),
                                                     PickOverload<const char*>(&GeometryShader::loadFromSource));
    geometryShader["clone"]          = &GeometryShader::clone;
  }
#endif

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

#if !defined(USE_OPENGL_ES)
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
#endif

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

} // namespace Raz
