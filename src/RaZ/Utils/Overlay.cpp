#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "RaZ/Utils/Overlay.hpp"

namespace Raz {

Overlay::Overlay(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_Init(window, true);
  ImGui::StyleColorsDark();
}

void Overlay::addElement(OverlayElementType type, const std::string& text,
                         std::function<void()> actionOn, std::function<void()> actionOff) {
  if (type == OverlayElementType::CHECKBOX)
    m_toggles.emplace(m_elements.size(), false);

  m_elements.emplace_back(std::make_tuple(type, text, std::move(actionOn), std::move(actionOff)));
}

void Overlay::addText(const std::string& text) {
  addElement(OverlayElementType::TEXT, text);
}

void Overlay::addButton(const std::string& text, std::function<void()> action) {
  addElement(OverlayElementType::BUTTON, text, std::move(action));
}

void Overlay::addCheckbox(const std::string& text, bool initVal, std::function<void()> actionOn, std::function<void()> actionOff) {
  addElement(OverlayElementType::CHECKBOX, text, std::move(actionOn), std::move(actionOff));
  m_toggles[m_elements.size() - 1] = initVal;
}

void Overlay::addSeparator() {
  addElement(OverlayElementType::SEPARATOR);
}

void Overlay::addFrameTime(const std::string& formattedText) {
  addElement(OverlayElementType::FRAME_TIME, formattedText);
}

void Overlay::addFpsCounter(const std::string& formattedText) {
  addElement(OverlayElementType::FPS_COUNTER, formattedText);
}

void Overlay::render() {
  ImGui_ImplGlfw_NewFrame();

  for (std::size_t eltIndex = 0; eltIndex < m_elements.size(); ++eltIndex) {
    const auto& element = m_elements[eltIndex];

    switch (std::get<0>(element)) {
      case OverlayElementType::TEXT:
        ImGui::TextUnformatted(std::get<1>(element).c_str());
        break;

      case OverlayElementType::BUTTON:
        if (ImGui::Button(std::get<1>(element).c_str()))
          std::get<2>(element)();
        break;

      case OverlayElementType::CHECKBOX: {
        bool prevValue = m_toggles[eltIndex];
        ImGui::Checkbox(std::get<1>(element).c_str(), &m_toggles[eltIndex]);

        if (m_toggles[eltIndex] != prevValue) {
          if (m_toggles[eltIndex])
            std::get<2>(element)();
          else
            std::get<3>(element)();
        }
        break;
      }

      case OverlayElementType::SEPARATOR:
        ImGui::Separator();
        break;

      case OverlayElementType::FRAME_TIME:
        ImGui::Text(std::get<1>(element).c_str(), 1000.f / ImGui::GetIO().Framerate);
        break;

      case OverlayElementType::FPS_COUNTER:
        ImGui::Text(std::get<1>(element).c_str(), ImGui::GetIO().Framerate);
        break;
    }
  }

  ImGui::Render();
  ImGui_ImplGlfw_RenderDrawData(ImGui::GetDrawData());
}

Overlay::~Overlay() {
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

} // namespace Raz
