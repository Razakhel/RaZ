#pragma once

#ifndef RAZ_INPUT_HPP
#define RAZ_INPUT_HPP

namespace Raz {

namespace Input {

/// Values defining on which frequency an associated action should take place.
enum ActionTrigger {
  ONCE,  ///< The action will be executed only once when the key/button is pressed/clicked.
  ALWAYS ///< The action will be executed as long as the key/button is pressed/clicked.
};

} // namespace Input

namespace Keyboard {

/// Values defining keyboard's keys.
enum Key {
  // Letter keys
  A = 65 /* GLFW_KEY_A */, ///< Letter key A.
  B = 66 /* GLFW_KEY_B */, ///< Letter key B.
  C = 67 /* GLFW_KEY_C */, ///< Letter key C.
  D = 68 /* GLFW_KEY_D */, ///< Letter key D.
  E = 69 /* GLFW_KEY_E */, ///< Letter key E.
  F = 70 /* GLFW_KEY_F */, ///< Letter key F.
  G = 71 /* GLFW_KEY_G */, ///< Letter key G.
  H = 72 /* GLFW_KEY_H */, ///< Letter key H.
  I = 73 /* GLFW_KEY_I */, ///< Letter key I.
  J = 74 /* GLFW_KEY_J */, ///< Letter key J.
  K = 75 /* GLFW_KEY_K */, ///< Letter key K.
  L = 76 /* GLFW_KEY_L */, ///< Letter key L.
  M = 77 /* GLFW_KEY_M */, ///< Letter key M.
  N = 78 /* GLFW_KEY_N */, ///< Letter key N.
  O = 79 /* GLFW_KEY_O */, ///< Letter key O.
  P = 80 /* GLFW_KEY_P */, ///< Letter key P.
  Q = 81 /* GLFW_KEY_Q */, ///< Letter key Q.
  R = 82 /* GLFW_KEY_R */, ///< Letter key R.
  S = 83 /* GLFW_KEY_S */, ///< Letter key S.
  T = 84 /* GLFW_KEY_T */, ///< Letter key T.
  U = 85 /* GLFW_KEY_U */, ///< Letter key U.
  V = 86 /* GLFW_KEY_V */, ///< Letter key V.
  W = 87 /* GLFW_KEY_W */, ///< Letter key W.
  X = 88 /* GLFW_KEY_X */, ///< Letter key X.
  Y = 89 /* GLFW_KEY_Y */, ///< Letter key Y.
  Z = 90 /* GLFW_KEY_Z */, ///< Letter key Z.

  // Function keys
  F1  = 290 /* GLFW_KEY_F1  */, ///< Function key F1.
  F2  = 291 /* GLFW_KEY_F2  */, ///< Function key F2.
  F3  = 292 /* GLFW_KEY_F3  */, ///< Function key F3.
  F4  = 293 /* GLFW_KEY_F4  */, ///< Function key F4.
  F5  = 294 /* GLFW_KEY_F5  */, ///< Function key F5.
  F6  = 295 /* GLFW_KEY_F6  */, ///< Function key F6.
  F7  = 296 /* GLFW_KEY_F7  */, ///< Function key F7.
  F8  = 297 /* GLFW_KEY_F8  */, ///< Function key F8.
  F9  = 298 /* GLFW_KEY_F9  */, ///< Function key F9.
  F10 = 299 /* GLFW_KEY_F10 */, ///< Function key F10.
  F11 = 300 /* GLFW_KEY_F11 */, ///< Function key F11.
  F12 = 301 /* GLFW_KEY_F12 */, ///< Function key F12.

  // Arrow keys
  UP    = 265 /* GLFW_KEY_UP    */, ///< Up arrow key.
  DOWN  = 264 /* GLFW_KEY_DOWN  */, ///< Down arrow key.
  RIGHT = 262 /* GLFW_KEY_RIGHT */, ///< Right arrow key.
  LEFT  = 263 /* GLFW_KEY_LEFT  */, ///< Left arrow key.

  // Numpad
  NUMLOCK   = 282 /* GLFW_KEY_NUM_LOCK    */, ///< Numeric keypad num lock key.
  NUM0      = 320 /* GLFW_KEY_KP_0        */, ///< Numeric keypad 0 key.
  NUM1      = 321 /* GLFW_KEY_KP_1        */, ///< Numeric keypad 1 key.
  NUM2      = 322 /* GLFW_KEY_KP_2        */, ///< Numeric keypad 2 key.
  NUM3      = 323 /* GLFW_KEY_KP_3        */, ///< Numeric keypad 3 key.
  NUM4      = 324 /* GLFW_KEY_KP_4        */, ///< Numeric keypad 4 key.
  NUM5      = 325 /* GLFW_KEY_KP_5        */, ///< Numeric keypad 5 key.
  NUM6      = 326 /* GLFW_KEY_KP_6        */, ///< Numeric keypad 6 key.
  NUM7      = 327 /* GLFW_KEY_KP_7        */, ///< Numeric keypad 7 key.
  NUM8      = 328 /* GLFW_KEY_KP_8        */, ///< Numeric keypad 8 key.
  NUM9      = 329 /* GLFW_KEY_KP_9        */, ///< Numeric keypad 9 key.
  DECIMAL   = 330 /* GLFW_KEY_KP_DECIMAL  */, ///< Numeric keypad decimal (.) key.
  DIVIDE    = 331 /* GLFW_KEY_KP_DIVIDE   */, ///< Numeric keypad divide (/) key.
  MULTIPLY  = 332 /* GLFW_KEY_KP_MULTIPLY */, ///< Numeric keypad multiply (*) key.
  SUBSTRACT = 333 /* GLFW_KEY_KP_SUBTRACT */, ///< Numeric keypad substract (-) key.
  ADD       = 334 /* GLFW_KEY_KP_ADD      */, ///< Numeric keypad add (+) key.

  // Modifiers
  LEFT_SHIFT  = 340 /* GLFW_KEY_LEFT_SHIFT    */, ///< Left shift key.
  RIGHT_SHIFT = 344 /* GLFW_KEY_RIGHT_SHIFT   */, ///< Right shift key.
  LEFT_CTRL   = 341 /* GLFW_KEY_LEFT_CONTROL  */, ///< Left control key.
  RIGHT_CTRL  = 345 /* GLFW_KEY_RIGHT_CONTROL */, ///< Right control key.
  LEFT_ALT    = 342 /* GLFW_KEY_LEFT_ALT      */, ///< Left alt key.
  RIGHT_ALT   = 346 /* GLFW_KEY_RIGHT_ALT     */, ///< Right alt key.

  // Miscellaneous
  HOME         = 268 /* GLFW_KEY_HOME         */, ///< Home key.
  END          = 269 /* GLFW_KEY_END          */, ///< End key.
  PAGEUP       = 266 /* GLFW_KEY_PAGE_UP      */, ///< Page up key.
  PAGEDOWN     = 267 /* GLFW_KEY_PAGE_DOWN    */, ///< Page down key.
  CAPSLOCK     = 280 /* GLFW_KEY_CAPS_LOCK    */, ///< Caps lock key.
  SPACE        = 32  /* GLFW_KEY_SPACE        */, ///< Space bar key.
  BACKSPACE    = 259 /* GLFW_KEY_BACKSPACE    */, ///< Backspace key.
  INSERT       = 260 /* GLFW_KEY_INSERT       */, ///< Insert key.
  ESCAPE       = 256 /* GLFW_KEY_ESCAPE       */, ///< Escape key.
  PRINT_SCREEN = 283 /* GLFW_KEY_PRINT_SCREEN */, ///< Print screen key.
  PAUSE        = 284 /* GLFW_KEY_PAUSE        */  ///< Pause key.
};

} // namespace Keyboard

namespace Mouse {

/// Values defining mouse buttons.
enum Button {
  LEFT_CLICK   = 0 /* GLFW_MOUSE_BUTTON_LEFT   */, ///< Left mouse button click.
  RIGHT_CLICK  = 1 /* GLFW_MOUSE_BUTTON_RIGHT  */, ///< Right mouse button click.
  MIDDLE_CLICK = 2 /* GLFW_MOUSE_BUTTON_MIDDLE */  ///< Middle mouse button click.
};

} // namespace Mouse

namespace Cursor {

/// Values defining the mouse cursor's state.
enum State {
  NORMAL   = 212993 /* GLFW_CURSOR_NORMAL   */, ///< The cursor is displayed and behaves normally.
  HIDDEN   = 212994 /* GLFW_CURSOR_HIDDEN   */, ///< The cursor stays hidden but exists nonetheless, so that it can freely go out of the window.
  DISABLED = 212995 /* GLFW_CURSOR_DISABLED */  ///< The cursor stays hidden while continuously returning to the center, so that it can't go out of the window.
};

} // namespace Cursor

} // namespace Raz

#endif // RAZ_INPUT_HPP
