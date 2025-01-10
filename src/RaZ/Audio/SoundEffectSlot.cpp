#include "RaZ/Audio/SoundEffect.hpp"
#include "RaZ/Audio/SoundEffectSlot.hpp"
#include "RaZ/Utils/CompilerUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <AL/al.h>
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <AL/efx.h>
#endif

namespace Raz {

namespace {

constexpr const char* recoverAlErrorStr(int errorCode) {
  switch (errorCode) {
    case AL_INVALID_NAME:      return "Invalid name";
    case AL_INVALID_ENUM:      return "Invalid enum";
    case AL_INVALID_VALUE:     return "Invalid value";
    case AL_INVALID_OPERATION: return "Invalid operation";
    case AL_OUT_OF_MEMORY:     return "Out of memory";
    case AL_NO_ERROR:          return "No error";
    default:                   return "Unknown error";
  }
}

inline void checkError(const std::string& errorMsg) {
  const int errorCode = alGetError();

  if (errorCode != AL_NO_ERROR)
    Logger::error("[OpenAL] " + errorMsg + " (" + recoverAlErrorStr(errorCode) + ").");
}

// Auxiliary effect slot functions
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
//LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
//LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
//LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
//LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
//LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
//LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
//LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

inline bool loadFunctions() noexcept {
  PUSH_WARNINGS_STATE
  DISABLE_WARNING_GCC(-Wconditionally-supported)

  alGenAuxiliaryEffectSlots    = reinterpret_cast<LPALGENAUXILIARYEFFECTSLOTS>(alGetProcAddress("alGenAuxiliaryEffectSlots"));
  alDeleteAuxiliaryEffectSlots = reinterpret_cast<LPALDELETEAUXILIARYEFFECTSLOTS>(alGetProcAddress("alDeleteAuxiliaryEffectSlots"));
  alIsAuxiliaryEffectSlot      = reinterpret_cast<LPALISAUXILIARYEFFECTSLOT>(alGetProcAddress("alIsAuxiliaryEffectSlot"));
  alAuxiliaryEffectSloti       = reinterpret_cast<LPALAUXILIARYEFFECTSLOTI>(alGetProcAddress("alAuxiliaryEffectSloti"));
  //alAuxiliaryEffectSlotiv      = reinterpret_cast<LPALAUXILIARYEFFECTSLOTIV>(alGetProcAddress("alAuxiliaryEffectSlotiv"));
  //alAuxiliaryEffectSlotf       = reinterpret_cast<LPALAUXILIARYEFFECTSLOTF>(alGetProcAddress("alAuxiliaryEffectSlotf"));
  //alAuxiliaryEffectSlotfv      = reinterpret_cast<LPALAUXILIARYEFFECTSLOTFV>(alGetProcAddress("alAuxiliaryEffectSlotfv"));
  //alGetAuxiliaryEffectSloti    = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTI>(alGetProcAddress("alGetAuxiliaryEffectSloti"));
  //alGetAuxiliaryEffectSlotiv   = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTIV>(alGetProcAddress("alGetAuxiliaryEffectSlotiv"));
  //alGetAuxiliaryEffectSlotf    = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTF>(alGetProcAddress("alGetAuxiliaryEffectSlotf"));
  //alGetAuxiliaryEffectSlotfv   = reinterpret_cast<LPALGETAUXILIARYEFFECTSLOTFV>(alGetProcAddress("alGetAuxiliaryEffectSlotfv"));

  POP_WARNINGS_STATE

  return true;
}

} // namespace

void SoundEffectSlot::init() {
  ZoneScopedN("SoundEffectSlot::init");

  if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX")) {
    Logger::error("[SoundEffectSlot] Sound effects are unavailable.");
    return;
  }

  [[maybe_unused]] static const bool funcsLoaded = loadFunctions();

  Logger::debug("[SoundEffectSlot] Initializing...");

  alGetError(); // Flushing errors

  destroy();

  alGenAuxiliaryEffectSlots(1, &m_index.get());
  checkError("Failed to create a sound effect slot");

  Logger::debug("[SoundEffectSlot] Initialized (ID: " + std::to_string(m_index) + ')');
}

void SoundEffectSlot::loadEffect(const SoundEffect& effect) const noexcept {
  ZoneScopedN("SoundEffectSlot::loadEffect");

  alAuxiliaryEffectSloti(m_index, AL_EFFECTSLOT_EFFECT, static_cast<int>(effect.getIndex()));
  checkError("Failed to load the sound effect");
}

void SoundEffectSlot::destroy() {
  ZoneScopedN("SoundEffectSlot::destroy");

  if (!m_index.isValid())
    return;

  Logger::debug("[SoundEffectSlot] Destroying (ID: " + std::to_string(m_index) + ")...");

  if (alIsAuxiliaryEffectSlot(m_index)) {
    alDeleteAuxiliaryEffectSlots(1, &m_index.get());
    checkError("Failed to delete sound effect slot");
  }

  m_index.reset();

  Logger::debug("[SoundEffectSlot] Destroyed");
}

} // namespace Raz
