#pragma once

#ifndef RAZ_LUAWRAPPER_HPP
#define RAZ_LUAWRAPPER_HPP

#include <string>

namespace sol { class state; }

namespace Raz {

class FilePath;

class LuaWrapper {
public:
  static void registerTypes();
  /// Executes a script from a string.
  /// \param code Code to be executed.
  /// \return True if the script has been executed without errors, false otherwise.
  static bool execute(const std::string& code);
  /// Executes a script from a file.
  /// \param filePath Path to the script to be executed. Must be valid ASCII; special characters are not handled.
  /// \return True if the script has been executed without errors, false otherwise.
  static bool executeFromFile(const FilePath& filePath);

private:
  static void registerAnimationTypes();
  static void registerAudioTypes();
  static void registerCoreTypes();
  static void registerDataTypes();
  static void registerEntityTypes();
  static void registerFileFormatTypes();
  static void registerMathTypes();
  static void registerMatrixTypes();
  static void registerMeshTypes();
  static void registerMeshRendererTypes();
  static void registerOverlayTypes();
  static void registerPhysicsTypes();
  static void registerRenderTypes();
  static void registerRenderGraphTypes();
  static void registerRenderSystemTypes();
  static void registerShaderTypes();
  static void registerShaderProgramTypes();
  static void registerShapeTypes();
  static void registerTextureTypes();
  static void registerUtilsTypes();
  static void registerVectorTypes();
  static void registerWindowTypes();

  static sol::state& getState();
};

} // namespace Raz

#endif // RAZ_LUAWRAPPER_HPP
