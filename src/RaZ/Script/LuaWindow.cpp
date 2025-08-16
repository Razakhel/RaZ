#include "RaZ/Data/Image.hpp"
#include "RaZ/Render/RenderSystem.hpp"
#include "RaZ/Render/Window.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerWindowTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Window> window = state.new_usertype<Window>("Window",
                                                              sol::constructors<Window(RenderSystem&, unsigned int, unsigned int),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&,
                                                                                       WindowSetting),
                                                                                Window(RenderSystem&, unsigned int, unsigned int, const std::string&,
                                                                                       WindowSetting, uint8_t)>());
    window["getWidth"]                 = &Window::getWidth;
    window["getHeight"]                = &Window::getHeight;
#if !defined(RAZ_NO_OVERLAY)
    window["getOverlay"]               = &Window::getOverlay;
#endif
    window["setClearColor"]            = sol::overload([] (const Window& w, const Color& c) { w.setClearColor(c); },
                                                       PickOverload<const Color&, float>(&Window::setClearColor),
                                                       [] (const Window& w, float r, float g, float b) { w.setClearColor(r, g, b); },
                                                       PickOverload<float, float, float, float>(&Window::setClearColor));
    window["setTitle"]                 = &Window::setTitle;
    window["setIcon"]                  = &Window::setIcon;
    window["resize"]                   = &Window::resize;
    window["makeFullscreen"]           = &Window::makeFullscreen;
    window["makeWindowed"]             = &Window::makeWindowed;
    window["enableFaceCulling"]        = sol::overload([] (const Window& w) { w.enableFaceCulling(); },
                                                       PickOverload<bool>(&Window::enableFaceCulling));
    window["disableFaceCulling"]       = &Window::disableFaceCulling;
    window["recoverVerticalSyncState"] = &Window::recoverVerticalSyncState;
    window["enableVerticalSync"]       = sol::overload([] (const Window& w) { w.enableVerticalSync(); },
                                                       PickOverload<bool>(&Window::enableVerticalSync));
    window["disableVerticalSync"]      = &Window::disableVerticalSync;
    window["showCursor"]               = &Window::showCursor;
    window["hideCursor"]               = &Window::hideCursor;
    window["disableCursor"]            = &Window::disableCursor;
    window["addKeyCallback"]           = sol::overload([] (Window& w, Keyboard::Key k, std::function<void(float)> c) { w.addKeyCallback(k, std::move(c)); },
                                                       [] (Window& w, Keyboard::Key k, std::function<void(float)> c,
                                                           Input::ActionTrigger f) { w.addKeyCallback(k, std::move(c), f); },
                                                       PickOverload<Keyboard::Key, std::function<void(float)>,
                                                                    Input::ActionTrigger, std::function<void()>>(&Window::addKeyCallback));
    window["addMouseButtonCallback"]   = sol::overload([] (Window& w, Mouse::Button b,
                                                           std::function<void(float)> c) { w.addMouseButtonCallback(b, std::move(c)); },
                                                       [] (Window& w, Mouse::Button b, std::function<void(float)> c,
                                                           Input::ActionTrigger f) { w.addMouseButtonCallback(b, std::move(c), f); },
                                                       PickOverload<Mouse::Button, std::function<void(float)>, Input::ActionTrigger,
                                                                    std::function<void()>>(&Window::addMouseButtonCallback));
    window["setMouseScrollCallback"]   = &Window::setMouseScrollCallback;
    window["setMouseMoveCallback"]     = &Window::setMouseMoveCallback;
    window["setCloseCallback"]         = &Window::setCloseCallback;
    window["updateCallbacks"]          = &Window::updateCallbacks;
#if !defined(RAZ_NO_OVERLAY)
    window["enableOverlay"]            = sol::overload([] (Window& w) { w.enableOverlay(); },
                                                       PickOverload<bool>(&Window::enableOverlay));
    window["disableOverlay"]           = &Window::disableOverlay;
#endif
    window["run"]                      = &Window::run;
    window["recoverMousePosition"]     = &Window::recoverMousePosition;

    state.new_enum<WindowSetting>("WindowSetting", {
      { "FOCUSED",        WindowSetting::FOCUSED },
      { "RESIZABLE",      WindowSetting::RESIZABLE },
      { "VISIBLE",        WindowSetting::VISIBLE },
      { "DECORATED",      WindowSetting::DECORATED },
      { "AUTO_MINIMIZE",  WindowSetting::AUTO_MINIMIZE },
      { "ALWAYS_ON_TOP",  WindowSetting::ALWAYS_ON_TOP },
      { "MAXIMIZED",      WindowSetting::MAXIMIZED },
#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
      { "CENTER_CURSOR",  WindowSetting::CENTER_CURSOR },
      { "TRANSPARENT_FB", WindowSetting::TRANSPARENT_FB },
      { "AUTOFOCUS",      WindowSetting::AUTOFOCUS },
#endif
      { "DEFAULT",        WindowSetting::DEFAULT },
      { "NON_RESIZABLE",  WindowSetting::NON_RESIZABLE },
      { "WINDOWED",       WindowSetting::WINDOWED },
      { "BORDERLESS",     WindowSetting::BORDERLESS },
      { "INVISIBLE",      WindowSetting::INVISIBLE }
    });

    state.new_enum<Input::ActionTrigger>("Input", {
      { "ONCE",   Input::ONCE },
      { "ALWAYS", Input::ALWAYS }
    });

    state.new_enum<Keyboard::Key>("Keyboard", {
      { "A", Keyboard::A },
      { "B", Keyboard::B },
      { "C", Keyboard::C },
      { "D", Keyboard::D },
      { "E", Keyboard::E },
      { "F", Keyboard::F },
      { "G", Keyboard::G },
      { "H", Keyboard::H },
      { "I", Keyboard::I },
      { "J", Keyboard::J },
      { "K", Keyboard::K },
      { "L", Keyboard::L },
      { "M", Keyboard::M },
      { "N", Keyboard::N },
      { "O", Keyboard::O },
      { "P", Keyboard::P },
      { "Q", Keyboard::Q },
      { "R", Keyboard::R },
      { "S", Keyboard::S },
      { "T", Keyboard::T },
      { "U", Keyboard::U },
      { "V", Keyboard::V },
      { "W", Keyboard::W },
      { "X", Keyboard::X },
      { "Y", Keyboard::Y },
      { "Z", Keyboard::Z },

      { "F1",  Keyboard::F1 },
      { "F2",  Keyboard::F2 },
      { "F3",  Keyboard::F3 },
      { "F4",  Keyboard::F4 },
      { "F5",  Keyboard::F5 },
      { "F6",  Keyboard::F6 },
      { "F7",  Keyboard::F7 },
      { "F8",  Keyboard::F8 },
      { "F9",  Keyboard::F9 },
      { "F10", Keyboard::F10 },
      { "F11", Keyboard::F11 },
      { "F12", Keyboard::F12 },

      { "UP",    Keyboard::UP },
      { "DOWN",  Keyboard::DOWN },
      { "RIGHT", Keyboard::RIGHT },
      { "LEFT",  Keyboard::LEFT },

      { "NUMLOCK",  Keyboard::NUMLOCK },
      { "NUM0",     Keyboard::NUM0 },
      { "NUM1",     Keyboard::NUM1 },
      { "NUM2",     Keyboard::NUM2 },
      { "NUM3",     Keyboard::NUM3 },
      { "NUM4",     Keyboard::NUM4 },
      { "NUM5",     Keyboard::NUM5 },
      { "NUM6",     Keyboard::NUM6 },
      { "NUM7",     Keyboard::NUM7 },
      { "NUM8",     Keyboard::NUM8 },
      { "NUM9",     Keyboard::NUM9 },
      { "DECIMAL",  Keyboard::DECIMAL },
      { "DIVIDE",   Keyboard::DIVIDE },
      { "MULTIPLY", Keyboard::MULTIPLY },
      { "SUBTRACT", Keyboard::SUBTRACT },
      { "ADD",      Keyboard::ADD },

      { "LEFT_SHIFT",  Keyboard::LEFT_SHIFT },
      { "RIGHT_SHIFT", Keyboard::RIGHT_SHIFT },
      { "LEFT_CTRL",   Keyboard::LEFT_CTRL },
      { "RIGHT_CTRL",  Keyboard::RIGHT_CTRL },
      { "LEFT_ALT",    Keyboard::LEFT_ALT },
      { "RIGHT_ALT",   Keyboard::RIGHT_ALT },

      { "HOME",         Keyboard::HOME },
      { "END",          Keyboard::END },
      { "PAGEUP",       Keyboard::PAGEUP },
      { "PAGEDOWN",     Keyboard::PAGEDOWN },
      { "CAPSLOCK",     Keyboard::CAPSLOCK },
      { "SPACE",        Keyboard::SPACE },
      { "BACKSPACE",    Keyboard::BACKSPACE },
      { "INSERT",       Keyboard::INSERT },
      { "ESCAPE",       Keyboard::ESCAPE },
      { "PRINT_SCREEN", Keyboard::PRINT_SCREEN },
      { "PAUSE",        Keyboard::PAUSE }
    });

    state.new_enum<Mouse::Button>("Mouse", {
      { "LEFT_CLICK",   Mouse::LEFT_CLICK },
      { "RIGHT_CLICK",  Mouse::RIGHT_CLICK },
      { "MIDDLE_CLICK", Mouse::MIDDLE_CLICK }
    });
  }
}

} // namespace Raz
