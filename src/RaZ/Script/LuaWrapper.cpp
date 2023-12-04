#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include "sol/sol.hpp"

namespace Raz {

void LuaWrapper::registerTypes() {
  [[maybe_unused]] static const bool _ = [] () {
    Logger::debug("[LuaWrapper] Registering types...");

    registerAnimationTypes();
#if defined(RAZ_USE_AUDIO)
    registerAudioTypes();
#endif
    registerCoreTypes();
    registerDataTypes();
    registerEntityTypes();
    registerFileFormatTypes();
    registerImageTypes();
    registerMathTypes();
    registerMatrixTypes();
    registerMeshTypes();
    registerMeshRendererTypes();
#if !defined(RAZ_NO_OVERLAY)
    registerOverlayBaseTypes();
    registerOverlayWidgetTypes();
#endif
    registerPhysicsTypes();
    registerRenderTypes();
    registerRenderGraphTypes();
    registerRenderSystemTypes();
    registerShaderTypes();
    registerShaderProgramTypes();
    registerShapeTypes();
    registerTextureTypes();
    registerUtilsTypes();
    registerVectorTypes();
    registerWindowTypes();

    Logger::debug("[LuaWrapper] Registered types");

    return true;
  }();
}

bool LuaWrapper::execute(const std::string& code) {
  if (code.empty())
    return false;

  Logger::debug("[LuaWrapper] Executing code...");

  try {
    getState().script(code);
  } catch (const sol::error& err) {
    Logger::error("[LuaWrapper] Error executing code: '" + std::string(err.what()) + "'.");
    return false;
  }

  Logger::debug("[LuaWrapper] Executed code");

  return true;
}

bool LuaWrapper::executeFromFile(const FilePath& filePath) {
  if (filePath.isEmpty())
    return false;

  Logger::debug("[LuaWrapper] Executing code from file ('" + filePath + "')...");

  try {
    getState().script_file(filePath.toUtf8());
  } catch (const sol::error& err) {
    Logger::error("[LuaWrapper] Error executing code from file: '" + std::string(err.what()) + "'.");
    return false;
  }

  Logger::debug("[LuaWrapper] Executed code from file");

  return true;
}

sol::state& LuaWrapper::getState() {
  static sol::state state = [] () {
    Logger::debug("[LuaWrapper] Initializing state...");

    sol::state luaState;
    luaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

    Logger::debug("[LuaWrapper] Initialized state");

    return luaState;
  }();

  return state;
}

} // namespace Raz
