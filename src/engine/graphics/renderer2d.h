#pragma once

#include "defines.h"

#include "resources/texture.h"
#include "resources/font.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>

// Rect
/////////////////////////////////////////////////////////////////////////////////
struct Rect {
  f32 x, y, width, height;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const bool renderer2d_create();
void renderer2d_destroy();
void renderer2d_flush();
void renderer2d_begin();
void renderer2d_end();

// Set the default font the renderer will use. 
void renderer2d_set_default_font(Font* font);

// Will return the default font the renderer uses. 
// NOTE: Will return a 'nullptr' if the default font is not set.
Font* renderer2d_get_default_font();

// Render geometry functions
void render_quad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

// Render texture functions
void render_texture(Texture* texture, const Rect& src, const Rect& dest, const glm::vec4& tint = glm::vec4(1.0f), const bool flip = false);
void render_texture(Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& tint = glm::vec4(1.0f));

// Renders the text with the given font.
void render_text(const Font* font, const f32 size, const std::string& text, const glm::vec2& position, const glm::vec4& color);

// Renders the text with the default font. 
// NOTE: The default font MUST be set with the 'renderer2d_set_default_font' function at the 
// initial startup of the application
void render_text(const f32 size, const std::string& text, const glm::vec2& position, const glm::vec4& color);
/////////////////////////////////////////////////////////////////////////////////
