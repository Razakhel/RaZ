#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include "RaZ/Data/Color.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Raz {

class OverlayWindow;
class Texture2D;

enum class OverlayElementType {
  LABEL,
  COLORED_LABEL,
  BUTTON,
  CHECKBOX,
  SLIDER,
  TEXTBOX,
  TEXT_AREA,
  LIST_BOX,
  DROPDOWN,
  COLOR_PICKER,
  TEXTURE,
  PROGRESS_BAR,
  PLOT,
  SEPARATOR,
  FRAME_TIME,
  FPS_COUNTER
};

enum class OverlayPlotType {
  LINE,
  SHADED
};

/// Overlay class, used to render GUI elements (labels, buttons, checkboxes, ...) into a Window.
class Overlay {
  friend OverlayWindow;
  friend class Window;

public:
  Overlay() = default;
  Overlay(const Overlay&) = delete;
  Overlay(Overlay&&) noexcept = default;

  bool isEmpty() const noexcept { return m_windows.empty(); }

  /// Adds a new overlay window.
  /// \param title Window title.
  /// \param initSize Initial window size. If both X & Y are strictly lower than 0, automatically resizes the window from its content.
  /// \param initPos Initial window position.
  /// \return The newly added window.
  [[nodiscard]] OverlayWindow& addWindow(std::string title, const Vec2f& initSize = Vec2f(0.f), const Vec2f& initPos = Vec2f(0.f));
  /// Checks if the overlay is currently requesting the keyboard inputs.
  /// \return True if the keyboard focus is taken, false otherwise.
  bool hasKeyboardFocus() const;
  /// Checks if the overlay is currently requesting the mouse inputs.
  /// \return True if the mouse focus is taken, false otherwise.
  bool hasMouseFocus() const;
  /// Renders the overlay.
  void render() const;

  Overlay& operator=(const Overlay&) = delete;
  Overlay& operator=(Overlay&&) noexcept = default;

private:
  /// Initializes ImGui with the containing window.
  /// \param windowHandle Handle to initialize the overlay with.
  static void initialize(GLFWwindow* windowHandle);
  /// Destroys the overlay.
  static void destroy();

  std::vector<std::unique_ptr<OverlayWindow>> m_windows;
};

class OverlayElement {
  friend OverlayWindow;

public:
  OverlayElement() = default;
  explicit OverlayElement(std::string label) : m_label{ std::move(label) } {}
  OverlayElement(const OverlayElement&) = delete;
  OverlayElement(OverlayElement&&) noexcept = delete;

  virtual OverlayElementType getType() const = 0;
  bool isEnabled() const noexcept { return m_enabled; }

  void enable(bool enabled = true) { m_enabled = enabled; }
  void disable() { enable(false); }

  OverlayElement& operator=(const OverlayElement&) = delete;
  OverlayElement& operator=(OverlayElement&&) noexcept = delete;

  virtual ~OverlayElement() = default;

protected:
  std::string m_label;
  bool m_enabled = true;
};

class OverlayLabel final : public OverlayElement {
  friend OverlayWindow;

public:
  explicit OverlayLabel(std::string label) noexcept : OverlayElement(std::move(label)) {}

  OverlayElementType getType() const override { return OverlayElementType::LABEL; }
  const std::string& getText() const noexcept { return m_label; }

  void setText(std::string text) { m_label = std::move(text); }
};

class OverlayColoredLabel final : public OverlayElement {
  friend OverlayWindow;

public:
  explicit OverlayColoredLabel(std::string label, const Color& color, float alpha = 1.f) noexcept
    : OverlayElement(std::move(label)), m_color{ color }, m_alpha{ alpha } {}

  OverlayElementType getType() const override { return OverlayElementType::COLORED_LABEL; }
  const std::string& getText() const noexcept { return m_label; }
  const Color& getColor() const noexcept { return m_color; }
  float getAlpha() const noexcept { return m_alpha; }

  void setText(std::string text) { m_label = std::move(text); }
  void setColor(const Color& color, float alpha = 1.f);
  void setAlpha(float alpha) { m_alpha = alpha; }

private:
  Color m_color;
  float m_alpha = 1.f;
};

class OverlayButton final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayButton(std::string label, std::function<void()> actionClick) : OverlayElement(std::move(label)), m_actionClick{ std::move(actionClick) } {
    if (m_actionClick == nullptr)
      throw std::invalid_argument("[OverlayButton] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::BUTTON; }

private:
  std::function<void()> m_actionClick;
};

class OverlayCheckbox final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal)
  : OverlayElement(std::move(label)), m_actionOn{ std::move(actionOn) }, m_actionOff{ std::move(actionOff) }, m_isChecked{ initVal } {
    if (m_actionOn == nullptr || m_actionOff == nullptr)
      throw std::invalid_argument("[OverlayCheckbox] Both callback functions must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::CHECKBOX; }

private:
  std::function<void()> m_actionOn;
  std::function<void()> m_actionOff;
  bool m_isChecked {};
};

class OverlaySlider final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlaySlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue)
    : OverlayElement(std::move(label)), m_actionSlide{ std::move(actionSlide) }, m_minValue{ minValue }, m_maxValue{ maxValue }, m_currentValue{ initValue } {
    if (m_actionSlide == nullptr)
      throw std::invalid_argument("[OverlaySlider] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::SLIDER; }

private:
  std::function<void(float)> m_actionSlide;
  float m_minValue {};
  float m_maxValue {};
  float m_currentValue {};
};

class OverlayTextbox final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayTextbox(std::string label, std::function<void(const std::string&)> callback, std::string initText = {})
    : OverlayElement(std::move(label)), m_text{ std::move(initText) }, m_callback{ std::move(callback) } {
    if (m_callback == nullptr)
      throw std::invalid_argument("[OverlayTextbox] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::TEXTBOX; }
  const std::string& getText() const noexcept { return m_text; }

  void setText(std::string text);

  OverlayTextbox& append(const std::string& text);
  void clear();

  OverlayTextbox& operator+=(const std::string& text) { return append(text); }

private:
  std::string m_text;
  std::function<void(const std::string&)> m_callback;
};

class OverlayTextArea final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayTextArea(std::string label, std::function<void(const std::string&)> callback, std::string initText = {}, float maxHeight = -1.f)
    : OverlayElement(std::move(label)), m_text{ std::move(initText) }, m_callback{ std::move(callback) }, m_maxHeight{ maxHeight } {
    if (m_callback == nullptr)
      throw std::invalid_argument("[OverlayTextArea] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::TEXT_AREA; }
  const std::string& getText() const noexcept { return m_text; }

  void setText(std::string text);

  OverlayTextArea& append(const std::string& text);
  void clear();

  OverlayTextArea& operator+=(const std::string& text) { return append(text); }

private:
  std::string m_text;
  std::function<void(const std::string&)> m_callback;
  float m_maxHeight {};
};

class OverlayListBox final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayListBox(std::string label, std::vector<std::string> entries,
                 std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0)
    : OverlayElement(std::move(label)), m_entries{ std::move(entries) }, m_actionChanged{ std::move(actionChanged) }, m_currentId{ initId } {
    if (m_actionChanged == nullptr)
      throw std::invalid_argument("[OverlayListBox] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::LIST_BOX; }

private:
  std::vector<std::string> m_entries;
  std::function<void(const std::string&, std::size_t)> m_actionChanged;
  std::size_t m_currentId {};
};

class OverlayDropdown final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayDropdown(std::string label, std::vector<std::string> entries,
                  std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0)
    : OverlayElement(std::move(label)), m_entries{ std::move(entries) }, m_actionChanged{ std::move(actionChanged) }, m_currentId{ initId } {
    if (m_actionChanged == nullptr)
      throw std::invalid_argument("[OverlayDropdown] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::DROPDOWN; }

private:
  std::vector<std::string> m_entries;
  std::function<void(const std::string&, std::size_t)> m_actionChanged;
  std::size_t m_currentId {};
};

class OverlayColorPicker final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayColorPicker(std::string label, std::function<void(const Color&)> actionChanged, const Color& initColor)
  : OverlayElement(std::move(label)), m_actionChanged{ std::move(actionChanged) }, m_currentColor{ initColor.red(), initColor.green(), initColor.blue() } {
    if (m_actionChanged == nullptr)
      throw std::invalid_argument("[OverlayColorPicker] The callback function must be valid");
  }

  OverlayElementType getType() const override { return OverlayElementType::COLOR_PICKER; }

private:
  std::function<void(const Color&)> m_actionChanged;
  std::array<float, 3> m_currentColor {};
};

class OverlayTexture final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayTexture(const Texture2D& texture, unsigned int maxWidth, unsigned int maxHeight) noexcept { setTexture(texture, maxWidth, maxHeight); }
  explicit OverlayTexture(const Texture2D& texture) noexcept;

  OverlayElementType getType() const override { return OverlayElementType::TEXTURE; }

  void setTexture(const Texture2D& texture, unsigned int maxWidth, unsigned int maxHeight) noexcept;
  void setTexture(const Texture2D& texture) noexcept;

private:
  unsigned int m_index {};
  float m_width {};
  float m_height {};
};

class OverlayProgressBar final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayProgressBar(int minVal, int maxVal, bool showValues = false)
    : m_minVal{ minVal }, m_maxVal{ maxVal }, m_curVal{ minVal }, m_showValues{ showValues } {
    if (m_minVal >= m_maxVal)
      throw std::invalid_argument("[OverlayProgressBar] The maximum value must be greater than the minimum one");
  }

  OverlayElementType getType() const override { return OverlayElementType::PROGRESS_BAR; }
  int getCurrentValue() const noexcept { return m_curVal; }
  bool hasStarted() const noexcept { return m_curVal > m_minVal; }
  bool hasFinished() const noexcept { return m_curVal >= m_maxVal; }

  void setCurrentValue(int curVal) noexcept { m_curVal = curVal; }

  int operator++() noexcept { return ++m_curVal; }
  int operator++(int) noexcept { return m_curVal++; }
  int& operator+=(int val) noexcept { return m_curVal += val; }
  int operator--() noexcept { return --m_curVal; }
  int operator--(int) noexcept { return m_curVal--; }
  int& operator-=(int val) noexcept { return m_curVal -= val; }

private:
  int m_minVal {};
  int m_maxVal {};
  int m_curVal {};
  bool m_showValues {};
};

class OverlayPlotEntry {
  friend class OverlayPlot;
  friend OverlayWindow;

public:
  void push(float value) {
    m_values.erase(m_values.begin());
    m_values.emplace_back(value);
  }

private:
  OverlayPlotEntry(std::string name, OverlayPlotType type, std::size_t maxValCount)
    : m_name{ std::move(name) }, m_type{ type }, m_values(maxValCount) {}

  std::string m_name;
  OverlayPlotType m_type {};
  std::vector<float> m_values;
};

class OverlayPlot final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayPlot(std::string label, std::size_t maxValCount,
              std::string xAxisLabel = {}, std::string yAxisLabel = {},
              float minYVal = 0.f, float maxYVal = 100.f, bool lockYAxis = false,
              float maxHeight = -1.f)
    : OverlayElement(std::move(label)),
      m_maxValCount{ maxValCount },
      m_xAxisLabel{ std::move(xAxisLabel) },
      m_yAxisLabel{ std::move(yAxisLabel) },
      m_minYVal{ minYVal },
      m_maxYVal{ maxYVal },
      m_lockY{ lockYAxis },
      m_maxHeight{ maxHeight } {
    if (m_maxValCount < 1)
      throw std::invalid_argument("[OverlayPlot] The maximum value count must be strictly greater than 0");
    if (m_minYVal >= m_maxYVal)
      throw std::invalid_argument("[OverlayPlot] The maximum Y value must be greater than the minimum one");
  }

  OverlayElementType getType() const override { return OverlayElementType::PLOT; }

  OverlayPlotEntry& addEntry(std::string name, OverlayPlotType type = OverlayPlotType::LINE);

private:
  std::vector<std::unique_ptr<OverlayPlotEntry>> m_entries;
  std::size_t m_maxValCount {};
  std::string m_xAxisLabel;
  std::string m_yAxisLabel;
  float m_minYVal {};
  float m_maxYVal {};
  bool m_lockY {};
  float m_maxHeight {};
};

class OverlaySeparator final : public OverlayElement {
public:
  OverlayElementType getType() const override { return OverlayElementType::SEPARATOR; }
};

class OverlayFrameTime final : public OverlayElement {
public:
  explicit OverlayFrameTime(std::string formattedLabel) noexcept : OverlayElement(std::move(formattedLabel)) {}

  OverlayElementType getType() const override { return OverlayElementType::FRAME_TIME; }
};

class OverlayFpsCounter final : public OverlayElement {
public:
  explicit OverlayFpsCounter(std::string formattedLabel) noexcept : OverlayElement(std::move(formattedLabel)) {}

  OverlayElementType getType() const override { return OverlayElementType::FPS_COUNTER; }
};

/// OverlayWindow class, representing a specific window in the Overlay.
class OverlayWindow {
public:
  /// Creates an overlay window.
  /// \param title Window title.
  /// \param initSize Initial window size. If both X & Y are strictly lower than 0, automatically resizes the window from its content.
  /// \param initPos Initial window position.
  explicit OverlayWindow(std::string title, const Vec2f& initSize = Vec2f(0.f), const Vec2f& initPos = Vec2f(0.f)) noexcept;
  OverlayWindow(const OverlayWindow&) = delete;
  OverlayWindow(OverlayWindow&&) noexcept = default;

  bool isEnabled() const noexcept { return m_enabled; }

  /// Changes the window's visibility state.
  /// \param enabled True if the window should be shown, false otherwise.
  void enable(bool enabled = true) noexcept { m_enabled = enabled; }
  /// Hides the window.
  void disable() noexcept { enable(false); }
  /// Adds a label on the overlay window.
  /// \param label Text to be displayed.
  /// \return Reference to the newly added label.
  OverlayLabel& addLabel(std::string label);
  /// Adds a colored label on the overlay window.
  /// \param label Text to be displayed.
  /// \param color Color to display the text in.
  /// \param alpha Transparency to apply to the text (between 0 for fully transparent and 1 for fully opaque).
  /// \return Reference to the newly added colored label.
  OverlayColoredLabel& addColoredLabel(std::string label, const Color& color, float alpha = 1.f);
  /// Adds a colored label on the overlay window.
  /// \param label Text to be displayed.
  /// \param red Text color's red component.
  /// \param green Text color's green component.
  /// \param blue Text color's blue component.
  /// \param alpha Text color's alpha component.
  /// \return Reference to the newly added colored label.
  OverlayColoredLabel& addColoredLabel(std::string label, float red, float green, float blue, float alpha = 1.f);
  /// Adds a button on the overlay window.
  /// \param label Text to be displayed beside the button.
  /// \param actionClick Action to be executed when clicked.
  /// \return Reference to the newly added button.
  OverlayButton& addButton(std::string label, std::function<void()> actionClick);
  /// Adds a checkbox on the overlay window.
  /// \param label Text to be displayed beside the checkbox.
  /// \param actionOn Action to be executed when toggled on.
  /// \param actionOff Action to be executed when toggled off.
  /// \param initVal Initial value, checked or not.
  /// \return Reference to the newly added checkbox.
  OverlayCheckbox& addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal);
  /// Adds a floating-point slider on the overlay window.
  /// \param label Text to be displayed beside the slider.
  /// \param actionSlide Action to be executed on a value change.
  /// \param minValue Lower value bound.
  /// \param maxValue Upper value bound.
  /// \param initValue Initial value.
  /// \return Reference to the newly added slider.
  OverlaySlider& addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue);
  /// Adds a textbox on the overlay window.
  /// \param label Text to be displayed beside the textbox.
  /// \param callback Function to be called every time the content is modified.
  /// \param initText Initial text to be set.
  /// \return Reference to the newly added textbox.
  OverlayTextbox& addTextbox(std::string label, std::function<void(const std::string&)> callback, std::string initText = {});
  /// Adds a text area on the overlay window.
  /// \param label Text to be displayed beside the text area.
  /// \param callback Function to be called every time the content is modified.
  /// \param initText Initial text to be set.
  /// \param maxHeight Maximum height. If strictly lower than 0, automatically resizes the widget to fit the window's content.
  /// \return Reference to the newly added text area.
  OverlayTextArea& addTextArea(std::string label, std::function<void(const std::string&)> callback, std::string initText = {}, float maxHeight = -1.f);
  /// Adds a list box on the overlay window.
  /// \param label Text to be displayed beside the list.
  /// \param entries Texts to fill the list with.
  /// \param actionChanged Action to be executed when a different element is selected. Receives the currently selected text & index.
  /// \param initId Index of the element to select at initialization. Must be less than the entry count.
  /// \return Reference to the newly added listbox.
  OverlayListBox& addListBox(std::string label, std::vector<std::string> entries,
                             std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0);
  /// Adds a dropdown list on the overlay window.
  /// \param label Text to be displayed beside the dropdown.
  /// \param entries Texts to fill the dropdown with.
  /// \param actionChanged Action to be executed when a different element is selected. Receives the currently selected text & index.
  /// \param initId Index of the element to select at initialization. Must be less than the entry count.
  /// \return Reference to the newly added dropdown list.
  OverlayDropdown& addDropdown(std::string label, std::vector<std::string> entries,
                               std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId = 0);
  /// Adds a color picker on the overlay window.
  /// \param label Text to be displayed beside the color picker.
  /// \param actionChanged Action to be executed when a color is selected.
  /// \param initColor Initial color.
  /// \return Reference to the newly added color picker.
  OverlayColorPicker& addColorPicker(std::string label, std::function<void(const Color&)> actionChanged, const Color& initColor);
  /// Adds a texture on the overlay window.
  /// \param texture Texture to be displayed.
  /// \param maxWidth Maximum texture's width.
  /// \param maxHeight Maximum texture's height.
  /// \return Reference to the newly added texture.
  OverlayTexture& addTexture(const Texture2D& texture, unsigned int maxWidth, unsigned int maxHeight);
  /// Adds a texture on the overlay window. The maximum width & height will be those of the texture.
  /// \param texture Texture to be displayed.
  /// \return Reference to the newly added texture.
  OverlayTexture& addTexture(const Texture2D& texture);
  /// Adds a progress bar on the overlay window.
  /// \param minVal Minimum value.
  /// \param maxVal Maximum value.
  /// \param showValues Show values ("<current>/<maximum>") instead of percentage.
  /// \return Reference to the newly added progress bar.
  [[nodiscard]] OverlayProgressBar& addProgressBar(int minVal, int maxVal, bool showValues = false);
  /// Adds a plot on the overlay window.
  /// \param label Text to be displayed beside the plot.
  /// \param maxValCount Maximum number of values to plot.
  /// \param xAxisLabel Label to be displayed on the X (horizontal) axis.
  /// \param yAxisLabel Label to be displayed on the Y (vertical) axis.
  /// \param minYVal Minimum value on the Y (vertical) axis.
  /// \param maxYVal Maximum value on the Y (vertical) axis.
  /// \param lockYAxis Whether to allow panning & zooming on the Y (vertical) axis.
  /// \param maxHeight Maximum height. If strictly lower than 0, automatically resizes the widget to fit the window's content.
  /// \return Reference to the newly added plot.
  [[nodiscard]] OverlayPlot& addPlot(std::string label, std::size_t maxValCount,
                                     std::string xAxisLabel = {}, std::string yAxisLabel = {},
                                     float minYVal = 0.f, float maxYVal = 100.f, bool lockYAxis = false,
                                     float maxHeight = -1.f);
  /// Adds a horizontal separator on the overlay window.
  /// \return Reference to the newly added separator.
  OverlaySeparator& addSeparator();
  /// Adds a frame time display on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the frame time (%.Xf, X being the precision after the comma).
  /// \return Reference to the newly added frame time.
  OverlayFrameTime& addFrameTime(std::string formattedLabel);
  /// Adds an FPS (frames per second) counter on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the FPS (%.Xf, X being the precision after the comma).
  /// \return Reference to the newly added FPS counter.
  OverlayFpsCounter& addFpsCounter(std::string formattedLabel);
  /// Renders the window's elements.
  void render() const;

  OverlayWindow& operator=(const OverlayWindow&) = delete;
  OverlayWindow& operator=(OverlayWindow&&) noexcept = default;

private:
  std::string m_title;
  Vec2f m_currentSize;
  Vec2f m_currentPos;
  bool m_enabled = true;
  std::vector<std::unique_ptr<OverlayElement>> m_elements;
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
