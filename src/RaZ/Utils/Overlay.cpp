#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "RaZ/Utils/CompilerUtils.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Overlay.hpp"

namespace Raz {

void Overlay::initialize(GLFWwindow* windowHandle) const {
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

OverlayWindow& Overlay::addWindow(std::string title) {
  return m_windows.emplace_back(std::move(title));
}

bool Overlay::hasKeyboardFocus() const {
  return ImGui::GetIO().WantCaptureKeyboard;
}

bool Overlay::hasMouseFocus() const {
  return ImGui::GetIO().WantCaptureMouse;
}

void Overlay::render() const {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  for (const OverlayWindow& window : m_windows)
    window.render();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Overlay::destroy() const {
  Logger::debug("[Overlay] Destroying...");

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

  Logger::debug("[Overlay] Destroyed");
}

void OverlayWindow::addLabel(std::string label) {
  m_elements.emplace_back(std::make_unique<OverlayLabel>(std::move(label)));
}

void OverlayWindow::addColoredLabel(std::string label, Vec4f color) {
  m_elements.emplace_back(std::make_unique<OverlayColoredLabel>(std::move(label), color));
}

void OverlayWindow::addColoredLabel(std::string label, float red, float green, float blue, float alpha) {
  addColoredLabel(std::move(label), Vec4f(red, green, blue, alpha));
}

void OverlayWindow::addButton(std::string label, std::function<void()> actionClick) {
  m_elements.emplace_back(std::make_unique<OverlayButton>(std::move(label), std::move(actionClick)));
}

void OverlayWindow::addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  m_elements.emplace_back(std::make_unique<OverlayCheckbox>(std::move(label), std::move(actionOn), std::move(actionOff), initVal));
}

void OverlayWindow::addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue) {
  m_elements.emplace_back(std::make_unique<OverlaySlider>(std::move(label), std::move(actionSlide), minValue, maxValue, initValue));
}

void OverlayWindow::addTextbox(std::string label, std::function<void(const std::string&)> callback) {
  m_elements.emplace_back(std::make_unique<OverlayTextbox>(std::move(label), std::move(callback)));
  static_cast<OverlayTextbox&>(*m_elements.back()).m_text.reserve(64);
}

void OverlayWindow::addListBox(std::string label, std::vector<std::string> entries,
                               std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId) {
  if (entries.empty()) {
    Logger::error("[Overlay] Cannot create a list box with no entry.");
    return;
  }

  assert("Error: A list box's initial index cannot reference a non-existing entry." && initId < entries.size());
  m_elements.emplace_back(std::make_unique<OverlayListBox>(std::move(label), std::move(entries), std::move(actionChanged), initId));
}

void OverlayWindow::addDropdown(std::string label, std::vector<std::string> entries,
                                std::function<void(const std::string&, std::size_t)> actionChanged, std::size_t initId) {
  if (entries.empty()) {
    Logger::error("[Overlay] Cannot create a dropdown list with no entry.");
    return;
  }

  assert("Error: A dropdown's initial index cannot reference a non-existing entry." && initId < entries.size());
  m_elements.emplace_back(std::make_unique<OverlayDropdown>(std::move(label), std::move(entries), std::move(actionChanged), initId));
}

void OverlayWindow::addTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight) {
  m_elements.emplace_back(std::make_unique<OverlayTexture>(texture, maxWidth, maxHeight));
}

void OverlayWindow::addTexture(const Texture& texture) {
  m_elements.emplace_back(std::make_unique<OverlayTexture>(texture));
}

OverlayProgressBar& OverlayWindow::addProgressBar(int minVal, int maxVal, bool showValues) {
  return static_cast<OverlayProgressBar&>(*m_elements.emplace_back(std::make_unique<OverlayProgressBar>(minVal, maxVal, showValues)));
}

void OverlayWindow::addSeparator() {
  m_elements.emplace_back(std::make_unique<OverlaySeparator>());
}

void OverlayWindow::addFrameTime(std::string formattedLabel) {
  m_elements.emplace_back(std::make_unique<OverlayFrameTime>(std::move(formattedLabel)));
}

void OverlayWindow::addFpsCounter(std::string formattedLabel) {
  m_elements.emplace_back(std::make_unique<OverlayFpsCounter>(std::move(formattedLabel)));
}

void OverlayWindow::render() const {
  ImGui::Begin(m_title.c_str());

  for (const auto& element : m_elements) {
    switch (element->getType()) {
      case OverlayElementType::LABEL:
        ImGui::TextUnformatted(element->m_label.c_str());
        break;

      case OverlayElementType::COLORED_LABEL:
      {
        const auto& label = static_cast<OverlayColoredLabel&>(*element);
        ImGui::TextColored(ImVec4(label.m_color.x(), label.m_color.y(), label.m_color.z(), label.m_color.w()), "%s", element->m_label.c_str());
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
        static auto callback = [] (ImGuiInputTextCallbackData* data) {
          auto& textbox = *static_cast<OverlayTextbox*>(data->UserData);

          textbox.m_text += static_cast<char>(data->EventChar);

          if (textbox.m_callback)
            textbox.m_callback(textbox.m_text);

          return 0;
        };

        auto& textbox = static_cast<OverlayTextbox&>(*element);

        ImGui::InputText(textbox.m_label.c_str(),
                         &textbox.m_text,
                         ImGuiInputTextFlags_CallbackCharFilter,
                         callback,
                         &textbox);

        break;
      }

      case OverlayElementType::LIST_BOX:
      {
        auto& listBox = static_cast<OverlayListBox&>(*element);

        if (ImGui::BeginListBox(listBox.m_label.c_str())) {
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
