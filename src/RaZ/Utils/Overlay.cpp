#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "RaZ/Utils/Overlay.hpp"

namespace Raz {

Overlay::Overlay(GLFWwindow* window) {
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Overlay::addLabel(std::string label) {
  m_elements.emplace_back(OverlayLabel::create(std::move(label)));
}

void Overlay::addButton(std::string label, std::function<void()> action) {
  m_elements.emplace_back(OverlayButton::create(std::move(label), std::move(action)));
}

void Overlay::addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  m_elements.emplace_back(OverlayCheckbox::create(std::move(label), std::move(actionOn), std::move(actionOff), initVal));
}

void Overlay::addTextbox(std::string label, std::function<void(const std::string&)> callback) {
  m_elements.emplace_back(OverlayTextbox::create(std::move(label), std::move(callback)));
  static_cast<OverlayTextbox&>(*m_elements.back()).text.reserve(64);
}

void Overlay::addSeparator() {
  m_elements.emplace_back(OverlaySeparator::create());
}

void Overlay::addFrameTime(std::string formattedLabel) {
  m_elements.emplace_back(OverlayFrameTime::create(std::move(formattedLabel)));
}

void Overlay::addFpsCounter(std::string formattedLabel) {
  m_elements.emplace_back(OverlayFpsCounter::create(std::move(formattedLabel)));
}

bool Overlay::hasKeyboardFocus() const {
  return ImGui::GetIO().WantCaptureKeyboard;
}

void Overlay::render() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  for (auto& element : m_elements) {
    switch (element->getType()) {
      case OverlayElementType::LABEL:
        ImGui::TextUnformatted(element->label.c_str());
        break;

      case OverlayElementType::BUTTON:
      {
        const auto& button = static_cast<OverlayButton&>(*element);

        if (ImGui::Button(button.label.c_str()))
          button.action();

        break;
      }

      case OverlayElementType::CHECKBOX:
      {
        auto& checkbox = static_cast<OverlayCheckbox&>(*element);
        const bool prevValue = checkbox.isChecked;

        ImGui::Checkbox(checkbox.label.c_str(), &checkbox.isChecked);

        if (checkbox.isChecked != prevValue) {
          if (checkbox.isChecked)
            checkbox.actionOn();
          else
            checkbox.actionOff();
        }

        break;
      }

      case OverlayElementType::TEXTBOX:
      {
        static auto callback = [] (ImGuiTextEditCallbackData* data) {
          auto& textbox = *static_cast<OverlayTextbox*>(data->UserData);

          textbox.text += static_cast<char>(data->EventChar);

          if (textbox.callback)
            textbox.callback(textbox.text);

          return 0;
        };

        auto& textbox = static_cast<OverlayTextbox&>(*element);

        ImGui::InputText(textbox.label.c_str(),
                         textbox.text.data(),
                         textbox.text.capacity(),
                         ImGuiInputTextFlags_CallbackCharFilter,
                         callback,
                         &textbox);

        break;
      }

      case OverlayElementType::SEPARATOR:
        ImGui::Separator();
        break;

      case OverlayElementType::FRAME_TIME:
        ImGui::Text(element->label.c_str(), static_cast<double>(1000.f / ImGui::GetIO().Framerate));
        break;

      case OverlayElementType::FPS_COUNTER:
        ImGui::Text(element->label.c_str(), static_cast<double>(ImGui::GetIO().Framerate));
        break;

      default:
        break;
    }
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Overlay::~Overlay() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

} // namespace Raz
