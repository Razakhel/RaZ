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

void Overlay::addElement(OverlayElementType type, const std::string& name, std::function<void()> action) {
  if (type == OverlayElementType::CHECKBOX)
    m_toggles.emplace(m_elements.size(), false);

  m_elements.emplace_back(std::make_tuple(type, name, action));
}

void Overlay::render() {
  ImGui_ImplGlfw_NewFrame();

  for (std::size_t eltIndex = 0; eltIndex < m_elements.size(); ++eltIndex) {
    const auto& element = m_elements[eltIndex];

    switch (std::get<0>(element)) {
      case OverlayElementType::TEXT:
        ImGui::Text(std::get<1>(element).c_str());
        break;

      case OverlayElementType::BUTTON:
        if (ImGui::Button(std::get<1>(element).c_str()))
          std::get<2>(element)();
        break;

      case OverlayElementType::CHECKBOX:
        ImGui::Checkbox(std::get<1>(element).c_str(), &m_toggles[eltIndex]);

        if (m_toggles[eltIndex])
          std::get<2>(element)();
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
