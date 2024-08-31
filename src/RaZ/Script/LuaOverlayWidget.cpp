#include "RaZ/Render/Overlay.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerOverlayWidgetTypes() {
  sol::state& state = getState();

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
    state.new_usertype<OverlayColorPicker>("OverlayColorPicker",
                                           sol::constructors<OverlayColorPicker(std::string, std::function<void(const Color&)>, const Color&)>(),
                                           sol::base_classes, sol::bases<OverlayElement>());
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
    sol::usertype<OverlayTextArea> overlayTextArea = state.new_usertype<OverlayTextArea>("OverlayTextArea",
                                                                                         sol::constructors<
                                                                                           OverlayTextArea(std::string,
                                                                                                           std::function<void(const std::string&)>),
                                                                                           OverlayTextArea(std::string,
                                                                                                           std::function<void(const std::string&)>,
                                                                                                           std::string),
                                                                                           OverlayTextArea(std::string,
                                                                                                           std::function<void(const std::string&)>,
                                                                                                           std::string,
                                                                                                           float)
                                                                                         >(),
                                                                                         sol::base_classes, sol::bases<OverlayElement>());
    overlayTextArea["text"]   = sol::property(&OverlayTextArea::getText, &OverlayTextArea::setText);
    overlayTextArea["append"] = &OverlayTextArea::append;
    overlayTextArea["clear"]  = &OverlayTextArea::clear;
  }

  {
    sol::usertype<OverlayTextbox> overlayTextbox = state.new_usertype<OverlayTextbox>("OverlayTextbox",
                                                                                      sol::constructors<
                                                                                        OverlayTextbox(std::string,
                                                                                                       std::function<void(const std::string&)>),
                                                                                        OverlayTextbox(std::string,
                                                                                                       std::function<void(const std::string&)>,
                                                                                                       std::string)
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

  state.new_enum<OverlayElementType>("OverlayElementType", {
    { "LABEL",         OverlayElementType::LABEL },
    { "COLORED_LABEL", OverlayElementType::COLORED_LABEL },
    { "BUTTON",        OverlayElementType::BUTTON },
    { "CHECKBOX",      OverlayElementType::CHECKBOX },
    { "SLIDER",        OverlayElementType::SLIDER },
    { "TEXTBOX",       OverlayElementType::TEXTBOX },
    { "TEXT_AREA",     OverlayElementType::TEXT_AREA },
    { "LIST_BOX",      OverlayElementType::LIST_BOX },
    { "DROPDOWN",      OverlayElementType::DROPDOWN },
    { "COLOR_PICKER",  OverlayElementType::COLOR_PICKER },
    { "TEXTURE",       OverlayElementType::TEXTURE },
    { "PROGRESS_BAR",  OverlayElementType::PROGRESS_BAR },
    { "PLOT",          OverlayElementType::PLOT },
    { "SEPARATOR",     OverlayElementType::SEPARATOR },
    { "FRAME_TIME",    OverlayElementType::FRAME_TIME },
    { "FPS_COUNTER",   OverlayElementType::FPS_COUNTER }
  });
}

} // namespace Raz
