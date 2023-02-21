#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 0
#include "sol/sol.hpp"

namespace Raz {

void LuaWrapper::registerTypes() {
  registerAnimationTypes();
  registerAudioTypes();
  registerCoreTypes();
  registerDataTypes();
  registerEntityTypes();
  registerFileFormatTypes();
  registerMathTypes();
  registerMatrixTypes();
  registerMeshTypes();
  registerMeshRendererTypes();
  registerOverlayTypes();
  registerPhysicsTypes();
  registerRenderTypes();
  registerRenderGraphTypes();
  registerShaderTypes();
  registerShapeTypes();
  registerTextureTypes();
  registerUtilsTypes();
  registerVectorTypes();
  registerWindowTypes();
}

bool LuaWrapper::execute(const std::string& code) {
  if (code.empty())
    return false;

  try {
    getState().script(code);
  } catch (const sol::error& err) {
    Logger::error("[LuaWrapper] Error running code: '" + std::string(err.what()) + "'.");
    return false;
  }

  return true;
}

bool LuaWrapper::executeFromFile(const FilePath& filePath) {
  if (filePath.isEmpty())
    return false;

  try {
    getState().script_file(filePath.toUtf8());
  } catch (const sol::error& err) {
    Logger::error("[LuaWrapper] Error running file: '" + std::string(err.what()) + "'.");
    return false;
  }

  return true;
}

sol::state& LuaWrapper::getState() {
  static sol::state state = [] () {
    sol::state luaState;
    luaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

    return luaState;
  }();

  return state;
}

} // namespace Raz
