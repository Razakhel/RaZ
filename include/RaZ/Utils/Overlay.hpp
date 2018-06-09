#pragma once

#ifndef RAZ_OVERLAY_HPP
#define RAZ_OVERLAY_HPP

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Raz {

enum class OverlayElementType { TEXT,
                                BUTTON,
                                CHECKBOX };

using OverlayElements = std::vector<std::tuple<OverlayElementType, std::string, std::function<void()>>>;

class Overlay {
public:
  explicit Overlay(GLFWwindow* window);

  void addElement(OverlayElementType type, const std::string& name, std::function<void()> = nullptr);
  void render();

  ~Overlay();

private:
  OverlayElements m_elements;
  std::unordered_map<std::size_t, bool> m_toggles;
};

using OverlayPtr = std::unique_ptr<Overlay>;

} // namespace Raz

#endif // RAZ_OVERLAY_HPP
