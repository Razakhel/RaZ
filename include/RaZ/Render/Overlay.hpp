#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include "RaZ/Math/Vector.hpp"

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
  LIST_BOX,
  DROPDOWN,
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

public:
  Overlay() = default;
  Overlay(const Overlay&) = delete;
  Overlay(Overlay&&) noexcept = default;

  bool isEmpty() const noexcept { return m_windows.empty(); }

  /// Initializes ImGui with the containing window.
  /// \param windowHandle Handle to initialize the overlay with.
  void initialize(GLFWwindow* windowHandle) const;
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
  /// Destroys the overlay.
  void destroy() const;

  Overlay& operator=(const Overlay&) = delete;
  Overlay& operator=(Overlay&&) noexcept = default;

private:
  std::vector<std::unique_ptr<OverlayWindow>> m_windows {};
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
  std::string m_label {};
  bool m_enabled = true;
};

class OverlayLabel final : public OverlayElement {
  friend OverlayWindow;

public:
  explicit OverlayLabel(std::string label) : OverlayElement(std::move(label)) {}

  OverlayElementType getType() const override { return OverlayElementType::LABEL; }
  const std::string& getText() const noexcept { return m_label; }

  void setText(std::string text) { m_label = std::move(text); }
};

class OverlayColoredLabel final : public OverlayElement {
  friend OverlayWindow;

public:
  explicit OverlayColoredLabel(std::string label, Vec4f color) : OverlayElement(std::move(label)), m_color{ color } {}

  OverlayElementType getType() const override { return OverlayElementType::COLORED_LABEL; }
  const std::string& getText() const noexcept { return m_label; }
  const Vec4f& getColor() const noexcept { return m_color; }

  void setText(std::string text) { m_label = std::move(text); }
  void setText(std::string text, Vec4f color);

private:
  Vec4f m_color {};
};

class OverlayButton final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayButton(std::string label, std::function<void()> actionClick) : OverlayElement(std::move(label)), m_actionClick{ std::move(actionClick) } {}

  OverlayElementType getType() const override { return OverlayElementType::BUTTON; }

private:
  std::function<void()> m_actionClick {};
};

class OverlayCheckbox final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal)
    : OverlayElement(std::move(label)), m_actionOn{ std::move(actionOn) }, m_actionOff{ std::move(actionOff) }, m_isChecked{ initVal } {}

  OverlayElementType getType() const override { return OverlayElementType::CHECKBOX; }

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
  const std::string& getText() const noexcept { return m_text; }

  void setText(std::string text);

  OverlayTextbox& append(const std::string& text);
  void clear();

  OverlayTextbox& operator+=(const std::string& text) { return append(text); }

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

private:
  std::vector<std::string> m_entries {};
  std::function<void(const std::string&, std::size_t)> m_actionChanged {};
  std::size_t m_currentId {};
  const char* m_currentVal {};
};

class OverlayTexture final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayTexture(const Texture2D& texture, unsigned int maxWidth, unsigned int maxHeight) { setTexture(texture, maxWidth, maxHeight); }
  explicit OverlayTexture(const Texture2D& texture);

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
    : m_minVal{ minVal }, m_maxVal{ maxVal }, m_curVal{ minVal }, m_showValues{ showValues } {}

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

  std::string m_name {};
  OverlayPlotType m_type {};
  std::vector<float> m_values {};
};

class OverlayPlot final : public OverlayElement {
  friend OverlayWindow;

public:
  OverlayPlot(std::string label, std::size_t maxValCount, std::string xAxisLabel = {}, std::string yAxisLabel = {})
    : OverlayElement(std::move(label)), m_maxValCount{ maxValCount }, m_xAxisLabel{ std::move(xAxisLabel) }, m_yAxisLabel{ std::move(yAxisLabel) } {}

  OverlayElementType getType() const override { return OverlayElementType::PLOT; }

  OverlayPlotEntry& addEntry(std::string name, OverlayPlotType type = OverlayPlotType::LINE);

private:
  std::vector<std::unique_ptr<OverlayPlotEntry>> m_entries {};
  std::size_t m_maxValCount {};
  std::string m_xAxisLabel {};
  std::string m_yAxisLabel {};
};

class OverlaySeparator final : public OverlayElement {
public:
  OverlayElementType getType() const override { return OverlayElementType::SEPARATOR; }
};

class OverlayFrameTime final : public OverlayElement {
public:
  explicit OverlayFrameTime(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

  OverlayElementType getType() const override { return OverlayElementType::FRAME_TIME; }
};

class OverlayFpsCounter final : public OverlayElement {
public:
  explicit OverlayFpsCounter(std::string formattedLabel) : OverlayElement(std::move(formattedLabel)) {}

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
  /// \param color Color to display the text with.
  /// \return Reference to the newly added colored label.
  OverlayColoredLabel& addColoredLabel(std::string label, const Vec4f& color);
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
  /// Adds a texbox on the overlay window.
  /// \param label Text to be displayed beside the checkbox.
  /// \param callback Function to be called every time the content is modified.
  /// \return Reference to the newly added textbox.
  OverlayTextbox& addTextbox(std::string label, std::function<void(const std::string&)> callback);
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
  /// \param xAxisLabel Label to be displayed on the X axis.
  /// \param yAxisLabel Label to be displayed on the Y axis.
  /// \return Reference to the newly added plot.
  [[nodiscard]] OverlayPlot& addPlot(std::string label, std::size_t maxValCount, std::string xAxisLabel = {}, std::string yAxisLabel = {});
  /// Adds an horizontal separator on the overlay window.
  /// \return Reference to the newly added separator.
  OverlaySeparator& addSeparator();
  /// Adds a frame time display on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the frame time (%.Xf, X being the precision after the comma).
  /// \return Reference to the newly added frame time.
  OverlayFrameTime& addFrameTime(std::string formattedLabel);
  /// Adds a FPS (frames per second) counter on the overlay window.
  /// \param formattedLabel Text with a formatting placeholder to display the FPS (%.Xf, X being the precision after the comma).
  /// \return Reference to the newly added FPS counter.
  OverlayFpsCounter& addFpsCounter(std::string formattedLabel);
  /// Renders the window's elements.
  void render() const;

  OverlayWindow& operator=(const OverlayWindow&) = delete;
  OverlayWindow& operator=(OverlayWindow&&) noexcept = default;

private:
  std::string m_title {};
  Vec2f m_currentSize {};
  Vec2f m_currentPos {};
  bool m_enabled = true;
  std::vector<std::unique_ptr<OverlayElement>> m_elements {};
};

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
