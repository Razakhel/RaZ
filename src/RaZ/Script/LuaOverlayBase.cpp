#include "RaZ/Render/Overlay.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerOverlayBaseTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Overlay> overlay = state.new_usertype<Overlay>("Overlay",
                                                                 sol::constructors<Overlay()>());
    overlay["isEmpty"]          = &Overlay::isEmpty;
    overlay["addWindow"]        = sol::overload([] (Overlay& o, std::string t) { return &o.addWindow(std::move(t)); },
                                                [] (Overlay& o, std::string t, const Vec2f& s) { return &o.addWindow(std::move(t), s); },
                                                PickOverload<std::string, const Vec2f&, const Vec2f&>(&Overlay::addWindow));
    overlay["hasKeyboardFocus"] = &Overlay::hasKeyboardFocus;
    overlay["hasMouseFocus"]    = &Overlay::hasMouseFocus;
    overlay["render"]           = &Overlay::render;
  }

  {
    sol::usertype<OverlayWindow> overlayWindow = state.new_usertype<OverlayWindow>("OverlayWindow",
                                                                                   sol::constructors<OverlayWindow(std::string),
                                                                                                     OverlayWindow(std::string, const Vec2f&),
                                                                                                     OverlayWindow(std::string, const Vec2f&, const Vec2f&)>());
    overlayWindow["isEnabled"]       = &OverlayWindow::isEnabled;
    overlayWindow["enable"]          = sol::overload([] (OverlayWindow& r) { r.enable(); },
                                                     PickOverload<bool>(&OverlayWindow::enable));
    overlayWindow["disable"]         = &OverlayWindow::disable;
    overlayWindow["addLabel"]        = &OverlayWindow::addLabel;
    overlayWindow["addColoredLabel"] = sol::overload([] (OverlayWindow& w, std::string t, const Color& c) { return &w.addColoredLabel(std::move(t), c); },
                                                     PickOverload<std::string, const Color&, float>(&OverlayWindow::addColoredLabel),
                                                     [] (OverlayWindow& w, std::string t,
                                                         float r, float g, float b) { return &w.addColoredLabel(std::move(t), r, g, b); },
                                                     PickOverload<std::string, float, float, float, float>(&OverlayWindow::addColoredLabel));
    overlayWindow["addButton"]       = &OverlayWindow::addButton;
    overlayWindow["addCheckbox"]     = &OverlayWindow::addCheckbox;
    overlayWindow["addSlider"]       = &OverlayWindow::addSlider;
    overlayWindow["addTextbox"]      = sol::overload([] (OverlayWindow& w, std::string l,
                                                         std::function<void(const std::string&)> c) { return &w.addTextbox(std::move(l),
                                                                                                                           std::move(c)); },
                                                     PickOverload<std::string, std::function<void(const std::string&)>,
                                                                  std::string>(&OverlayWindow::addTextbox));
    overlayWindow["addTextArea"]     = sol::overload([] (OverlayWindow& w, std::string l,
                                                         std::function<void(const std::string&)> c) { return &w.addTextArea(std::move(l), std::move(c)); },
                                                     [] (OverlayWindow& w, std::string l, std::function<void(const std::string&)> c,
                                                         std::string t) { return &w.addTextArea(std::move(l), std::move(c), std::move(t)); },
                                                     PickOverload<std::string, std::function<void(const std::string&)>,
                                                                  std::string, float>(&OverlayWindow::addTextArea));
    overlayWindow["addListBox"]      = sol::overload([] (OverlayWindow& w, std::string l, std::vector<std::string> e,
                                                         std::function<void(const std::string&, std::size_t)> c) { return &w.addListBox(std::move(l),
                                                                                                                                        std::move(e),
                                                                                                                                        std::move(c)); },
                                                     PickOverload<std::string, std::vector<std::string>, std::function<void(const std::string&, std::size_t)>,
                                                                  std::size_t>(&OverlayWindow::addListBox));
    overlayWindow["addDropdown"]     = sol::overload([] (OverlayWindow& w, std::string l, std::vector<std::string> e,
                                                         std::function<void(const std::string&, std::size_t)> c) { return &w.addDropdown(std::move(l),
                                                                                                                                         std::move(e),
                                                                                                                                         std::move(c)); },
                                                     PickOverload<std::string, std::vector<std::string>, std::function<void(const std::string&, std::size_t)>,
                                                                  std::size_t>(&OverlayWindow::addDropdown));
    overlayWindow["addTexture"]      = sol::overload(PickOverload<const Texture2D&, unsigned int, unsigned int>(&OverlayWindow::addTexture),
                                                     PickOverload<const Texture2D&>(&OverlayWindow::addTexture));
    overlayWindow["addProgressBar"]  = sol::overload([] (OverlayWindow& w, int min, int max) { return &w.addProgressBar(min, max); },
                                                     PickOverload<int, int, bool>(&OverlayWindow::addProgressBar));
    overlayWindow["addPlot"]         = sol::overload([] (OverlayWindow& w, std::string l, std::size_t c) { return &w.addPlot(std::move(l), c); },
                                                     [] (OverlayWindow& w, std::string l, std::size_t c,
                                                         std::string x) { return &w.addPlot(std::move(l), c, std::move(x)); },
                                                     [] (OverlayWindow& w, std::string l, std::size_t c, std::string x,
                                                         std::string y) { return &w.addPlot(std::move(l), c, std::move(x), std::move(y)); },
                                                     [] (OverlayWindow& w, std::string l, std::size_t c, std::string x, std::string y,
                                                         float min) { return &w.addPlot(std::move(l), c, std::move(x), std::move(y), min); },
                                                     [] (OverlayWindow& w, std::string l, std::size_t c, std::string x, std::string y, float min,
                                                         float max) { return &w.addPlot(std::move(l), c, std::move(x), std::move(y), min, max); },
                                                     [] (OverlayWindow& w, std::string l, std::size_t c, std::string x, std::string y, float min, float max,
                                                         bool lock) { return &w.addPlot(std::move(l), c, std::move(x), std::move(y), min, max, lock); },
                                                     PickOverload<std::string, std::size_t, std::string, std::string,
                                                                  float, float, bool, float>(&OverlayWindow::addPlot));
    overlayWindow["addSeparator"]    = &OverlayWindow::addSeparator;
    overlayWindow["addFrameTime"]    = &OverlayWindow::addFrameTime;
    overlayWindow["addFpsCounter"]   = &OverlayWindow::addFpsCounter;
    overlayWindow["render"]          = &OverlayWindow::render;
  }
}

} // namespace Raz
