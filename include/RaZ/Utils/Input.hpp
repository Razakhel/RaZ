#pragma once

#ifndef RAZ_INPUT_HPP
#define RAZ_INPUT_HPP

#include "GLFW/glfw3.h"

namespace Raz {

namespace Keyboard {

enum Key {
  // Letter keys
  A = GLFW_KEY_A,
  B = GLFW_KEY_B,
  C = GLFW_KEY_C,
  D = GLFW_KEY_D,
  E = GLFW_KEY_E,
  F = GLFW_KEY_F,
  G = GLFW_KEY_G,
  H = GLFW_KEY_H,
  I = GLFW_KEY_I,
  J = GLFW_KEY_J,
  K = GLFW_KEY_K,
  L = GLFW_KEY_L,
  M = GLFW_KEY_M,
  N = GLFW_KEY_N,
  O = GLFW_KEY_O,
  P = GLFW_KEY_P,
  Q = GLFW_KEY_Q,
  R = GLFW_KEY_R,
  S = GLFW_KEY_S,
  T = GLFW_KEY_T,
  U = GLFW_KEY_U,
  V = GLFW_KEY_V,
  W = GLFW_KEY_W,
  X = GLFW_KEY_X,
  Y = GLFW_KEY_Y,
  Z = GLFW_KEY_Z,

  // Function keys
  F1  = GLFW_KEY_F1,
  F2  = GLFW_KEY_F2,
  F3  = GLFW_KEY_F3,
  F4  = GLFW_KEY_F4,
  F5  = GLFW_KEY_F5,
  F6  = GLFW_KEY_F6,
  F7  = GLFW_KEY_F7,
  F8  = GLFW_KEY_F8,
  F9  = GLFW_KEY_F9,
  F10 = GLFW_KEY_F10,
  F11 = GLFW_KEY_F11,
  F12 = GLFW_KEY_F12,

  // Arrow keys
  UP    = GLFW_KEY_UP,
  DOWN  = GLFW_KEY_DOWN,
  RIGHT = GLFW_KEY_RIGHT,
  LEFT  = GLFW_KEY_LEFT,

  // Numpad
  NUMLOCK   = GLFW_KEY_NUM_LOCK,
  ADD       = GLFW_KEY_KP_ADD,
  DECIMAL   = GLFW_KEY_KP_DECIMAL,
  SUBSTRACT = GLFW_KEY_KP_SUBTRACT,
  MULTIPLY  = GLFW_KEY_KP_MULTIPLY,
  DIVIDE    = GLFW_KEY_KP_DIVIDE,
  NUM0      = GLFW_KEY_KP_0,
  NUM1      = GLFW_KEY_KP_1,
  NUM2      = GLFW_KEY_KP_2,
  NUM3      = GLFW_KEY_KP_3,
  NUM4      = GLFW_KEY_KP_4,
  NUM5      = GLFW_KEY_KP_5,
  NUM6      = GLFW_KEY_KP_6,
  NUM7      = GLFW_KEY_KP_7,
  NUM8      = GLFW_KEY_KP_8,
  NUM9      = GLFW_KEY_KP_9,

  // Modifiers
  LEFT_SHIFT  = GLFW_KEY_LEFT_SHIFT,
  RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
  LEFT_CTRL   = GLFW_KEY_LEFT_CONTROL,
  RIGHT_CTRL  = GLFW_KEY_RIGHT_CONTROL,
  LEFT_ALT    = GLFW_KEY_LEFT_ALT,
  RIGHT_ALT   = GLFW_KEY_RIGHT_ALT,

  // Miscellaneous
  HOME      = GLFW_KEY_HOME,
  END       = GLFW_KEY_END,
  PAGEUP    = GLFW_KEY_PAGE_UP,
  PAGEDOWN  = GLFW_KEY_PAGE_DOWN,
  CAPSLOCK  = GLFW_KEY_CAPS_LOCK,
  BACKSPACE = GLFW_KEY_BACKSPACE,
  SPACE     = GLFW_KEY_SPACE,
  ESCAPE    = GLFW_KEY_ESCAPE
};

} // namespace Keyboard

namespace Mouse {

enum MouseButton {
  LEFT_CLICK   = GLFW_MOUSE_BUTTON_LEFT,
  RIGHT_CLICK  = GLFW_MOUSE_BUTTON_RIGHT,
  MIDDLE_CLICK = GLFW_MOUSE_BUTTON_MIDDLE
};

enum MouseWheel {
  SCROLL
};

} // namespace Mouse

} // namespace Raz

#endif // RAZ_INPUT_HPP
