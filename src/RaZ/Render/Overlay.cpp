#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "RaZ/Render/Overlay.hpp"
#include "RaZ/Render/Renderer.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Utils/Logger.hpp"

namespace Raz {

void Overlay::initialize(GLFWwindow* windowHandle) const {
  if (ImGui::GetCurrentContext() != nullptr)
    return; // The overlay has already been initialized

  Logger::debug("[Overlay] Initializing...");

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(windowHandle, false);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ImGui_ImplOpenGL3_Init("#version 330 core");
#else
  ImGui_ImplOpenGL3_Init("#version 300 es");
#endif

  Logger::debug("[Overlay] Initialized");
}

OverlayWindow& Overlay::addWindow(std::string title, const Vec2f& initSize, const Vec2f& initPos) {
  return *m_windows.emplace_back(std::make_unique<OverlayWindow>(std::move(title), initSize, initPos));
}

bool Overlay::hasKeyboardFocus() const {
  return ImGui::GetIO().WantCaptureKeyboard;
}

bool Overlay::hasMouseFocus() const {
  return ImGui::GetIO().WantCaptureMouse;
}

void Overlay::render() const {
#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::pushDebugGroup("Overlay pass");
#endif

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  for (const std::unique_ptr<OverlayWindow>& window : m_windows)
    window->render();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#if !defined(USE_OPENGL_ES) && defined(RAZ_CONFIG_DEBUG)
  if (Renderer::checkVersion(4, 3))
    Renderer::popDebugGroup();
#endif
}

void Overlay::destroy() const {
  if (ImGui::GetCurrentContext() == nullptr)
    return; // The overlay has already been destroyed

  Logger::debug("[Overlay] Destroying...");

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

  Logger::debug("[Overlay] Destroyed");
}

void OverlayColoredLabel::setText(std::string text, Vec4f color) {
  m_label = std::move(text);
  m_color = color;
}

void OverlayTextbox::setText(std::string text) {
  m_text = std::move(text);
  m_callback(m_text);
}

OverlayTextbox& OverlayTextbox::append(const std::string& text) {
  m_text += text;
  m_callback(m_text);

  return *this;
}

void OverlayTextbox::clear() {
  m_text.clear();
  m_callback(m_text);
}

OverlayTexture::OverlayTexture(const Texture& texture)
  : OverlayTexture(texture, texture.getImage().getWidth(), texture.getImage().getHeight()) {}

void OverlayTexture::setTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight) noexcept {
  m_index  = texture.getIndex();
  m_width  = static_cast<float>(maxWidth);
  m_height = static_cast<float>(maxHeight);
}

void OverlayTexture::setTexture(const Texture& texture) noexcept {
  setTexture(texture, texture.getImage().getWidth(), texture.getImage().getHeight());
}

OverlayWindow::OverlayWindow(std::string title, const Vec2f& initSize, const Vec2f& initPos) noexcept
  : m_title{ std::move(title) }, m_currentSize{ initSize }, m_currentPos{ initPos } {}

OverlayLabel& OverlayWindow::addLabel(std::string label) {
  return static_cast<OverlayLabel&>(*m_elements.emplace_back(std::make_unique<OverlayLabel>(std::move(label))));
}

OverlayColoredLabel& OverlayWindow::addColoredLabel(std::string label, const Vec4f& color) {
  return static_cast<OverlayColoredLabel&>(*m_elements.emplace_back(std::make_unique<OverlayColoredLabel>(std::move(label), color)));
}

OverlayColoredLabel& OverlayWindow::addColoredLabel(std::string label, float red, float green, float blue, float alpha) {
  return addColoredLabel(std::move(label), Vec4f(red, green, blue, alpha));
}

OverlayButton& OverlayWindow::addButton(std::string label, std::function<void()> actionClick) {
  return static_cast<OverlayButton&>(*m_elements.emplace_back(std::make_unique<OverlayButton>(std::move(label), std::move(actionClick))));
}

OverlayCheckbox& OverlayWindow::addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  return static_cast<OverlayCheckbox&>(*m_elements.emplace_back(std::make_unique<OverlayCheckbox>(std::move(label), std::move(actionOn), std::move(actionOff),
                                                                                                  initVal)));
}

OverlaySlider& OverlayWindow::addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue) {
  return static_cast<OverlaySlider&>(*m_elements.emplace_back(std::make_unique<OverlaySlider>(std::move(label), std::move(actionSlide),
                                                                                              minValue, maxValue, initValue)));
}

OverlayTextbox& OverlayWindow::addTextbox(std::string label, std::function<void(const std::string&)> callback) {
  auto& textbox = static_cast<OverlayTextbox&>(*m_elements.emplace_back(std::make_unique<OverlayTextbox>(std::move(label), std::move(callback))));
  textbox.m_text.reserve(64);
  return textbox;
}

OverlayListBox& OverlayWindow::addListBox(std::string label, std::vector<std::string> entries,
                                          std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId) {
  if (entries.empty())
    throw std::invalid_argument("[Overlay] Cannot create a list box with no entry");

  assert("Error: A list box's initial index cannot reference a non-existing entry." && initId < entries.size());
  return static_cast<OverlayListBox&>(*m_elements.emplace_back(std::make_unique<OverlayListBox>(std::move(label), std::move(entries),
                                                                                                std::move(actionChanged), initId)));
}

OverlayDropdown& OverlayWindow::addDropdown(std::string label, std::vector<std::string> entries,
                                            std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId) {
  if (entries.empty())
    throw std::invalid_argument("[Overlay] Cannot create a dropdown list with no entry");

  assert("Error: A dropdown's initial index cannot reference a non-existing entry." && initId < entries.size());
  return static_cast<OverlayDropdown&>(*m_elements.emplace_back(std::make_unique<OverlayDropdown>(std::move(label), std::move(entries),
                                                                                                  std::move(actionChanged), initId)));
}

OverlayTexture& OverlayWindow::addTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight) {
  return static_cast<OverlayTexture&>(*m_elements.emplace_back(std::make_unique<OverlayTexture>(texture, maxWidth, maxHeight)));
}

OverlayTexture& OverlayWindow::addTexture(const Texture& texture) {
  return static_cast<OverlayTexture&>(*m_elements.emplace_back(std::make_unique<OverlayTexture>(texture)));
}

OverlayProgressBar& OverlayWindow::addProgressBar(int minVal, int maxVal, bool showValues) {
  return static_cast<OverlayProgressBar&>(*m_elements.emplace_back(std::make_unique<OverlayProgressBar>(minVal, maxVal, showValues)));
}

OverlaySeparator& OverlayWindow::addSeparator() {
  return static_cast<OverlaySeparator&>(*m_elements.emplace_back(std::make_unique<OverlaySeparator>()));
}

OverlayFrameTime& OverlayWindow::addFrameTime(std::string formattedLabel) {
  return static_cast<OverlayFrameTime&>(*m_elements.emplace_back(std::make_unique<OverlayFrameTime>(std::move(formattedLabel))));
}

OverlayFpsCounter& OverlayWindow::addFpsCounter(std::string formattedLabel) {
  return static_cast<OverlayFpsCounter&>(*m_elements.emplace_back(std::make_unique<OverlayFpsCounter>(std::move(formattedLabel))));
}

void OverlayWindow::render() const {
  if (!m_enabled)
    return;

  ImGui::SetNextWindowSize(ImVec2(m_currentSize.x(), m_currentSize.y()), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(m_currentPos.x(), m_currentPos.y()), ImGuiCond_Once);
  ImGui::Begin(m_title.c_str(), nullptr, (m_currentSize.x() < 0.f && m_currentSize.y() < 0.f ? ImGuiWindowFlags_AlwaysAutoResize : ImGuiWindowFlags_None));

  for (const auto& element : m_elements) {
    if (!element->isEnabled())
      continue;

    switch (element->getType()) {
      case OverlayElementType::LABEL:
        ImGui::PushTextWrapPos();
        ImGui::TextUnformatted(element->m_label.c_str());
        ImGui::PopTextWrapPos();
        break;

      case OverlayElementType::COLORED_LABEL:
      {
        const auto& label = static_cast<OverlayColoredLabel&>(*element);
        ImGui::PushTextWrapPos();
        ImGui::TextColored(ImVec4(label.m_color.x(), label.m_color.y(), label.m_color.z(), label.m_color.w()), "%s", element->m_label.c_str());
        ImGui::PopTextWrapPos();
        break;
      }

      case OverlayElementType::BUTTON:
      {
        const auto& button = static_cast<OverlayButton&>(*element);

        if (ImGui::Button(button.m_label.c_str()))
          button.m_actionClick();

        break;
      }

      case OverlayElementType::CHECKBOX:
      {
        auto& checkbox = static_cast<OverlayCheckbox&>(*element);
        const bool prevValue = checkbox.m_isChecked;

        ImGui::Checkbox(checkbox.m_label.c_str(), &checkbox.m_isChecked);

        if (checkbox.m_isChecked != prevValue) {
          if (checkbox.m_isChecked)
            checkbox.m_actionOn();
          else
            checkbox.m_actionOff();
        }

        break;
      }

      case OverlayElementType::SLIDER:
      {
        auto& slider = static_cast<OverlaySlider&>(*element);

        if (ImGui::SliderFloat(slider.m_label.c_str(), &slider.m_currentValue, slider.m_minValue, slider.m_maxValue))
          slider.m_actionSlide(slider.m_currentValue);

        break;
      }

      case OverlayElementType::TEXTBOX:
      {
        auto& textbox = static_cast<OverlayTextbox&>(*element);

        if (ImGui::InputText(textbox.m_label.c_str(), &textbox.m_text))
          textbox.m_callback(textbox.m_text);

        break;
      }

      case OverlayElementType::LIST_BOX:
      {
        auto& listBox = static_cast<OverlayListBox&>(*element);

        // The list box will get a default width, while being automatically resized vertically up to 5 elements
        // The stride added is to avoid showing a scrollbar when having few entries. Its value is directly defined here,
        //  but may be required to be ImGui::GetStyle().ItemSpacing.y / 2
        const ImVec2 dimensions(0, ImGui::GetTextLineHeightWithSpacing() * std::min(static_cast<float>(listBox.m_entries.size()), 5.f) + 2.f);

        if (ImGui::BeginListBox(listBox.m_label.c_str(), dimensions)) {
          for (std::size_t i = 0; i < listBox.m_entries.size(); ++i) {
            const bool isSelected = (listBox.m_currentId == i);

            if (ImGui::Selectable(listBox.m_entries[i].c_str(), isSelected)) {
              if (!isSelected) { // If the item isn't already selected
                listBox.m_actionChanged(listBox.m_entries[i], i);
                listBox.m_currentId = i;
              }
            }

            if (isSelected)
              ImGui::SetItemDefaultFocus();
          }

          ImGui::EndListBox();
        }

        break;
      }

      case OverlayElementType::DROPDOWN:
      {
        auto& dropdown = static_cast<OverlayDropdown&>(*element);

        if (ImGui::BeginCombo(dropdown.m_label.c_str(), dropdown.m_entries[dropdown.m_currentId].c_str())) {
          for (std::size_t i = 0; i < dropdown.m_entries.size(); ++i) {
            const bool isSelected = (dropdown.m_currentId == i);

            if (ImGui::Selectable(dropdown.m_entries[i].c_str(), isSelected)) {
              if (!isSelected) { // If the item isn't already selected
                dropdown.m_actionChanged(dropdown.m_entries[i], i);
                dropdown.m_currentId = i;
              }
            }

            if (isSelected)
              ImGui::SetItemDefaultFocus();
          }

          ImGui::EndCombo();
        }

        break;
      }

      case OverlayElementType::TEXTURE:
      {
        auto& texture = static_cast<OverlayTexture&>(*element);
        assert("Error: The given texture is invalid." && Renderer::isTexture(texture.m_index));

        const float minRatio = std::min(ImGui::GetWindowWidth() / texture.m_width, ImGui::GetWindowHeight() / texture.m_height);
        const ImVec2 textureSize(std::min(texture.m_width, texture.m_width * minRatio),
                                 std::min(texture.m_height, texture.m_height * minRatio));

        // The UV's y component must be reverted, so that the texture isn't flipped upside down
        const ImVec2 topCoords(0.f, 1.f);
        const ImVec2 bottomCoords(1.f, 0.f);

        ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture.m_index)), textureSize, topCoords, bottomCoords);

        break;
      }

      case OverlayElementType::PROGRESS_BAR:
      {
        auto& progressBar = static_cast<OverlayProgressBar&>(*element);

        const std::string text = (progressBar.m_showValues ? std::to_string(progressBar.m_curVal) + '/' + std::to_string(progressBar.m_maxVal) : std::string());
        ImGui::ProgressBar(static_cast<float>(progressBar.m_minVal + progressBar.m_curVal) / static_cast<float>(progressBar.m_maxVal),
                           ImVec2(-1.f, 0.f),
                           (text.empty() ? nullptr : text.c_str()));

        break;
      }

      case OverlayElementType::SEPARATOR:
        ImGui::Separator();
        break;

      case OverlayElementType::FRAME_TIME:
        ImGui::Text(element->m_label.c_str(), static_cast<double>(1000.f / ImGui::GetIO().Framerate));
        break;

      case OverlayElementType::FPS_COUNTER:
        ImGui::Text(element->m_label.c_str(), static_cast<double>(ImGui::GetIO().Framerate));
        break;

      default:
        break;
    }
  }

  ImGui::End();
}

} // namespace Raz