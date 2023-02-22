#include "RaZ/Render/Overlay.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerOverlayTypes() {
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
    state.new_usertype<OverlayButton>("OverlayButton",
                                      sol::constructors<OverlayButton(std::string, std::function<void()>)>(),
                                      sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    state.new_usertype<OverlayCheckbox>("OverlayCheckbox",
                                        sol::constructors<OverlayCheckbox(std::string, std::function<void()>, std::function<void()>, bool)>(),
                                        sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    sol::usertype<OverlayColoredLabel> overlayColoredLabel = state.new_usertype<OverlayColoredLabel>("OverlayColoredLabel",
                                                                                                     sol::constructors<OverlayColoredLabel(std::string,
                                                                                                                                           const Color&),
                                                                                                                       OverlayColoredLabel(std::string,
                                                                                                                                           const Color&,
                                                                                                                                           float)>(),
                                                                                                     sol::base_classes, sol::bases<OverlayElement>());
    overlayColoredLabel["text"]  = sol::property(&OverlayColoredLabel::getText, PickOverload<std::string>(&OverlayColoredLabel::setText));
    overlayColoredLabel["color"] = sol::property(&OverlayColoredLabel::getColor, [] (OverlayColoredLabel& l, const Color& c) { l.setColor(c); });
    overlayColoredLabel["alpha"] = sol::property(&OverlayColoredLabel::getAlpha, &OverlayColoredLabel::setAlpha);
  }

  {
    state.new_usertype<OverlayDropdown>("OverlayDropdown",
                                        sol::constructors<OverlayDropdown(std::string, std::vector<std::string>,
                                                                          std::function<void(const std::string&, std::size_t)>),
                                                          OverlayDropdown(std::string, std::vector<std::string>,
                                                                          std::function<void(const std::string&, std::size_t)>, std::size_t)>(),
                                        sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    sol::usertype<OverlayElement> overlayElement = state.new_usertype<OverlayElement>("OverlayElement", sol::no_constructor);
    overlayElement["getType"]   = &OverlayElement::getType;
    overlayElement["isEnabled"] = &OverlayElement::isEnabled;
    overlayElement["enable"]    = sol::overload([] (OverlayElement& r) { r.enable(); },
                                                PickOverload<bool>(&OverlayElement::enable));
    overlayElement["disable"]   = &OverlayElement::disable;
  }

  {
    state.new_usertype<OverlayFpsCounter>("OverlayFpsCounter",
                                          sol::constructors<OverlayFpsCounter(std::string)>(),
                                          sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    state.new_usertype<OverlayFrameTime>("OverlayFrameTime",
                                         sol::constructors<OverlayFrameTime(std::string)>(),
                                         sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    sol::usertype<OverlayLabel> overlayLabel = state.new_usertype<OverlayLabel>("OverlayLabel",
                                                                                sol::constructors<OverlayLabel(std::string)>(),
                                                                                sol::base_classes, sol::bases<OverlayElement>());
    overlayLabel["text"] = sol::property(&OverlayLabel::getText, &OverlayLabel::setText);
  }

  {
    state.new_usertype<OverlayListBox>("OverlayListBox",
                                       sol::constructors<OverlayListBox(std::string, std::vector<std::string>,
                                                                        std::function<void(const std::string&, std::size_t)>),
                                                         OverlayListBox(std::string, std::vector<std::string>,
                                                                        std::function<void(const std::string&, std::size_t)>, std::size_t)>(),
                                       sol::base_classes, sol::bases<OverlayElement>());
  }

  // OverlayPlot
  {
    {
      sol::usertype<OverlayPlot> overlayPlot = state.new_usertype<OverlayPlot>("OverlayPlot",
                                                                               sol::constructors<OverlayPlot(std::string, std::size_t),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string,
                                                                                                             std::string),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string, std::string,
                                                                                                             float),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string, std::string,
                                                                                                             float, float),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string, std::string,
                                                                                                             float, float, bool),
                                                                                                 OverlayPlot(std::string, std::size_t, std::string, std::string,
                                                                                                             float, float, bool, float)>(),
                                                                               sol::base_classes, sol::bases<OverlayElement>());
      overlayPlot["addEntry"] = sol::overload([] (OverlayPlot& p, std::string n) { return p.addEntry(std::move(n)); },
                                              PickOverload<std::string, OverlayPlotType>(&OverlayPlot::addEntry));
    }

    {
      sol::usertype<OverlayPlotEntry> overlayPlotEntry = state.new_usertype<OverlayPlotEntry>("OverlayPlotEntry", sol::no_constructor);
      overlayPlotEntry["push"] = &OverlayPlotEntry::push;
    }

    state.new_enum<OverlayPlotType>("OverlayPlotType", {
      { "LINE",   OverlayPlotType::LINE },
      { "SHADED", OverlayPlotType::SHADED }
    });
  }

  {
    sol::usertype<OverlayProgressBar> overlayProgressBar = state.new_usertype<OverlayProgressBar>("OverlayProgressBar",
                                                                                                  sol::constructors<OverlayProgressBar(int, int),
                                                                                                                    OverlayProgressBar(int, int, bool)>(),
                                                                                                  sol::base_classes, sol::bases<OverlayElement>());
    overlayProgressBar["currentValue"] = sol::property(&OverlayProgressBar::getCurrentValue, &OverlayProgressBar::setCurrentValue);
    overlayProgressBar["hasStarted"]   = &OverlayProgressBar::hasStarted;
    overlayProgressBar["hasFinished"]  = &OverlayProgressBar::hasFinished;
    overlayProgressBar["add"]          = &OverlayProgressBar::operator+=;
  }

  {
    state.new_usertype<OverlaySeparator>("OverlaySeparator",
                                         sol::constructors<OverlaySeparator()>(),
                                         sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    state.new_usertype<OverlaySlider>("OverlaySlider",
                                      sol::constructors<OverlaySlider(std::string, std::function<void(float)>, float, float, float)>(),
                                      sol::base_classes, sol::bases<OverlayElement>());
  }

  {
    sol::usertype<OverlayTextbox> overlayTextbox = state.new_usertype<OverlayTextbox>("OverlayTextbox",
                                                                                      sol::constructors<
                                                                                        OverlayTextbox(std::string,
                                                                                                       std::function<void(const std::string&)>)
                                                                                      >(),
                                                                                      sol::base_classes, sol::bases<OverlayElement>());
    overlayTextbox["text"]   = sol::property(&OverlayTextbox::getText, &OverlayTextbox::setText);
    overlayTextbox["append"] = &OverlayTextbox::append;
    overlayTextbox["clear"]  = &OverlayTextbox::clear;
  }

  {
    sol::usertype<OverlayTexture> overlayTexture = state.new_usertype<OverlayTexture>("OverlayTexture",
                                                                                      sol::constructors<OverlayTexture(const Texture2D&, unsigned int,
                                                                                                                       unsigned int),
                                                                                                        OverlayTexture(const Texture2D&)>(),
                                                                                      sol::base_classes, sol::bases<OverlayElement>());
    overlayTexture["setTexture"] = sol::overload(PickOverload<const Texture2D&, unsigned int, unsigned int>(&OverlayTexture::setTexture),
                                                 PickOverload<const Texture2D&>(&OverlayTexture::setTexture));
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
    overlayWindow["addTextbox"]      = &OverlayWindow::addTextbox;
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

  state.new_enum<OverlayElementType>("OverlayElementType", {
    { "LABEL",         OverlayElementType::LABEL },
    { "COLORED_LABEL", OverlayElementType::COLORED_LABEL },
    { "BUTTON",        OverlayElementType::BUTTON },
    { "CHECKBOX",      OverlayElementType::CHECKBOX },
    { "SLIDER",        OverlayElementType::SLIDER },
    { "TEXTBOX",       OverlayElementType::TEXTBOX },
    { "LIST_BOX",      OverlayElementType::LIST_BOX },
    { "DROPDOWN",      OverlayElementType::DROPDOWN },
    { "TEXTURE",       OverlayElementType::TEXTURE },
    { "PROGRESS_BAR",  OverlayElementType::PROGRESS_BAR },
    { "PLOT",          OverlayElementType::PLOT },
    { "SEPARATOR",     OverlayElementType::SEPARATOR },
    { "FRAME_TIME",    OverlayElementType::FRAME_TIME },
    { "FPS_COUNTER",   OverlayElementType::FPS_COUNTER }
  });
}

} // namespace Raz
