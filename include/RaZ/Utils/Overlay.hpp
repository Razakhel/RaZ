#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include "RaZ/Render/Texture.hpp"

#include <functional>
#include <memory>
#include <string>
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
  SLIDER,
  TEXTBOX,
  TEXTURE,
  SEPARATOR,
  FRAME_TIME,
  FPS_COUNTER
};

class Overlay {
public:
  explicit Overlay(GLFWwindow* window);
  Overlay(const Overlay&) = delete;
  Overlay(Overlay&&) noexcept = default;

  template <typename... Args>
  static OverlayPtr create(Args&&... args) { return std::make_unique<Overlay>(std::forward<Args>(args)...); }

  void addLabel(std::string label);
  void addButton(std::string label, std::function<void()> actionClick);
  void addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal);
  void addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue);
  void addTextbox(std::string label, std::function<void(const std::string&)> callback);
#if !defined(RAZ_USE_VULKAN)
  void addTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight);
  void addTexture(const Texture& texture);
#endif
  void addSeparator();
  void addFrameTime(std::string formattedLabel);
  void addFpsCounter(std::string formattedLabel);
  bool hasKeyboardFocus() const;
  void render();

  Overlay& operator=(const Overlay&) = delete;
  Overlay& operator=(Overlay&&) noexcept = default;

  ~Overlay();

private:
  class OverlayElement;
  using OverlayElementPtr = std::unique_ptr<OverlayElement>;

  class OverlayLabel;
  using OverlayLabelPtr = std::unique_ptr<OverlayLabel>;

  class OverlayButton;
  using OverlayButtonPtr = std::unique_ptr<OverlayButton>;

  class OverlayCheckbox;
  using OverlayCheckboxPtr = std::unique_ptr<OverlayCheckbox>;

  class OverlaySlider;
  using OverlaySliderPtr = std::unique_ptr<OverlaySlider>;

  class OverlayTextbox;
  using OverlayTextboxPtr = std::unique_ptr<OverlayTextbox>;

  class OverlayTexture;
  using OverlayTexturePtr = std::unique_ptr<OverlayTexture>;

  class OverlaySeparator;
  using OverlaySeparatorPtr = std::unique_ptr<OverlaySeparator>;

  class OverlayFrameTime;
  using OverlayFrameTimePtr = std::unique_ptr<OverlayFrameTime>;

  class OverlayFpsCounter;
  using OverlayFpsCounterPtr = std::unique_ptr<OverlayFpsCounter>;

  class OverlayElement {
    friend Overlay;

  public:
    OverlayElement() = default;
    explicit OverlayElement(std::string label) : m_label{ std::move(label) } {}

    virtual OverlayElementType getType() const = 0;

    virtual ~OverlayElement() = default;

  private:
    std::string m_label {};
  };

  class OverlayLabel final : public OverlayElement {
    friend Overlay;

  public:
    explicit OverlayLabel(std::string label) : OverlayElement(std::move(label)) {}

    OverlayElementType getType() const override { return OverlayElementType::LABEL; }

    template <typename... Args>
    static OverlayLabelPtr create(Args&&... args) { return std::make_unique<OverlayLabel>(std::forward<Args>(args)...); }
  };

  class OverlayButton final : public OverlayElement {
    friend Overlay;

  public:
    OverlayButton(std::string label, std::function<void()> actionClick) : OverlayElement(std::move(label)), m_actionClick{ std::move(actionClick) } {}

    OverlayElementType getType() const override { return OverlayElementType::BUTTON; }

    template <typename... Args>
    static OverlayButtonPtr create(Args&&... args) { return std::make_unique<OverlayButton>(std::forward<Args>(args)...); }

  private:
    std::function<void()> m_actionClick {};
  };

  class OverlayCheckbox final : public OverlayElement {
    friend Overlay;

  public:
    OverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal)
      : OverlayElement(std::move(label)), m_actionOn{ std::move(actionOn) }, m_actionOff{ std::move(actionOff) }, m_isChecked{ initVal } {}

    OverlayElementType getType() const override { return OverlayElementType::CHECKBOX; }

    template <typename... Args>
    static OverlayCheckboxPtr create(Args&&... args) { return std::make_unique<OverlayCheckbox>(std::forward<Args>(args)...); }

  private:
    std::function<void()> m_actionOn {};
    std::function<void()> m_actionOff {};
    bool m_isChecked {};
  };

  class OverlaySlider final : public OverlayElement {
    friend Overlay;

  public:
    OverlaySlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue)
      : OverlayElement(std::move(label)), m_actionSlide{ std::move(actionSlide) }, m_minValue{ minValue }, m_maxValue{ maxValue }, m_currentValue{ initValue } {}

    OverlayElementType getType() const override { return OverlayElementType::SLIDER; }

    template <typename... Args>
    static OverlaySliderPtr create(Args&&... args) { return std::make_unique<OverlaySlider>(std::forward<Args>(args)...); }

  private:
    std::function<void(float)> m_actionSlide {};
    float m_minValue {};
    float m_maxValue {};
    float m_currentValue {};
  };

  class OverlayTextbox final : public OverlayElement {
    friend Overlay;

  public:
    OverlayTextbox(std::string label, std::function<void(const std::string&)> callback) : OverlayElement(std::move(label)), m_callback{ std::move(callback) } {}

    OverlayElementType getType() const override { return OverlayElementType::TEXTBOX; }

    template <typename... Args>
    static OverlayTextboxPtr create(Args&&... args) { return std::make_unique<OverlayTextbox>(std::forward<Args>(args)...); }

  private:
    std::string m_text {};
    std::function<void(const std::string&)> m_callback {};
  };

#if !defined(RAZ_USE_VULKAN)
  class OverlayTexture final : public OverlayElement {
    friend Overlay;

  public:
    OverlayTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight)
      : OverlayElement(), m_index{ texture.getIndex() }, m_width{ static_cast<float>(maxWidth) }, m_height{ static_cast<float>(maxHeight) } {}
    explicit OverlayTexture(const Texture& texture) : OverlayTexture(texture, texture.getImage().getWidth(), texture.getImage().getHeight()) {}

    OverlayElementType getType() const override { return OverlayElementType::TEXTURE; }

    template <typename... Args>
    static OverlayTexturePtr create(Args&&... args) { return std::make_unique<OverlayTexture>(std::forward<Args>(args)...); }

  private:
    unsigned int m_index {};
    float m_width {};
    float m_height {};
  };
#endif

  class OverlaySeparator final : public OverlayElement {
  public:
    OverlayElementType getType() const override { return OverlayElementType::SEPARATOR; }

    template <typename... Args>
    static OverlaySeparatorPtr create(Args&&... args) { return std::make_unique<OverlaySeparator>(std::forward<Args>(args)...); }
  };

  class OverlayFrameTime final : public OverlayElement {
  public:
    explicit OverlayFrameTime(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

    OverlayElementType getType() const override { return OverlayElementType::FRAME_TIME; }

    template <typename... Args>
    static OverlayFrameTimePtr create(Args&&... args) { return std::make_unique<OverlayFrameTime>(std::forward<Args>(args)...); }
  };

  class OverlayFpsCounter final : public OverlayElement {
  public:
    explicit OverlayFpsCounter(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

    OverlayElementType getType() const override { return OverlayElementType::FPS_COUNTER; }

    template <typename... Args>
    static OverlayFpsCounterPtr create(Args&&... args) { return std::make_unique<OverlayFpsCounter>(std::forward<Args>(args)...); }
  };

  std::vector<OverlayElementPtr> m_elements;
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
