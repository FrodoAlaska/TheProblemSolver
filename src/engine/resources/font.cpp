#include "font.h"
#include "defines.h"
#include "resources/texture.h"
#include "utils/utils_file.h"

#include <glad/gl.h>
#include <stb_truetype/stb_truetype.h>
#include <glm/glm.hpp>

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
static u8* get_font_data(const std::string& path) {
  std::ifstream file(path, std::ifstream::binary);
  if(!file.is_open()) {
    fprintf(stderr, "[ERROR]: Failed to open font file \'%s\'\n", path.c_str());
    return nullptr;
  }

  usizei size = file_get_size(file);
  u8* data = new u8[size]; 
  file_read_binary(file, data, size); 

  return data; 
}

static void init_font_chars(Font* font, stbtt_fontinfo* info) {
  /*
   * NOTE: Bits and pieces of this code is borrowed from Raylib. Particularly, from 
   * the "rtext.c" file. Check them out if you have not already. 
   */

  // This will return a value that will get used constantly to put all the values 
  // that come out of the stb in "scaled" coordinates.  
  f32 scale_factor = stbtt_ScaleForPixelHeight(info, font->base_size);

  // Get the ascent, descent, and line gap of the font and put them in 
  // the scaled space 
  i32 ascent, descent, line_gap;
  stbtt_GetFontVMetrics(info, &ascent, &descent, &line_gap);

  font->ascent = ascent * scale_factor;
  font->descent = descent * scale_factor;

  for(u32 i = 0; i < info->numGlyphs; i++) { 
    Glyph glyph;
    glyph.unicode = i + 32;

    // This functions will return 0 if the given unicode is not in 
    // the font. Thus, to speed up the loop, we just skip these unicodes.
    i32 glyph_index = stbtt_FindGlyphIndex(info, glyph.unicode);
    if(glyph_index == 0) {
      continue;
    }

    // Pixels of the specific codepoint, offset, and size 
    u8* mono_bitmap = stbtt_GetGlyphBitmap(info, 
                                           0,
                                           scale_factor, 
                                           glyph_index, 
                                           &glyph.width, 
                                           &glyph.height, 
                                           &glyph.x_offset, 
                                           &glyph.y_offset);

    stbtt_GetGlyphBitmapBox(info, 
                            glyph_index, 
                            scale_factor, 
                            scale_factor, 
                            &glyph.left, 
                            &glyph.top, 
                            &glyph.right, 
                            &glyph.bottom);

    // Only give OpenGL the pixels when it is valid 
    if(mono_bitmap) {
      // Converting the mono bitmap into an RGBA bitmap format to be used more 
      // efficiently with the batch renderer.
      usizei btmp_size = glyph.width * glyph.height * 4;
      u8* bitmap = new u8[btmp_size];
      for(u32 i = 0; i < glyph.height; i++) {
        for(u32 j = 0; j < glyph.width; j++) {
          u32 mono_index = (i * glyph.width) + j; 
          u32 rgba_index = ((i * glyph.width) + j) * 4;

          bitmap[rgba_index] = 255; // Red 
          bitmap[rgba_index + 1] = 255; // Green 
          bitmap[rgba_index + 2] = 255; // Blue 
          bitmap[rgba_index + 3] = mono_bitmap[mono_index]; // Alpha 
        }
      }

      glyph.texture = texture_load(glyph.width, glyph.height, TEXTURE_FORMAT_RGBA, bitmap);
      delete[] bitmap;
    }  
    else { // Probably the space character (' ') so just allocate an empty buffer for it
      u32 pixels = 0x00; 
      glyph.texture = texture_load(1, 1, TEXTURE_FORMAT_RED, &pixels);
    }
    
    // Getting the advance and the left side bearing of the specific codepoint/glyph.
    // The advance is the value required to "advance" to the next glyph
    stbtt_GetGlyphHMetrics(info, glyph_index, &glyph.advance_x, &glyph.left_side_bearing);
    glyph.advance_x *= scale_factor;

    // Getting the kern of the glyph. The kern is used to make some specific glyphs look better 
    // when next to each other.
    glyph.kern = stbtt_GetCodepointKernAdvance(info, glyph.unicode, glyph.unicode + 32); 
    glyph.kern *= scale_factor;
    
    glyph.y_offset += font->ascent;
    glyph.y_offset -= glyph.top / 2;
    glyph.x_offset = glyph.right / 2;

    // Make sure the deallocate the bitmap data that was allocated by STB 
    // NOTE: Potentially really slow to have an allocation and a deallocation in a loop 
    stbtt_FreeBitmap(mono_bitmap, nullptr);
  
    // A valid glyph that was loaded 
    font->glyphs_count++;
    font->glyphs.push_back(glyph);
  }

  // Resizing the vector down only for the loaded glyphs 
  font->glyphs.resize(font->glyphs_count);
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Font* font_load(const std::string& path, const f32 size) {
  Font* font = new Font{};
  font->base_size = size; 

  u8* data = get_font_data(path);
  stbtt_fontinfo info;
  if(stbtt_InitFont(&info, data, stbtt_GetFontOffsetForIndex(data, 0)) == 0) {
    fprintf(stderr, "[ERROR]: Failed to initialize stb for font \'%s\'\n", path.c_str());
    return font;
  }

  font->glyphs.reserve(info.numGlyphs);
  font->glyph_padding = 4.0f;

  // Load the textures and required values for every glyph in the font 
  init_font_chars(font, &info);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  delete[] data;
  return font;
}

void font_unload(Font* font) {
  if(!font) {
    return;
  }

  for(u32 i = 0; i < font->glyphs.size(); i++) {
    texture_unload(font->glyphs[i].texture);
  }
  
  font->glyphs.clear();
  delete font;
  
  font = nullptr;
}

i32 font_get_glyph_index(const Font* font, i8 codepoint) {
  for(u32 i = 0; i < font->glyphs.size(); i++) {
    if(font->glyphs[i].unicode == codepoint) {
      return i;
    }
  }

  return 0;
}
/////////////////////////////////////////////////////////////////////////////////
