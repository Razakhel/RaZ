#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "RaZ/Utils/Overlay.hpp"

namespace Raz {

Overlay::Overlay(GLFWwindow* window) {
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);

#if !defined(RAZ_PLATFORM_EMSCRIPTEN)
  ImGui_ImplOpenGL3_Init("#version 330 core");
#else
  ImGui_ImplOpenGL3_Init("#version 300 es");
#endif
}

void Overlay::addLabel(std::string label) {
  m_elements.emplace_back(OverlayLabel::create(std::move(label)));
}

void Overlay::addButton(std::string label, std::function<void()> actionClick) {
  m_elements.emplace_back(OverlayButton::create(std::move(label), std::move(actionClick)));
}

void Overlay::addCheckbox(std::string label, std::function<void()> actionOn, std::function<void()> actionOff, bool initVal) {
  m_elements.emplace_back(OverlayCheckbox::create(std::move(label), std::move(actionOn), std::move(actionOff), initVal));
}

void Overlay::addSlider(std::string label, std::function<void(float)> actionSlide, float minValue, float maxValue, float initValue) {
  m_elements.emplace_back(OverlaySlider::create(std::move(label), std::move(actionSlide), minValue, maxValue, initValue));
}

void Overlay::addTextbox(std::string label, std::function<void(const std::string&)> callback) {
  m_elements.emplace_back(OverlayTextbox::create(std::move(label), std::move(callback)));
  static_cast<OverlayTextbox&>(*m_elements.back()).m_text.reserve(64);
}

#if !defined(RAZ_USE_VULKAN)
void Overlay::addTexture(const Texture& texture, unsigned int maxWidth, unsigned int maxHeight) {
  m_elements.emplace_back(OverlayTexture::create(texture, maxWidth, maxHeight));
}

void Overlay::addTexture(const Texture& texture) {
  m_elements.emplace_back(OverlayTexture::create(texture));
}
#endif

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
        ImGui::TextUnformatted(element->m_label.c_str());
        break;

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
        static auto callback = [] (ImGuiTextEditCallbackData* data) {
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

#if !defined(RAZ_USE_VULKAN)
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
#endif

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

  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Overlay::~Overlay() {
  ImGui_ImplOpenGL3_Shutdown();

  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

} // namespace Raz
