#include "ui_button.h"
#include "core/input.h"
#include "defines.h"
#include "ui/ui_text.h"
#include "ui/ui_anchor.h"
#include "resources/font.h"
#include "graphics/renderer2d.h"

#include <glm/glm.hpp>

#include <string> 

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void ui_button_create(UIButton* button, 
                      Font* font, 
                      const std::string& str, 
                      const f32 font_size,
                      UIAnchor anc, 
                      const glm::vec4& button_color, 
                      const glm::vec4& text_color, 
                      const glm::vec2 offset) {
  ui_text_create(&button->text, font, str, font_size, anc, text_color, offset);

  button->size = ui_text_measure_size(&button->text) + 10.0f;
  button->position = button->text.position - 8.0f; // @TODO: Must have a more generic way of determing the margin to remove

  button->offset = offset; 
  button->color = button_color;
  button->is_active = true;
  button->is_disabled = false;

  button->user_data = nullptr;
  button->callback = nullptr;
}

bool ui_button_hovered(UIButton* button) {
  glm::vec2 mouse_pos = input_mouse_pos();

  return ((mouse_pos.x >= button->position.x) && (mouse_pos.x <= (button->position.x + button->size.x))) && 
         ((mouse_pos.y >= button->position.y) && (mouse_pos.y <= (button->position.y + button->size.y)));
}

bool ui_button_pressed(UIButton* button) {
  return ui_button_hovered(button) && input_button_down(MOUSE_BUTTON_LEFT);
}

void ui_button_render(UIButton* button) {
  if(!button->is_active) {
    return;
  }

  if(ui_button_pressed(button)) {
    button->color.a = 0.25f;

    if(button->callback) {
      button->callback(button, BUTTON_STATE_PRESSED, button->user_data);
    }
  }
  else if(ui_button_hovered(button)) {
    button->color.a = 0.5f;
    
    if(button->callback) {
      button->callback(button, BUTTON_STATE_HOVERED, button->user_data);
    }
  }
  else if(button->is_disabled) {
    button->color.a = 0.1f;
  }
  else {
    button->color.a = 1.0f;
  }

  render_quad(button->position + button->size / 2.0f, button->size, button->color);
  ui_text_render(&button->text);
}
/////////////////////////////////////////////////////////////////////////////////
