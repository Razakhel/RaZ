#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

// Forward declaration of GLFWwindow, to allow its usage into functions
struct GLFWwindow;

namespace Raz {

class Overlay;
using OverlayPtr = std::unique_ptr<Overlay>;

enum class OverlayElementType {
  LABEL,
  BUTTON,
  CHECKBOX,
  SEPARATOR,
  FRAME_TIME,
  FPS_COUNTER
};

class Overlay {
public:
  explicit Overlay(GLFWwindow* window);

  template <typename... Args>
  static OverlayPtr create(Args&&... args) { return std::make_unique<Overlay>(std::forward<Args>(args)...); }

  void addLabel(std::string label);
  void addButton(std::string label, std::function<void()> action);
  void addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal);
  void addSeparator();
  void addFrameTime(std::string formattedLabel);
  void addFpsCounter(std::string formattedLabel);
  void render();

  ~Overlay();

private:
  struct OverlayElement;
  using OverlayElementPtr = std::unique_ptr<OverlayElement>;

  struct OverlayLabel;
  using OverlayLabelPtr = std::unique_ptr<OverlayLabel>;

  struct OverlayButton;
  using OverlayButtonPtr = std::unique_ptr<OverlayButton>;

  struct OverlayCheckbox;
  using OverlayCheckboxPtr = std::unique_ptr<OverlayCheckbox>;

  struct OverlaySeparator;
  using OverlaySeparatorPtr = std::unique_ptr<OverlaySeparator>;

  struct OverlayFrameTime;
  using OverlayFrameTimePtr = std::unique_ptr<OverlayFrameTime>;

  struct OverlayFpsCounter;
  using OverlayFpsCounterPtr = std::unique_ptr<OverlayFpsCounter>;

  struct OverlayElement {
    OverlayElement() = default;
    explicit OverlayElement(std::string label) : label{ std::move(label) } {}

    virtual OverlayElementType getType() const = 0;

    virtual ~OverlayElement() = default;

    std::string label {};
  };

  struct OverlayLabel : public OverlayElement {
    explicit OverlayLabel(std::string label) : OverlayElement(std::move(label)) {}

    OverlayElementType getType() const override { return OverlayElementType::LABEL; }

    template <typename... Args>
    static OverlayLabelPtr create(Args&&... args) { return std::make_unique<OverlayLabel>(std::forward<Args>(args)...); }
  };

  struct OverlayButton : public OverlayElement {
    OverlayButton(std::string label, std::function<void()> action) : OverlayElement(std::move(label)), action{ std::move(action) } {}

    OverlayElementType getType() const override { return OverlayElementType::BUTTON; }

    template <typename... Args>
    static OverlayButtonPtr create(Args&&... args) { return std::make_unique<OverlayButton>(std::forward<Args>(args)...); }

    std::function<void()> action {};
  };

  struct OverlayCheckbox : public OverlayElement {
    OverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal)
      : OverlayElement(std::move(label)), actionOn{ std::move(actionOn) }, actionOff{ std::move(actionOff) }, isChecked{ initVal } {}

    OverlayElementType getType() const override { return OverlayElementType::CHECKBOX; }

    template <typename... Args>
    static OverlayCheckboxPtr create(Args&&... args) { return std::make_unique<OverlayCheckbox>(std::forward<Args>(args)...); }

    std::function<void()> actionOn {};
    std::function<void()> actionOff {};
    bool isChecked {};
  };

  struct OverlaySeparator : public OverlayElement {
    OverlayElementType getType() const override { return OverlayElementType::SEPARATOR; }

    template <typename... Args>
    static OverlaySeparatorPtr create(Args&&... args) { return std::make_unique<OverlaySeparator>(std::forward<Args>(args)...); }
  };

  struct OverlayFrameTime : public OverlayElement {
    explicit OverlayFrameTime(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

    OverlayElementType getType() const override { return OverlayElementType::FRAME_TIME; }

    template <typename... Args>
    static OverlayFrameTimePtr create(Args&&... args) { return std::make_unique<OverlayFrameTime>(std::forward<Args>(args)...); }
  };

  struct OverlayFpsCounter : public OverlayElement {
    explicit OverlayFpsCounter(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

    OverlayElementType getType() const override { return OverlayElementType::FPS_COUNTER; }

    template <typename... Args>
    static OverlayFpsCounterPtr create(Args&&... args) { return std::make_unique<OverlayFpsCounter>(std::forward<Args>(args)...); }
  };

  std::vector<OverlayElementPtr> m_elements;
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
