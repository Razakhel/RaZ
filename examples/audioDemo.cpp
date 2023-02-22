#include "RaZ/RaZ.hpp"

using namespace std::literals;

namespace {

constexpr unsigned int baseWindowWidth = 375;
constexpr Raz::Vec2f overlayAudioSize(baseWindowWidth, 80.f);
constexpr Raz::Vec2f overlaySoundSize(baseWindowWidth, 220.f);
constexpr Raz::Vec2f overlayMicSize(baseWindowWidth, 330.f);
constexpr auto baseWindowHeight = static_cast<unsigned int>(overlayAudioSize.y() + overlaySoundSize.y() + overlayMicSize.y());
constexpr Raz::Vec2f overlayReverbSize(487.f, 655.f);

constexpr Raz::AudioFormat recoverFormat(bool isStereo, uint8_t bitDepth) {
  return (isStereo ? (bitDepth == 8 ? Raz::AudioFormat::STEREO_U8 : Raz::AudioFormat::STEREO_I16)
                   : (bitDepth == 8 ? Raz::AudioFormat::MONO_U8 : Raz::AudioFormat::MONO_I16));
}

} // namespace

int main() {
  try {
    ////////////////////
    // Initialization //
    ////////////////////

    Raz::Application app;
    Raz::World& world = app.addWorld(2);

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    ///////////////
    // Rendering //
    ///////////////

    Raz::Window& window = world.addSystem<Raz::RenderSystem>(baseWindowWidth, baseWindowHeight, "RaZ", Raz::WindowSetting::NON_RESIZABLE).getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Camera>(); // The RenderSystem needs a Camera

    ///////////
    // Audio //
    ///////////

    auto& audio = world.addSystem<Raz::AudioSystem>();

    Raz::Entity& sound = world.addEntity();
    auto& soundTrans   = sound.addComponent<Raz::Transform>();
    auto& soundComp    = sound.addComponent<Raz::Sound>(Raz::WavFormat::load(RAZ_ROOT "assets/sounds/knock.wav"));

    Raz::Microphone microphone(Raz::AudioFormat::MONO_U8, 16000, 1.f);

    // Sound effects are not (yet?) available with Emscripten's own OpenAL implementation
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    Raz::SoundEffect reverb;
    Raz::SoundEffectSlot effectSlot;

    Raz::ReverberationParams reverbParams {};
    reverb.load(reverbParams);

    effectSlot.loadEffect(reverb);
#endif

    /////////////
    // Overlay //
    /////////////

    bool isRepeating        = false;
    bool moveSource         = false;
    bool isCapturing        = false;
    bool isCaptureStereo    = false;
    uint8_t captureBitDepth = 8;
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    bool effectEnabled = false;
#endif

    Raz::OverlayWindow& overlayAudio = window.getOverlay().addWindow("Audio info", overlayAudioSize);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    overlayAudio.addDropdown("Output device",
                             Raz::AudioSystem::recoverDevices(),
                             [&audio, &soundComp, &isRepeating, &reverb, &reverbParams, &effectSlot, &effectEnabled] (const std::string& name, std::size_t) {
      const float gain  = soundComp.recoverGain();
      const float pitch = soundComp.recoverPitch();

      audio.openDevice(name.c_str());

      soundComp.init();
      soundComp.load();
      soundComp.setRepeat(isRepeating);
      soundComp.setGain(gain);
      soundComp.setPitch(pitch);

      reverb.init();
      reverb.load(reverbParams);

      effectSlot.init();
      effectSlot.loadEffect(reverb);

      if (effectEnabled)
        soundComp.linkSlot(effectSlot);
    });

    overlayAudio.addDropdown("Input device",
                             Raz::Microphone::recoverDevices(),
                             [&microphone, &isCaptureStereo, &captureBitDepth] (const std::string& name, std::size_t) {
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f, name.c_str());
    });
#else
    overlayAudio.addLabel("Output & input devices cannot be changed with Emscripten");
#endif

    Raz::OverlayWindow& overlaySound = window.getOverlay().addWindow("Sound params", overlaySoundSize, Raz::Vec2f(0.f, overlayAudioSize.y()));

    overlaySound.addDropdown("Sound", { "Knock", "Wave seagulls" }, [&soundComp, &isRepeating
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
                                                                     , &effectSlot, &reverb, &effectEnabled
#endif
                                                                     ] (const std::string&, std::size_t i) {
      const float gain  = soundComp.recoverGain();
      const float pitch = soundComp.recoverPitch();

      switch (i) {
        case 0:
        default:
          soundComp = Raz::WavFormat::load(RAZ_ROOT "assets/sounds/knock.wav");
          break;

        case 1:
          soundComp = Raz::WavFormat::load(RAZ_ROOT "assets/sounds/wave_seagulls.wav");
          break;
      }

      soundComp.setRepeat(isRepeating);
      soundComp.setGain(gain);
      soundComp.setPitch(pitch);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
      effectSlot.loadEffect(reverb);

      if (effectEnabled)
        soundComp.linkSlot(effectSlot);
#endif
    });

    overlaySound.addButton("Play sound", [&soundComp] () noexcept {
      soundComp.play();
    });

    overlaySound.addButton("Pause sound", [&soundComp] () noexcept {
      soundComp.pause();
    });

    overlaySound.addCheckbox("Repeat", [&isRepeating, &soundComp] () noexcept {
      isRepeating = true;
      soundComp.setRepeat(isRepeating);
    }, [&isRepeating, &soundComp] () noexcept {
      isRepeating = false;
      soundComp.setRepeat(isRepeating);
    }, isRepeating);

    overlaySound.addCheckbox("Move source", [&moveSource] () noexcept {
      moveSource = true;
    }, [&moveSource] () noexcept {
      moveSource = false;
    }, moveSource);

    overlaySound.addSlider("Sound gain", [&soundComp] (float val) noexcept {
      soundComp.setGain(val);
    }, 0.f, 1.f, 1.f);

    overlaySound.addSlider("Sound pitch", [&soundComp] (float val) noexcept {
      soundComp.setPitch(val);
    }, 0.f, 1.f, 1.f);

    Raz::OverlayWindow& overlayMic = window.getOverlay().addWindow("Microphone params",
                                                                   overlayMicSize,
                                                                   Raz::Vec2f(0.f, overlayAudioSize.y() + overlaySoundSize.y()));

    overlayMic.addButton("Start capture", [&isCapturing, &microphone] () noexcept {
      isCapturing = true;
      microphone.start();
    });

    overlayMic.addButton("Stop capture", [&isCapturing, &microphone] () noexcept {
      isCapturing = false;
      microphone.stop();
    });

    Raz::OverlayPlot& stereoPlot            = overlayMic.addPlot("Stereo capture", 16000.f, {}, {}, -1.f, 1.f, true, 200.f);
    Raz::OverlayPlotEntry& leftCapturePlot  = stereoPlot.addEntry("Left");
    Raz::OverlayPlotEntry& rightCapturePlot = stereoPlot.addEntry("Right");
    stereoPlot.disable();

    Raz::OverlayPlot& monoPlot             = overlayMic.addPlot("Mono capture", 16000.f, {}, {}, -1.f, 1.f, true, 200.f);
    Raz::OverlayPlotEntry& monoCapturePlot = monoPlot.addEntry({});

    overlayMic.addDropdown("Channels",
                           { "Mono", "Stereo" },
                           [&isCaptureStereo, &captureBitDepth, &microphone, &isCapturing, &stereoPlot, &monoPlot] (const std::string&, std::size_t i) {
      isCaptureStereo = (i == 1);
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f);
      isCapturing = false;

      stereoPlot.enable(isCaptureStereo);
      monoPlot.enable(!isCaptureStereo);
    }, 0);

    overlayMic.addDropdown("Bit depth", { "8", "16" }, [&captureBitDepth, &isCaptureStereo, &microphone, &isCapturing] (const std::string&, std::size_t i) {
      captureBitDepth = (i == 0 ? 8 : 16);
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f);
      isCapturing = false;
    }, 0);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    Raz::OverlayWindow& overlayReverb = window.getOverlay().addWindow("Reverb params", overlayReverbSize, Raz::Vec2f(baseWindowWidth - 20, 0.f));
    overlayReverb.disable();

    overlaySound.addCheckbox("Enable reverb", [&effectEnabled, &soundComp, &effectSlot, &overlayReverb, &window] () noexcept {
      effectEnabled = true;
      soundComp.linkSlot(effectSlot);
      overlayReverb.enable();
      window.resize(baseWindowWidth + static_cast<unsigned int>(overlayReverbSize.x()) - 40,
                    std::max(baseWindowHeight, static_cast<unsigned int>(overlayReverbSize.y())));
    }, [&effectEnabled, &soundComp, &overlayReverb, &window] () noexcept {
      effectEnabled = false;
      soundComp.unlinkSlot();
      overlayReverb.disable();
      window.resize(baseWindowWidth, baseWindowHeight);
    }, false);

    overlayReverb.addSlider("Density", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.density = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.density);

    overlayReverb.addSlider("Diffusion", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.diffusion = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.diffusion);

    overlayReverb.addSlider("Gain", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.gain = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.gain);

    overlayReverb.addSlider("Gain high frequency", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.gainHighFrequency = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.gainHighFrequency);

    overlayReverb.addSlider("Gain low frequency", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.gainLowFrequency = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.gainLowFrequency);

    overlayReverb.addSlider("Decay time", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.decayTime = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.1f, 20.f, reverbParams.decayTime);

    overlayReverb.addSlider("Decay high frequency ratio", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.decayHighFrequencyRatio = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.1f, 2.f, reverbParams.decayHighFrequencyRatio);

    overlayReverb.addSlider("Decay low frequency ratio", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.decayLowFrequencyRatio = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.1f, 2.f, reverbParams.decayLowFrequencyRatio);

    overlayReverb.addSlider("Reflections gain", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.reflectionsGain = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 3.16f, reverbParams.reflectionsGain);

    overlayReverb.addSlider("Reflections delay", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.reflectionsDelay = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 0.3f, reverbParams.reflectionsDelay);

    overlayReverb.addSlider("Reflections pan X", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.reflectionsPan[0] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.reflectionsPan[0]);

    overlayReverb.addSlider("Reflections pan Y", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.reflectionsPan[1] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.reflectionsPan[1]);

    overlayReverb.addSlider("Reflections pan Z", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.reflectionsPan[2] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.reflectionsPan[2]);

    overlayReverb.addSlider("Late reverb gain", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lateReverbGain = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 10.f, reverbParams.lateReverbGain);

    overlayReverb.addSlider("Late reverb delay", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lateReverbDelay = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 0.01f, reverbParams.lateReverbDelay);

    overlayReverb.addSlider("Late reverb pan X", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lateReverbPan[0] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.lateReverbPan[0]);

    overlayReverb.addSlider("Late reverb pan Y", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lateReverbPan[1] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.lateReverbPan[1]);

    overlayReverb.addSlider("Late reverb pan Z", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lateReverbPan[2] = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, -100.f, 100.f, reverbParams.lateReverbPan[2]);

    overlayReverb.addSlider("Echo time", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.echoTime = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.075f, 0.25f, reverbParams.echoTime);

    overlayReverb.addSlider("Echo depth", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.echoDepth = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.echoDepth);

    overlayReverb.addSlider("Modulation time", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.modulationTime = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.04f, 4.f, reverbParams.modulationTime);

    overlayReverb.addSlider("Modulation depth", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.modulationDepth = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 1.f, reverbParams.modulationDepth);

    overlayReverb.addSlider("Air absorption gain high frequency", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.airAbsorptionGainHighFrequency = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.892f, 1.f, reverbParams.airAbsorptionGainHighFrequency);

    overlayReverb.addSlider("High frequency reference", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.highFrequencyReference = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 1000.f, 20000.f, reverbParams.highFrequencyReference);

    overlayReverb.addSlider("Low frequency reference", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.lowFrequencyReference = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 20.f, 1000.f, reverbParams.lowFrequencyReference);

    overlayReverb.addSlider("Room rolloff factor", [&reverbParams, &reverb, &effectSlot] (float val) {
      reverbParams.roomRolloffFactor = val;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, 0.f, 10.f, reverbParams.roomRolloffFactor);

    overlayReverb.addCheckbox("Decay high frequency limit", [&reverbParams, &reverb, &effectSlot] () {
      reverbParams.decayHighFrequencyLimit = true;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, [&reverbParams, &reverb, &effectSlot] () {
      reverbParams.decayHighFrequencyLimit = false;
      reverb.load(reverbParams);
      effectSlot.loadEffect(reverb);
    }, reverbParams.decayHighFrequencyLimit);
#endif

    //////////////////////////
    // Starting application //
    //////////////////////////

    app.run([&] (float deltaTime) {
      static float globalTime = 0.f;
      globalTime += deltaTime;

      soundTrans.setPosition((moveSource ? Raz::Vec3f(std::sin(globalTime) * 3.f, 0.f, 1.f) : Raz::Vec3f(0.f)));

      if (isCapturing) {
        const std::vector<uint8_t> captureData = microphone.recoverData();

        // Stereo data is interleaved: the left capture is first, then the right (L, R, L, R, ...)

        // 8-bit depth values are unsigned and have a range between [0; 255]. They are remapped to [-1; 1]

        // 16-bit depth values are assumed to be in little endian, and are converted to big endian (each 2-bytes pack [AB] is converted to [BA])
        // They are signed and have a range between [-32768; 32767]. They are remapped to [-1; 1]

        constexpr float factorU8  = 1.f / 128;
        constexpr float factorI16 = 1.f / 32767;

        if (captureBitDepth == 8) {
          for (std::size_t i = 0; i < captureData.size(); ++i) {
            if (isCaptureStereo) { // Stereo 8
              leftCapturePlot.push(static_cast<float>(captureData[i]) * factorU8 - 1.f);
              rightCapturePlot.push(static_cast<float>(captureData[i + 1]) * factorU8 - 1.f);
              ++i;
            } else { // Mono 8
              monoCapturePlot.push(static_cast<float>(captureData[i]) * factorU8 - 1.f);
            }
          }
        } else {
          for (std::size_t i = 0; i < captureData.size(); i += 2) {
            if (isCaptureStereo) { // Stereo 16
              leftCapturePlot.push(static_cast<float>(static_cast<int16_t>((captureData[i] << 0u) | (captureData[i + 1] << 8u))) * factorI16);
              rightCapturePlot.push(static_cast<float>(static_cast<int16_t>((captureData[i + 2] << 0u) | (captureData[i + 3] << 8u))) * factorI16);
              i += 2;
            } else { // Mono 16
              monoCapturePlot.push(static_cast<float>(static_cast<int16_t>((captureData[i] << 0u) | (captureData[i + 1] << 8u))) * factorI16);
            }
          }
        }
      }
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
