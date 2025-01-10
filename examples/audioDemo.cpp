#include "RaZ/RaZ.hpp"

using namespace std::literals;

namespace {

constexpr unsigned int baseWindowWidth = 375;
constexpr Raz::Vec2f overlayAudioSize(baseWindowWidth, 100.f);
constexpr Raz::Vec2f overlaySoundSize(baseWindowWidth, 220.f);
constexpr Raz::Vec2f overlayMicSize(baseWindowWidth, 330.f);
constexpr auto baseWindowHeight = static_cast<unsigned int>(overlayAudioSize.y() + overlaySoundSize.y() + overlayMicSize.y());
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
constexpr Raz::Vec2f overlayReverbSize(487.f, 655.f);
constexpr Raz::Vec2f overlayChorusSize(320.f, 170.f);
constexpr Raz::Vec2f overlayDistortionSize(350.f, 147.f);
constexpr Raz::Vec2f overlayEchoSize(320.f, 147.f);
#endif

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

    // The RenderSystem requires an entity with Camera & Transform components
    world.addEntityWithComponents<Raz::Camera, Raz::Transform>();

    ///////////
    // Audio //
    ///////////

    static const Raz::AudioData knockAudio        = Raz::WavFormat::load(RAZ_ROOT "assets/sounds/knock.wav");
    static const Raz::AudioData waveSeagullsAudio = Raz::WavFormat::load(RAZ_ROOT "assets/sounds/wave_seagulls.wav");

    auto& audio = world.addSystem<Raz::AudioSystem>();

    // The Listener entity requires a Transform component
    auto& listener = world.addEntityWithComponent<Raz::Transform>().addComponent<Raz::Listener>();

    Raz::Entity& sound = world.addEntity();
    auto& soundTrans   = sound.addComponent<Raz::Transform>();
    auto& soundComp    = sound.addComponent<Raz::Sound>(knockAudio);

    Raz::Microphone microphone(Raz::AudioFormat::MONO_U8, 16000, 1.f);

    // Sound effects are not (yet?) available with Emscripten's own OpenAL implementation
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    Raz::SoundEffect reverb;
    Raz::ReverberationParams reverbParams {};
    reverb.load(reverbParams);

    Raz::SoundEffect chorus;
    Raz::ChorusParams chorusParams {};
    chorus.load(chorusParams);

    Raz::SoundEffect distortion;
    Raz::DistortionParams distortionParams {};
    distortion.load(distortionParams);

    Raz::SoundEffect echo;
    Raz::EchoParams echoParams {};
    echo.load(echoParams);

    Raz::SoundEffect* enabledEffect = nullptr;

    Raz::SoundEffectSlot effectSlot;
#endif

    /////////////
    // Overlay //
    /////////////

    bool isRepeating = false;
    bool moveSource  = false;

    std::string captureDevice;
    bool isCapturing        = false;
    bool isCaptureStereo    = false;
    uint8_t captureBitDepth = 8;

    Raz::OverlayWindow& overlayAudio = window.getOverlay().addWindow("Audio info", overlayAudioSize);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    overlayAudio.addDropdown("Output device", Raz::AudioSystem::recoverDevices(), [&] (const std::string& name, std::size_t) {
      const float listenerGain = listener.recoverGain();
      const float soundGain    = soundComp.recoverGain();
      const float soundPitch   = soundComp.recoverPitch();

      audio.openDevice(name);

      listener.setGain(listenerGain);

      soundComp.init();
      soundComp.setRepeat(isRepeating);
      soundComp.setGain(soundGain);
      soundComp.setPitch(soundPitch);

      reverb.init();
      reverb.load(reverbParams);

      chorus.init();
      chorus.load(chorusParams);

      distortion.init();
      distortion.load(distortionParams);

      echo.init();
      echo.load(echoParams);

      effectSlot.init();

      if (enabledEffect) {
        effectSlot.loadEffect(*enabledEffect);
        soundComp.linkSlot(effectSlot);
      }
    });

    overlayAudio.addDropdown("Input device",
                             Raz::Microphone::recoverDevices(),
                             [&captureDevice, &microphone, &isCaptureStereo, &captureBitDepth] (const std::string& name, std::size_t) {
      captureDevice = name;
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f, captureDevice);
    });
#else
    overlayAudio.addLabel("Output & input devices cannot be changed with Emscripten\n"
                          "Reload the page if you changed them externally");
#endif

    overlayAudio.addSlider("Listener gain", [&listener] (float val) noexcept {
      listener.setGain(val);
    }, 0.f, 1.f, 1.f);

    ///////////
    // Sound //
    ///////////

    Raz::OverlayWindow& overlaySound = window.getOverlay().addWindow("Sound params", overlaySoundSize, Raz::Vec2f(0.f, overlayAudioSize.y()));

    overlaySound.addDropdown("Sound", { "Knock", "Wave seagulls" }, [&] (const std::string&, std::size_t i) {
      const float gain  = soundComp.recoverGain();
      const float pitch = soundComp.recoverPitch();

      switch (i) {
        case 0:
        default:
          soundComp.load(knockAudio);
          break;

        case 1:
          soundComp.load(waveSeagullsAudio);
          break;
      }

      soundComp.setRepeat(isRepeating);
      soundComp.setGain(gain);
      soundComp.setPitch(pitch);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
      if (enabledEffect)
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

    ////////////////
    // Microphone //
    ////////////////

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
                           [&isCaptureStereo, &captureBitDepth, &microphone, &captureDevice,
                            &isCapturing, &stereoPlot, &monoPlot] (const std::string&, std::size_t i) {
      isCaptureStereo = (i == 1);
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f, captureDevice);
      isCapturing = false;

      stereoPlot.enable(isCaptureStereo);
      monoPlot.enable(!isCaptureStereo);
    }, 0);

    overlayMic.addDropdown("Bit depth", { "8", "16" }, [&captureBitDepth, &isCaptureStereo, &microphone, &captureDevice,
                                                        &isCapturing] (const std::string&, std::size_t i) {
      captureBitDepth = (i == 0 ? 8 : 16);
      microphone.openDevice(recoverFormat(isCaptureStereo, captureBitDepth), 16000, 1.f, captureDevice);
      isCapturing = false;
    }, 0);

    ///////////////////
    // Sound effects //
    ///////////////////

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    Raz::OverlayWindow& overlayReverb = window.getOverlay().addWindow("Reverb params", overlayReverbSize, Raz::Vec2f(baseWindowWidth - 20, 0.f));
    overlayReverb.disable();
    Raz::OverlayWindow& overlayChorus = window.getOverlay().addWindow("Chorus params", overlayChorusSize, Raz::Vec2f(baseWindowWidth - 20, 0.f));
    overlayChorus.disable();
    Raz::OverlayWindow& overlayDistortion = window.getOverlay().addWindow("Distortion params", overlayDistortionSize, Raz::Vec2f(baseWindowWidth - 20, 0.f));
    overlayDistortion.disable();
    Raz::OverlayWindow& overlayEcho = window.getOverlay().addWindow("Echo params", overlayEchoSize, Raz::Vec2f(baseWindowWidth - 20, 0.f));
    overlayEcho.disable();

    overlaySound.addDropdown("Sound effect",
                             { "None", "Reverberation", "Chorus", "Distortion", "Echo" },
                             [&] (const std::string&, std::size_t i) noexcept {
      overlayReverb.enable(i == 1);
      overlayChorus.enable(i == 2);
      overlayDistortion.enable(i == 3);
      overlayEcho.enable(i == 4);

      if (i == 0) {
        enabledEffect = nullptr;
        soundComp.unlinkSlot();
        window.resize(baseWindowWidth, baseWindowHeight);
        return;
      }

      unsigned int newWindowWidth = baseWindowWidth;
      unsigned int overlayHeight {};

      switch (i) {
        case 1:
          enabledEffect   = &reverb;
          newWindowWidth += static_cast<unsigned int>(overlayReverbSize.x());
          overlayHeight   = static_cast<unsigned int>(overlayReverbSize.y());
          break;

        case 2:
          enabledEffect   = &chorus;
          newWindowWidth += static_cast<unsigned int>(overlayChorusSize.x());
          overlayHeight   = static_cast<unsigned int>(overlayChorusSize.y());
          break;

        case 3:
          enabledEffect   = &distortion;
          newWindowWidth += static_cast<unsigned int>(overlayDistortionSize.x());
          overlayHeight   = static_cast<unsigned int>(overlayDistortionSize.y());
          break;

        case 4:
          enabledEffect   = &echo;
          newWindowWidth += static_cast<unsigned int>(overlayEchoSize.x());
          overlayHeight   = static_cast<unsigned int>(overlayEchoSize.y());
          break;

        default:
          break;
      }

      window.resize(newWindowWidth - 40, std::max(baseWindowHeight, overlayHeight));

      effectSlot.loadEffect(*enabledEffect);
      soundComp.linkSlot(effectSlot);
    }, 0);

    /////////////////////////
    // Reveberation params //
    /////////////////////////

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

    ///////////////////
    // Chorus params //
    ///////////////////

    overlayChorus.addDropdown("Waveform", { "Sinusoid", "Triangle" }, [&chorusParams, &chorus, &effectSlot] (const std::string&, std::size_t i) {
      chorusParams.waveform = static_cast<Raz::SoundWaveform>(i);
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, static_cast<std::size_t>(chorusParams.waveform));

    overlayChorus.addSlider("Phase", [&chorusParams, &chorus, &effectSlot] (float val) {
      chorusParams.phase = static_cast<int>(val);
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, -180.f, 180.f, static_cast<float>(chorusParams.phase));

    overlayChorus.addSlider("Rate", [&chorusParams, &chorus, &effectSlot] (float val) {
      chorusParams.rate = val;
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, 0.f, 10.f, chorusParams.rate);

    overlayChorus.addSlider("Depth", [&chorusParams, &chorus, &effectSlot] (float val) {
      chorusParams.depth = val;
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, 0.f, 1.f, chorusParams.depth);

    overlayChorus.addSlider("Feedback", [&chorusParams, &chorus, &effectSlot] (float val) {
      chorusParams.feedback = val;
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, -1.f, 1.f, chorusParams.feedback);

    overlayChorus.addSlider("Delay", [&chorusParams, &chorus, &effectSlot] (float val) {
      chorusParams.delay = val;
      chorus.load(chorusParams);
      effectSlot.loadEffect(chorus);
    }, 0.f, 0.016f, chorusParams.delay);

    ///////////////////////
    // Distortion params //
    ///////////////////////

    overlayDistortion.addSlider("Edge", [&distortionParams, &distortion, &effectSlot] (float val) {
      distortionParams.edge = val;
      distortion.load(distortionParams);
      effectSlot.loadEffect(distortion);
    }, 0.f, 1.f, distortionParams.edge);

    overlayDistortion.addSlider("Gain", [&distortionParams, &distortion, &effectSlot] (float val) {
      distortionParams.gain = val;
      distortion.load(distortionParams);
      effectSlot.loadEffect(distortion);
    }, 0.01f, 1.f, distortionParams.gain);

    overlayDistortion.addSlider("Lowpass cutoff", [&distortionParams, &distortion, &effectSlot] (float val) {
      distortionParams.lowpassCutoff = val;
      distortion.load(distortionParams);
      effectSlot.loadEffect(distortion);
    }, 80.f, 24000.f, distortionParams.lowpassCutoff);

    overlayDistortion.addSlider("Eq. center", [&distortionParams, &distortion, &effectSlot] (float val) {
      distortionParams.eqCenter = val;
      distortion.load(distortionParams);
      effectSlot.loadEffect(distortion);
    }, 80.f, 24000.f, distortionParams.eqCenter);

    overlayDistortion.addSlider("Eq. bandwidth", [&distortionParams, &distortion, &effectSlot] (float val) {
      distortionParams.eqBandwidth = val;
      distortion.load(distortionParams);
      effectSlot.loadEffect(distortion);
    }, 80.f, 24000.f, distortionParams.eqBandwidth);

    /////////////////
    // Echo params //
    /////////////////

    overlayEcho.addSlider("Delay", [&echoParams, &echo, &effectSlot] (float val) {
      echoParams.delay = val;
      echo.load(echoParams);
      effectSlot.loadEffect(echo);
    }, 0.f, 0.207f, echoParams.delay);

    overlayEcho.addSlider("Left/right delay", [&echoParams, &echo, &effectSlot] (float val) {
      echoParams.leftRightDelay = val;
      echo.load(echoParams);
      effectSlot.loadEffect(echo);
    }, 0.f, 0.404f, echoParams.leftRightDelay);

    overlayEcho.addSlider("Damping", [&echoParams, &echo, &effectSlot] (float val) {
      echoParams.damping = val;
      echo.load(echoParams);
      effectSlot.loadEffect(echo);
    }, 0.f, 0.99f, echoParams.damping);

    overlayEcho.addSlider("Feedback", [&echoParams, &echo, &effectSlot] (float val) {
      echoParams.feedback = val;
      echo.load(echoParams);
      effectSlot.loadEffect(echo);
    }, 0.f, 1.f, echoParams.feedback);

    overlayEcho.addSlider("Spread", [&echoParams, &echo, &effectSlot] (float val) {
      echoParams.spread = val;
      echo.load(echoParams);
      effectSlot.loadEffect(echo);
    }, -1.f, 1.f, echoParams.spread);
#endif

    //////////////////////////
    // Starting application //
    //////////////////////////

    Raz::AudioData captureData;

    app.run([&] (const Raz::FrameTimeInfo& timeInfo) {
      soundTrans.setPosition((moveSource ? Raz::Vec3f(std::sin(timeInfo.globalTime) * 3.f, 0.f, 1.f) : Raz::Vec3f(0.f)));

      if (isCapturing) {
        microphone.recoverData(captureData);

        // Stereo data is interleaved: the left capture is first, then the right (L, R, L, R, ...)

        // 8-bit depth values are unsigned and have a range between [0; 255]. They are remapped to [-1; 1]

        // 16-bit depth values are assumed to be in little endian, and are converted to big endian (each 2-bytes pack [AB] is converted to [BA])
        // They are signed and have a range between [-32768; 32767]. They are remapped to [-1; 1]

        constexpr float factorU8  = 1.f / 128;
        constexpr float factorI16 = 1.f / 32767;

        if (captureBitDepth == 8) {
          for (std::size_t i = 0; i < captureData.buffer.size(); ++i) {
            if (isCaptureStereo) { // Stereo 8
              leftCapturePlot.push(static_cast<float>(captureData.buffer[i]) * factorU8 - 1.f);
              rightCapturePlot.push(static_cast<float>(captureData.buffer[i + 1]) * factorU8 - 1.f);
              ++i;
            } else { // Mono 8
              monoCapturePlot.push(static_cast<float>(captureData.buffer[i]) * factorU8 - 1.f);
            }
          }
        } else {
          for (std::size_t i = 0; i < captureData.buffer.size(); i += 2) {
            if (isCaptureStereo) { // Stereo 16
              leftCapturePlot.push(static_cast<float>(static_cast<int16_t>(captureData.buffer[i] | (captureData.buffer[i + 1] << 8u))) * factorI16);
              rightCapturePlot.push(static_cast<float>(static_cast<int16_t>(captureData.buffer[i + 2] | (captureData.buffer[i + 3] << 8u))) * factorI16);
              i += 2;
            } else { // Mono 16
              monoCapturePlot.push(static_cast<float>(static_cast<int16_t>(captureData.buffer[i] | (captureData.buffer[i + 1] << 8u))) * factorI16);
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
