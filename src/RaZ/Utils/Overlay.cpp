#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "RaZ/Utils/Overlay.hpp"

namespace Raz {

Overlay::Overlay(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 core");
  ImGui::StyleColorsDark();
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

void Overlay::addSeparator() {
  m_elements.emplace_back(OverlaySeparator::create());
}

void Overlay::addFrameTime(std::string formattedLabel) {
  m_elements.emplace_back(OverlayFrameTime::create(std::move(formattedLabel)));
}

void Overlay::addFpsCounter(std::string formattedLabel) {
  m_elements.emplace_back(OverlayFpsCounter::create(std::move(formattedLabel)));
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

      case OverlayElementType::BUTTON: {
        const auto& button = static_cast<OverlayButton&>(*element);

        if (ImGui::Button(button.label.c_str()))
          button.action();

        break;
      }

      case OverlayElementType::CHECKBOX: {
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

      case OverlayElementType::SEPARATOR:
        ImGui::Separator();
        break;

      case OverlayElementType::FRAME_TIME:
        ImGui::Text(element->label.c_str(), 1000.f / ImGui::GetIO().Framerate);
        break;

      case OverlayElementType::FPS_COUNTER:
        ImGui::Text(element->label.c_str(), ImGui::GetIO().Framerate);
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
