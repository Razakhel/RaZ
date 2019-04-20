#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "GLFW/glfw3.h"

namespace Raz {

class Overlay;
using OverlayPtr = std::unique_ptr<Overlay>;

enum class OverlayElementType {
  TEXT,
  BUTTON,
  CHECKBOX,
  SEPARATOR,
  FRAME_TIME,
  FPS_COUNTER
};

using OverlayElements = std::vector<std::tuple<OverlayElementType, std::string, std::function<void()>, std::function<void()>>>;

class Overlay {
public:
  explicit Overlay(GLFWwindow* window);

  template <typename... Args>
  static OverlayPtr create(Args&&... args) { return std::make_unique<Overlay>(std::forward<Args>(args)...); }

  void addElement(OverlayElementType type, const std::string& text = "",
                  std::function<void()> actionOn = nullptr, std::function<void()> actionOff = nullptr);
  void addText(const std::string& text);
  void addButton(const std::string& text, std::function<void()> action);
  void addCheckbox(const std::string& text, bool initVal, std::function<void()> actionOn, std::function<void()> actionOff);
  void addSeparator();
  void addFrameTime(const std::string& formattedText);
  void addFpsCounter(const std::string& formattedText);
  void render();

  ~Overlay();

private:
  OverlayElements m_elements;
  std::unordered_map<std::size_t, bool> m_toggles;
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
