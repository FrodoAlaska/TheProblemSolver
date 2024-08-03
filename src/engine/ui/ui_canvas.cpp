#include "ui_canvas.h"
#include "ui/ui_text.h"
#include "ui/ui_button.h"
#include "ui/ui_anchor.h"
#include "resources/font.h"
#include "defines.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
UICanvas* ui_canvas_create(Font* font, const bool active) {
  UICanvas* canvas = new UICanvas{};
  canvas->font = font;
  canvas->is_active = active;
  canvas->offset = glm::vec2(0.0f);
  canvas->current_offset = glm::vec2(0.0f);
  canvas->current_anchor = UI_ANCHOR_TOP_LEFT;

  return canvas;
}

void ui_canvas_destroy(UICanvas* canvas) {
  canvas->texts.clear();
  canvas->buttons.clear();

  delete canvas;
}

void ui_canvas_begin(UICanvas* canvas, const glm::vec2& offset, const UIAnchor start_anchor) {
  canvas->offset = offset;
  canvas->current_offset = glm::vec2(0.0f);//offset;
  canvas->current_anchor = start_anchor;
}

void ui_canvas_end(UICanvas* canvas) {
  canvas->current_offset = glm::vec2(0.0f);
  canvas->offset = glm::vec2(0.0f);
  canvas->current_anchor = UI_ANCHOR_TOP_LEFT;
}

UIText& ui_canvas_push_text(UICanvas* canvas, 
                            const std::string& text, 
                            const f32 font_size,
                            const glm::vec4& text_color, 
                            const UIAnchor anc, 
                            const glm::vec2& offset) {
  UIText txt; 
  ui_text_create(&txt, 
                 canvas->font, 
                 text, 
                 font_size, 
                 anc, 
                 text_color,
                 canvas->current_offset);

  canvas->texts.push_back(txt);
  canvas->current_offset += offset; 

  return *canvas->texts.end();
}

UIText& ui_canvas_push_text(UICanvas* canvas, 
                            const std::string& text, 
                            const f32 font_size,
                            const glm::vec4& text_color) {
  return ui_canvas_push_text(canvas, text, font_size, text_color, canvas->current_anchor, canvas->offset);
}

UIButton& ui_canvas_push_button(UICanvas* canvas, 
                                const std::string& text, 
                                const f32 font_size,
                                const glm::vec4& button_color, 
                                const glm::vec4& text_color, 
                                void* user_data,
                                UIButtonCallback callback,
                                const UIAnchor anc, 
                                const glm::vec2& offset) {
  UIButton button; 
  ui_button_create(&button, 
                   canvas->font, 
                   text, 
                   font_size, 
                   anc, 
                   button_color, 
                   text_color, 
                   canvas->current_offset);
  button.user_data = user_data;
  button.callback = callback;

  canvas->buttons.push_back(button);
  canvas->current_offset += offset; 

  return *canvas->buttons.end();
}

UIButton& ui_canvas_push_button(UICanvas* canvas, 
                                const std::string& text, 
                                const f32 font_size,
                                const glm::vec4& button_color, 
                                const glm::vec4& text_color, 
                                void* user_data, 
                                UIButtonCallback callback) {
  return ui_canvas_push_button(canvas, 
                               text, 
                               font_size,
                               button_color, 
                               text_color,
                               user_data,
                               callback,
                               canvas->current_anchor, 
                               canvas->offset);
}

void ui_canvas_render(UICanvas* canvas) {
  if(!canvas->is_active) {
    return;
  }

  for(auto& button : canvas->buttons) {
    ui_button_render(&button);
  }
  
  for(auto& text : canvas->texts) {
    ui_text_render(&text);
  }
}

/////////////////////////////////////////////////////////////////////////////////
