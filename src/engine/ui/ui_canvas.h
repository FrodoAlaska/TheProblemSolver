#pragma once

#include "ui/ui_text.h"
#include "ui/ui_button.h"
#include "ui/ui_anchor.h"
#include "resources/font.h"
#include "defines.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <string>

// UICanvas
/////////////////////////////////////////////////////////////////////////////////
struct UICanvas {
  Font* font;
  bool is_active;

  glm::vec2 offset, current_offset;
  UIAnchor current_anchor;

  std::vector<UIText> texts;
  std::vector<UIButton> buttons;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
UICanvas* ui_canvas_create(Font* font, const bool active = true);
void ui_canvas_destroy(UICanvas* canvas);

void ui_canvas_begin(UICanvas* canvas, const glm::vec2& offset, const UIAnchor start_anchor);
void ui_canvas_end(UICanvas* canvas);

UIText& ui_canvas_push_text(UICanvas* canvas, 
                            const std::string& text, 
                            const f32 font_size,
                            const glm::vec4& text_color, 
                            const UIAnchor anc, 
                            const glm::vec2& offset = glm::vec2(0.0f));
UIText& ui_canvas_push_text(UICanvas* canvas, 
                            const std::string& text, 
                            const f32 font_size,
                            const glm::vec4& text_color);

UIButton& ui_canvas_push_button(UICanvas* canvas, 
                                const std::string& text, 
                                const f32 font_size,
                                const glm::vec4& button_color, 
                                const glm::vec4& text_color, 
                                void* user_data, 
                                UIButtonCallback callback,
                                const UIAnchor anc, 
                                const glm::vec2& offset = glm::vec2(0.0f));
UIButton& ui_canvas_push_button(UICanvas* canvas, 
                                const std::string& text, 
                                const f32 font_size,
                                const glm::vec4& button_color, 
                                const glm::vec4& text_color, 
                                void* user_data, 
                                UIButtonCallback callback);

void ui_canvas_render(UICanvas* canvas);
/////////////////////////////////////////////////////////////////////////////////
