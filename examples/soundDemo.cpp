#include "RaZ/RaZ.hpp"

#include <iostream>

using namespace std::literals;

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

    Raz::Window& window = world.addSystem<Raz::RenderSystem>(375, 270, "RaZ", Raz::WindowSetting::NON_RESIZABLE).getWindow();

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

    bool isRepeating   = false;
    bool moveSource    = false;
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    bool effectEnabled = false;
#endif

    Raz::OverlayWindow& audioInfo = window.getOverlay().addWindow("Audio info", Raz::Vec2f(window.getWidth(), 55.f));

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    audioInfo.addDropdown("Output device",
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
#else
    audioInfo.addLabel("Changing output device unavailable with Emscripten");
#endif

    Raz::OverlayWindow& overlaySound = window.getOverlay().addWindow("Sound params",
                                                                     Raz::Vec2f(window.getWidth(), window.getHeight() - 55),
                                                                     Raz::Vec2f(0.f, 55.f));

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

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
    Raz::OverlayWindow& overlayReverb = window.getOverlay().addWindow("Reverb params",
                                                                      Raz::Vec2f(window.getWidth() * 1.3, window.getHeight() * 2.45),
                                                                      Raz::Vec2f(window.getWidth() - 20, 0.f));
    overlayReverb.disable();

    overlaySound.addCheckbox("Enable reverb", [&effectEnabled, &soundComp, &effectSlot, &overlayReverb, &window] () noexcept {
      effectEnabled = true;
      soundComp.linkSlot(effectSlot);
      overlayReverb.enable();
      window.resize(static_cast<unsigned int>(window.getWidth() * 2.2), static_cast<unsigned int>(window.getHeight() * 2.45));
    }, [&effectEnabled, &soundComp, &overlayReverb, &window] () noexcept {
      effectEnabled = false;
      soundComp.unlinkSlot();
      overlayReverb.disable();
      window.resize(static_cast<unsigned int>(window.getWidth() / 2.2), static_cast<unsigned int>(window.getHeight() / 2.45));
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

      soundTrans.setPosition((moveSource ? Raz::Vec3f(std::sin(globalTime), 0.f, 1.f) : Raz::Vec3f(0.f)));
    });
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occured: "s + exception.what());
  }

  return EXIT_SUCCESS;
}
