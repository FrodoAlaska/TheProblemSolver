#pragma once

#include "defines.h"
#include "ui/ui_text.h"
#include "ui/ui_anchor.h"
#include "resources/font.h"

#include <glm/glm.hpp>

#include <string> 

// UIButtonState
/////////////////////////////////////////////////////////////////////////////////
enum UIButtonState {
  BUTTON_STATE_HOVERED, 
  BUTTON_STATE_PRESSED,
};
/////////////////////////////////////////////////////////////////////////////////
 
// Callbacks
/////////////////////////////////////////////////////////////////////////////////
struct UIButton; // Have to do this because the function pointer requires it

// A callback that will be called everytime the button is pressed or hovered
typedef void(*UIButtonCallback)(UIButton*, const UIButtonState, void*);
/////////////////////////////////////////////////////////////////////////////////

// UIButton
/////////////////////////////////////////////////////////////////////////////////
struct UIButton {
  UIText text;

  glm::vec2 position, size, offset;
  glm::vec4 color;

  bool is_active, is_disabled;

  void* user_data; 
  UIButtonCallback callback;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void ui_button_create(UIButton* button, 
                      Font* font, 
                      const std::string& str, 
                      const f32 font_size,
                      UIAnchor anc, 
                      const glm::vec4& button_color, 
                      const glm::vec4& text_color, 
                      const glm::vec2 offset = glm::vec2(0.0f));
bool ui_button_hovered(UIButton* button);
bool ui_button_pressed(UIButton* button);
void ui_button_render(UIButton* button);
/////////////////////////////////////////////////////////////////////////////////
