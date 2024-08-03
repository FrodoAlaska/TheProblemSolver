#pragma once

#include "defines.h"
#include "ui/ui_anchor.h"
#include "resources/font.h"

#include <glm/glm.hpp>

#include <string>

// UIText
/////////////////////////////////////////////////////////////////////////////////
struct UIText {
  glm::vec2 position, offset;

  f32 font_size;
  UIAnchor anchor; 

  glm::vec4 color;
  std::string str;

  bool is_active;

  Font* font;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
// Measure the width and height of each character of the string and return 
// the accumlation of that. The height might be the same for each character 
// depending on the font.
glm::vec2 ui_text_measure_size(UIText* text);

// Set the position of the text based on the given anchor. 
// NOTE: You are encourged to use this function over directly changing 
// the 'position' member of UIText since this will position the text based on 
// the string's size and the window size.
void ui_text_set_position(UIText* text, UIAnchor anc);

// Create a text object and return it through the pointer given. 
// If 'nullptr' was passed to the 'font' parametar, the text will use 
// the default font if it was available. 
//
// NOTE: The 'font' given in this function should be an already loaded font. 
// Don't load a font for every text object. Instead, have a font that will be passed 
// into the function from an external resource manager or perhaps this engine's resource manager.
void ui_text_create(UIText* text, 
                    Font* font, 
                    const std::string& str, 
                    f32 font_size, 
                    UIAnchor anc, 
                    const glm::vec4& color, 
                    const glm::vec2 offset = glm::vec2(0.0f));

// Set the string of the text object to the 'new_str' given. 
// NOTE: Like the 'ui_text_set_position' function, using function 
// is encourged over just directly changing the inner 'str' member.
// This function remeasures the string and resets its position to keep 
// it seamless. If you edit the 'str' member directly it will be out of place.
void ui_text_set_string(UIText* text, const std::string& new_str);

// Render the given text object
void ui_text_render(UIText* text);
/////////////////////////////////////////////////////////////////////////////////
