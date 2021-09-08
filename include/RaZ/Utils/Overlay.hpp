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

class OverlayWindow;
using OverlayWindowPtr = std::unique_ptr<OverlayWindow>;

enum class OverlayElementType {
  LABEL,
  COLORED_LABEL,
  BUTTON,
  CHECKBOX,
  SLIDER,
  TEXTBOX,
  LIST_BOX,
  DROPDOWN,
  TEXTURE,
  SEPARATOR,
  FRAME_TIME,
  FPS_COUNTER
};

/// Overlay class, used to render GUI elements (labels, buttons, checkboxes, ...) into a Window.
class Overlay {
  friend OverlayWindow;

public:
  Overlay() = default;
  Overlay(const Overlay&) = delete;
  Overlay(Overlay&&) noexcept = default;

  bool isEmpty() const noexcept { return m_windows.empty(); }

  template <typename... Args>
  static OverlayPtr create(Args&&... args) { return std::make_unique<Overlay>(std::forward<Args>(args)...); }

  /// Initializes ImGui with the containing window.
  /// \param windowHandle Handle to initialize the overlay with.
  void initialize(GLFWwindow* windowHandle) const;
  /// Adds a new overlay window.
  /// \param title Window title.
  /// \return The newly added window.
  [[nodiscard]] OverlayWindow& addWindow(std::string title);
  /// Checks if the overlay is currently requesting the keyboard inputs.
  /// \return True if the keyboard focus is taken, false otherwise.
  bool hasKeyboardFocus() const;
  /// Renders the overlay.
  void render() const;
  /// Destroys the overlay.
  void destroy() const;

  Overlay& operator=(const Overlay&) = delete;
  Overlay& operator=(Overlay&&) noexcept = default;

private:
  class OverlayElement;
  using OverlayElementPtr = std::unique_ptr<OverlayElement>;

  class OverlayLabel;
  using OverlayLabelPtr = std::unique_ptr<OverlayLabel>;

  class OverlayColoredLabel;
  using OverlayColoredLabelPtr = std::unique_ptr<OverlayColoredLabel>;

  class OverlayButton;
  using OverlayButtonPtr = std::unique_ptr<OverlayButton>;

  class OverlayCheckbox;
  using OverlayCheckboxPtr = std::unique_ptr<OverlayCheckbox>;

  class OverlaySlider;
  using OverlaySliderPtr = std::unique_ptr<OverlaySlider>;

  class OverlayTextbox;
  using OverlayTextboxPtr = std::unique_ptr<OverlayTextbox>;

  class OverlayListBox;
  using OverlayListBoxPtr = std::unique_ptr<OverlayListBox>;

  class OverlayDropdown;
  using OverlayDropdownPtr = std::unique_ptr<OverlayDropdown>;

  class OverlayTexture;
  using OverlayTexturePtr = std::unique_ptr<OverlayTexture>;

  class OverlaySeparator;
  using OverlaySeparatorPtr = std::unique_ptr<OverlaySeparator>;

  class OverlayFrameTime;
  using OverlayFrameTimePtr = std::unique_ptr<OverlayFrameTime>;

  class OverlayFpsCounter;
  using OverlayFpsCounterPtr = std::unique_ptr<OverlayFpsCounter>;

  class OverlayElement {
    friend OverlayWindow;

  public:
    OverlayElement() = default;
    explicit OverlayElement(std::string label) : m_label{ std::move(label) } {}

    virtual OverlayElementType getType() const = 0;

    virtual ~OverlayElement() = default;

  private:
    std::string m_label {};
  };

  class OverlayLabel final : public OverlayElement {
    friend OverlayWindow;

  public:
    explicit OverlayLabel(std::string label) : OverlayElement(std::move(label)) {}

    OverlayElementType getType() const override { return OverlayElementType::LABEL; }

    template <typename... Args>
    static OverlayLabelPtr create(Args&&... args) { return std::make_unique<OverlayLabel>(std::forward<Args>(args)...); }
  };

  class OverlayColoredLabel final : public OverlayElement {
    friend OverlayWindow;

  public:
    explicit OverlayColoredLabel(std::string label, Vec4f color) : OverlayElement(std::move(label)), m_color{ color } {}

    OverlayElementType getType() const override { return OverlayElementType::COLORED_LABEL; }

    template <typename... Args>
    static OverlayColoredLabelPtr create(Args&&... args) { return std::make_unique<OverlayColoredLabel>(std::forward<Args>(args)...); }

  private:
    Vec4f m_color {};
  };

  class OverlayButton final : public OverlayElement {
    friend OverlayWindow;

  public:
    OverlayButton(std::string label, std::function<void()> actionClick) : OverlayElement(std::move(label)), m_actionClick{ std::move(actionClick) } {}

    OverlayElementType getType() const override { return OverlayElementType::BUTTON; }

    template <typename... Args>
    static OverlayButtonPtr create(Args&&... args) { return std::make_unique<OverlayButton>(std::forward<Args>(args)...); }

  private:
    std::function<void()> m_actionClick {};
  };

  class OverlayCheckbox final : public OverlayElement {
    friend OverlayWindow;

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
    friend OverlayWindow;

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
    friend OverlayWindow;

  public:
    OverlayTextbox(std::string label, std::function<void(const std::string&)> callback) : OverlayElement(std::move(label)), m_callback{ std::move(callback) } {}

    OverlayElementType getType() const override { return OverlayElementType::TEXTBOX; }

    template <typename... Args>
    static OverlayTextboxPtr create(Args&&... args) { return std::make_unique<OverlayTextbox>(std::forward<Args>(args)...); }

  private:
    std::string m_text {};
    std::function<void(const std::string&)> m_callback {};
  };

  class OverlayListBox final : public OverlayElement {
    friend OverlayWindow;

  public:
    OverlayListBox(std::string label, std::vector<std::string> entries,
                   std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0)
      : OverlayElement(std::move(label)),
        m_entries{ std::move(entries) }, m_actionChanged{ std::move(actionChanged) }, m_currentId{ initId }, m_currentVal{ m_entries[initId].c_str() } {}

    OverlayElementType getType() const override { return OverlayElementType::LIST_BOX; }

    template <typename... Args>
    static OverlayListBoxPtr create(Args&&... args) { return std::make_unique<OverlayListBox>(std::forward<Args>(args)...); }

  private:
    std::vector<std::string> m_entries {};
    std::function<void(const std::string&, std::size_t)> m_actionChanged {};
    std::size_t m_currentId {};
    const char* m_currentVal {};
  };

  class OverlayDropdown final : public OverlayElement {
    friend OverlayWindow;

  public:
    OverlayDropdown(std::string label, std::vector<std::string> entries,
                    std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0)
      : OverlayElement(std::move(label)),
        m_entries{ std::move(entries) }, m_actionChanged{ std::move(actionChanged) }, m_currentId{ initId }, m_currentVal{ m_entries[initId].c_str() } {}

    OverlayElementType getType() const override { return OverlayElementType::DROPDOWN; }

    template <typename... Args>
    static OverlayDropdownPtr create(Args&&... args) { return std::make_unique<OverlayDropdown>(std::forward<Args>(args)...); }

  private:
    std::vector<std::string> m_entries {};
    std::function<void(const std::string&, std::size_t)> m_actionChanged {};
    std::size_t m_currentId {};
    const char* m_currentVal {};
  };

  class OverlayTexture final : public OverlayElement {
    friend OverlayWindow;

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

  std::vector<OverlayWindow> m_windows {};
};

/// OverlayWindow class, representing a specific window in the Overlay.
class OverlayWindow final {
public:
  explicit OverlayWindow(std::string title) : m_title{ std::move(title) } {}
  OverlayWindow(const OverlayWindow&) = delete;
  OverlayWindow(OverlayWindow&&) noexcept = default;

  /// Adds a label on the overlay window.
  /// \param label Text to be displayed.
  void addLabel(std::string label);
  /// Adds a colored label on the overlay window.
  /// \param label Text to be displayed.
  /// \param color Color to display the text with.
  void addColoredLabel(std::string label, Vec4f color);
  /// Adds a colored label on the overlay window.
  /// \param label Text to be displayed.
  /// \param red Text color's red component.
  /// \param green Text color's green component.
  /// \param blue Text color's blue component.
  /// \param alpha Text color's alpha component.
  void addColoredLabel(std::string label, float red, float green, float blue, float alpha = 1.f);
  /// Adds a button on the overlay window.
  /// \param label Text to be displayed beside the button.
  /// \param actionClick Action to be executed when clicked.
  void addButton(std::string label, std::function<void()> actionClick);
  /// Adds a checkbox on the overlay window.
  /// \param label Text to be displayed beside the checkbox.
  /// \param actionOn Action to be executed when toggled on.
  /// \param actionOff Action to be executed when toggled off.
  /// \param initVal Initial value, checked or not.
  void addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal);
  /// Adds a floating-point slider on the overlay window.
  /// \param label Text to be displayed beside the slider.
  /// \param actionSlide Action to be executed on a value change.
  /// \param minValue Lower value bound.
  /// \param maxValue Upper value bound.
  /// \param initValue Initial value.
  void addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue);
  /// Adds a texbox on the overlay window.
  /// \param label Text to be displayed beside the checkbox.
  /// \param callback Function to be called every time the content is modified.
  void addTextbox(std::string label, std::function<void(const std::string&)> callback);
  /// Adds a list box on the overlay window.
  /// \param label Text to be displayed beside the list.
  /// \param entries Texts to fill the list with.
  /// \param actionChanged Action to be executed when a different element is selected. Receives the currently selected text & index.
  /// \param initId Index of the element to select at initialization. Must be less than the entry count.
  void addListBox(std::string label, std::vector<std::string> entries,
                  std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0);
  /// Adds a dropdown list on the overlay window.
  /// \param label Text to be displayed beside the dropdown.
  /// \param entries Texts to fill the dropdown with.
  /// \param actionChanged Action to be executed when a different element is selected. Receives the currently selected text & index.
  /// \param initId Index of the element to select at initialization. Must be less than the entry count.
  void addDropdown(std::string label, std::vector<std::string> entries,
                   std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0);
  /// Adds a texture on the overlay window.
  /// \param texture Texture to be displayed.
  /// \param maxWidth Maximum texture's width.
  /// \param maxHeight Maximum texture's height.
  void addTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight);
  /// Adds a texture on the overlay window. The maximum width & height will be those of the texture.
  /// \param texture Texture to be displayed.
  void addTexture(const Texture& texture);
  /// Adds an horizontal separator on the overlay window.
  void addSeparator();
  /// Adds a frame time display on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the frame time (%.Xf, X being the precision after the comma).
  void addFrameTime(std::string formattedLabel);
  /// Adds a FPS (frames per second) counter on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the FPS (%.Xf, X being the precision after the comma).
  void addFpsCounter(std::string formattedLabel);
  /// Renders the window's elements.
  void render() const;

  OverlayWindow& operator=(const OverlayWindow&) = delete;
  OverlayWindow& operator=(OverlayWindow&&) noexcept = default;

private:
  std::string m_title {};
  std::vector<Overlay::OverlayElementPtr> m_elements {};
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
