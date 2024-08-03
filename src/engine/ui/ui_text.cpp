#include "ui_text.h"
#include "defines.h"
#include "resources/font.h"
#include "ui/ui_anchor.h"
#include "core/window.h"
#include "graphics/renderer2d.h"

#include <glm/glm.hpp>

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
glm::vec2 ui_text_measure_size(UIText* text) {
  glm::vec2 result(0.0f);
  f32 scale = text->font_size / text->font->base_size;
  result.y = text->font->base_size;

  for(u32 i = 0; i < text->str.size(); i++) {
    usizei index = font_get_glyph_index(text->font, text->str[i]);
    Glyph glyph = text->font->glyphs[index];

    // Measure the height 
    if(glyph.unicode == '\n') {
      result.y += text->font->line_gap; 
      // @TODO: This will not suffice. You'll need to reset the width of the 
      // text as well since the next line could have more characters.
      continue;
    } 

    // Measure the width 
    if(glyph.advance_x != 0) {
      result.x += glyph.advance_x;
    } 
    else {
      result.x += glyph.width + glyph.x_offset;
    }
  } 

  return result * scale;
}

void ui_text_set_position(UIText* text, UIAnchor anc) {
  glm::vec2 text_size = ui_text_measure_size(text);
  glm::vec2 window_size = window_get_size();
  glm::vec2 window_center = window_size / 2.0f;
  glm::vec2 padding = glm::vec2(10.0f, 10.0f);

  switch(anc) {
    case UI_ANCHOR_TOP_LEFT:  
      text->position = padding + text->offset;
      break;
    case UI_ANCHOR_TOP_CENTER:
      text->position.x = (window_center.x - text_size.x / 2.0f) + text->offset.x; 
      text->position.y = padding.y + text->offset.y; 
      break;
    case UI_ANCHOR_TOP_RIGHT:
      text->position.x = (window_size.x - text_size.x - padding.x) + text->offset.x; 
      text->position.y = padding.y + text->offset.y;  
      break;
    
    case UI_ANCHOR_CENTER_LEFT:  
      text->position.x = padding.x + text->offset.x;
      text->position.y = (window_center.y - text_size.y / 2.0f) + text->offset.y; 
      break;
    case UI_ANCHOR_CENTER:
      text->position = (window_center - text_size / 2.0f) + text->offset;
      break;
    case UI_ANCHOR_CENTER_RIGHT:
      text->position.x = (window_size.x - text_size.x - padding.x) + text->offset.x; 
      text->position.y = (window_center.y - text_size.y / 2.0f) + text->offset.y; 
      break;
    
    case UI_ANCHOR_BOTTOM_LEFT:  
      text->position.x = padding.x + text->offset.x;
      text->position.y = (window_size.y - text_size.y - padding.y) + text->offset.y; 
      break;
    case UI_ANCHOR_BOTTOM_CENTER:
      text->position.x = (window_center.x - text_size.x / 2.0f) + text->offset.x;
      text->position.y = (window_size.y - text_size.y - padding.y) + text->offset.y; 
      break;
    case UI_ANCHOR_BOTTOM_RIGHT:
      text->position = (window_size - text_size - padding) + text->offset; 
      break;
  }
}

void ui_text_create(UIText* text, Font* font, const std::string& str, f32 font_size, UIAnchor anc, const glm::vec4& color, const glm::vec2 offset) {
  text->offset = offset;
  
  text->font_size = font_size;
  text->anchor = anc; 
  
  text->color = color;
  text->str = str; 
  
  text->is_active = true;

  text->font = font == nullptr ? renderer2d_get_default_font() : font;

  ui_text_set_position(text, anc);
}

void ui_text_set_string(UIText* text, const std::string& new_str) {
  text->str = new_str;
  ui_text_set_position(text, text->anchor);
}

void ui_text_render(UIText* text) {
  if(!text->is_active) {
    return;
  }

  render_text(text->font, text->font_size, text->str, text->position, text->color);
}
/////////////////////////////////////////////////////////////////////////////////
