#include "renderer2d.h"
#include "core/window.h"
#include "defines.h"
#include "math/vertex.h"
#include "graphics/shader.h"

#include "resources/texture.h"
#include "resources/font.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glad/gl.h>

#include <cstddef>
#include <vector>
#include <string>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define MAX_QUADS     10000
#define MAX_VERTICES  MAX_QUADS * 4
#define MAX_INDICES   MAX_QUADS * 6
#define MAX_TEXTURES  32 // @TODO: Probably should query the driver for the max textures instead of assuming
/////////////////////////////////////////////////////////////////////////////////

// Renderer2d
/////////////////////////////////////////////////////////////////////////////////
struct Renderer2D {
  u32 vao, vbo, ebo;

  std::vector<Vertex2D> vertices;
  Texture* textures[MAX_TEXTURES];
  glm::vec4 quad_vertices[4];

  Shader* batch_shader = nullptr;
  Font* default_font = nullptr;

  usizei texture_index = 1;
  usizei indices_count = 0;

  glm::mat4 ortho;
};

static Renderer2D renderer;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void setup_buffers() {
  // Gen buffers
  glGenVertexArrays(1, &renderer.vao);
  glGenBuffers(1, &renderer.vbo);
  glGenBuffers(1, &renderer.ebo);

  // Index buffer data 
  u32 indices[MAX_INDICES];
  u32 offset = 0;
  for(u32 i = 0; i < MAX_INDICES; i += 6) {
    indices[i + 0] = 0 + offset;
    indices[i + 1] = 1 + offset;
    indices[i + 2] = 2 + offset;
    
    indices[i + 3] = 2 + offset;
    indices[i + 4] = 3 + offset;
    indices[i + 5] = 0 + offset;

    offset += 4; 
  }

  // VAO
  glBindVertexArray(renderer.vao);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * MAX_VERTICES, nullptr, GL_DYNAMIC_DRAW);

  // EBO 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_INDICES, indices, GL_STATIC_DRAW);

  // Layout 
  // Position 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, position));
  
  // Color 
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));
  
  // Texture coords 
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, texture_coords));
  
  // Texture index 
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Vertex2D), (void*)offsetof(Vertex2D, texture_index));

  glBindVertexArray(0);
}

static void load_shaders() {
  std::string batch_code = 
    "@type vertex\n"
    "#version 460 core\n"
    "\n"
    "// Layouts\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "layout (location = 2) in vec2 aTextureCoords;\n"
    "layout (location = 3) in float aTextureIndex;\n"
    "\n"
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec4 out_color;\n"
    "  vec2 tex_coords;\n"
    "  float tex_index;\n"
    "} vs_out;\n"
    "\n"
    "void main() {\n"
    "  vs_out.out_color  = aColor;\n"
    "  vs_out.tex_coords = aTextureCoords;\n"
    "  vs_out.tex_index  = aTextureIndex;\n"
    "\n"
    "  gl_Position = vec4(aPos, 1.0f);\n"
    "}\n"
    "\n"
    "@type fragment\n"
    "#version 460 core\n"
    "\n"
    "// Outputs\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec4 out_color;\n"
    "  vec2 tex_coords;\n"
    "  float tex_index;\n"
    "} fs_in;\n"
    "\n"
    "// Uniforms\n"
    "uniform sampler2D u_textures[32];\n"
    "\n"
    "void main() {\n"
    "  int index = int(fs_in.tex_index);\n"
    "  frag_color = texture(u_textures[index], fs_in.tex_coords) * fs_in.out_color;\n"
    "}";

  // Shader init
  renderer.batch_shader = shader_load("batch.glsl", batch_code);

  // Textures init
  u32 pixels = 0xffffffff;
  renderer.textures[0] = texture_load(1, 1, TEXTURE_FORMAT_RGBA, &pixels);

  shader_bind(renderer.batch_shader);
  i32 tex_slots[MAX_TEXTURES];
  for(u32 i = 0; i < 32; i++)
    tex_slots[i] = i;
  shader_upload_int_arr(renderer.batch_shader, "u_textures", tex_slots, MAX_TEXTURES);
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const bool renderer2d_create() {
  setup_buffers();

  // Load the default batch shader
  load_shaders();

  // Quad vertices init
  renderer.quad_vertices[0] = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
  renderer.quad_vertices[1] = glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
  renderer.quad_vertices[2] = glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
  renderer.quad_vertices[3] = glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);
  
  return true;
}

void renderer2d_destroy() {
  renderer.vertices.clear();

  shader_unload(renderer.batch_shader);
}

void renderer2d_flush() {
  // Render all of the unique textures
  for(u32 i = 0; i < renderer.texture_index; i++)
    texture_use(renderer.textures[i], i);

  // Initiate draw call!
  glBindVertexArray(renderer.vao); 
  glDrawElements(GL_TRIANGLES, renderer.indices_count, GL_UNSIGNED_INT, 0);

  renderer.texture_index = 1;
  renderer.indices_count = 0;
}

void renderer2d_begin() {
  shader_bind(renderer.batch_shader);
  renderer.vertices.clear();

  glm::vec2 window_size = window_get_size();
  renderer.ortho = glm::ortho(0.0f, window_size.x, window_size.y, 0.0f);
}

void renderer2d_end() {
  glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D) * renderer.vertices.size(), renderer.vertices.data());

  renderer2d_flush();
}

void renderer2d_set_default_font(Font* font) {
  renderer.default_font = font;
}

Font* renderer2d_get_default_font() {
  return renderer.default_font;
}

void render_quad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
  // Restart the renderer once the max quads is reached 
  if(renderer.indices_count >= MAX_INDICES) {
    renderer2d_end();
    renderer2d_begin();
  }
  
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
  model = glm::scale(model, glm::vec3(size.x, size.y, 0.0f));
  
  // Top-left 
  Vertex2D v1; 
  v1.position       = renderer.ortho * model * renderer.quad_vertices[0]; 
  v1.color          = color;
  v1.texture_coords = glm::vec2(0.0f, 1.0f); 
  v1.texture_index  = 0.0f;
  renderer.vertices.push_back(v1);
 
  // Top-right
  Vertex2D v2; 
  v2.position       = renderer.ortho * model * renderer.quad_vertices[1]; 
  v2.color          = color;
  v2.texture_coords = glm::vec2(1.0f, 1.0f); 
  v2.texture_index  = 0.0f;
  renderer.vertices.push_back(v2);
 
  // Bottom-right
  Vertex2D v3; 
  v3.position       = renderer.ortho * model * renderer.quad_vertices[2]; 
  v3.color          = color;
  v3.texture_coords = glm::vec2(1.0f, 0.0f); 
  v3.texture_index  = 0.0f;
  renderer.vertices.push_back(v3);
 
  // Bottom-left
  Vertex2D v4; 
  v4.position       = renderer.ortho * model * renderer.quad_vertices[3]; 
  v4.color          = color;
  v4.texture_coords = glm::vec2(0.0f, 0.0f); 
  v4.texture_index  = 0.0f;
  renderer.vertices.push_back(v4);

  renderer.indices_count += 6;
}

void render_texture(Texture* texture, const Rect& src, const Rect& dest, const glm::vec4& tint, const bool flip) {
  // Restart the renderer once the max quads is reached 
  if(renderer.indices_count >= MAX_INDICES || renderer.texture_index >= MAX_TEXTURES) {
    renderer2d_end();
    renderer2d_begin();
  }

  bool found = false;
  f32 index = 1.0f;
  
  // Check to find if the texture already exists in the array
  for(i32 i = 1; i < renderer.texture_index; i++) {
    if(texture->id == renderer.textures[i]->id) {
      found = true; 
      index = i;

      break;
    }
  }

  // Only add unique textures into the array
  // If the texture is unique, add it to the array and 
  // increament the amount of textures to render next flush
  if(!found) {
    renderer.textures[renderer.texture_index] = texture;
    index = renderer.texture_index++;
  }

  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(dest.x, dest.y, 0.0f));
  model = glm::scale(model, glm::vec3(dest.width, dest.height, 0.0f));

  // Top-left 
  Vertex2D v1; 
  v1.position       = renderer.ortho * model * renderer.quad_vertices[0]; 
  v1.color          = tint;
  v1.texture_coords = flip ? glm::vec2(src.x / src.width, src.y / src.height) :
                             glm::vec2(src.x / src.width, (src.y + src.height) / src.height);
                              
  v1.texture_index  = index;
  renderer.vertices.push_back(v1);
 
  // Top-right
  Vertex2D v2; 
  v2.position       = renderer.ortho * model * renderer.quad_vertices[1]; 
  v2.color          = tint;
  v2.texture_coords = flip ? glm::vec2((src.x + src.width) / src.width, src.y / src.height) :
                             glm::vec2((src.x + src.width) / src.width, (src.y + src.height) / src.height);
  v2.texture_index  = index;
  renderer.vertices.push_back(v2);
 
  // Bottom-right
  Vertex2D v3; 
  v3.position       = renderer.ortho * model * renderer.quad_vertices[2]; 
  v3.color          = tint;
  v3.texture_coords = flip ? glm::vec2((src.x + src.width) / src.width, (src.y + src.height) / src.height) :
                             glm::vec2((src.x + src.width) / src.width, src.y / src.height); 
  v3.texture_index  = index;
  renderer.vertices.push_back(v3);
 
  // Bottom-left
  Vertex2D v4; 
  v4.position       = renderer.ortho * model * renderer.quad_vertices[3]; 
  v4.color          = tint;
  v4.texture_coords = flip ? glm::vec2(src.x / src.width, (src.y + src.height) / src.height) :
                             glm::vec2(src.x / src.width, src.y / src.height); 
  v4.texture_index  = index;
  renderer.vertices.push_back(v4);

  renderer.indices_count += 6;
 
}

void render_texture(Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& tint) {
  Rect src = {0, 0, size.x, size.y}; 
  Rect dest = {position.x, position.y, size.x, size.y};

  render_texture(texture, src, dest, tint);
}

void render_text(const Font* font, const f32 size, const std::string& text, const glm::vec2& position, const glm::vec4& color) {
  if(!font) {
    return; 
  }  

  f32 off_x = 0.0f;
  f32 off_y = 0.0f;
  f32 scale = size / font->base_size;

  for(u32 i = 0; i < text.size(); i++) {
    i8 ch = text[i]; 
    i32 index = font_get_glyph_index(font, ch);
    Glyph glyph = font->glyphs[index];

    if(ch == '\n') {
      off_x = 0.0f;
      off_y = (font->ascent - font->descent) + font->line_gap;
      continue;
    }
    else if(ch == ' ' || ch == '\t') {
      off_x += glyph.advance_x + glyph.kern;
      continue;
    }
   
    glm::vec2 offset((off_x + glyph.x_offset) * scale, (off_y + glyph.y_offset) * scale);
    Rect src = {0.0f, 0.0f, -(glyph.width * font->glyph_padding), -(glyph.height * font->glyph_padding)};
    Rect dest = {position.x + offset.x, position.y, glyph.width * scale, glyph.height * scale};

    render_texture(glyph.texture, src, dest, color, true);

    off_x += glyph.advance_x + glyph.kern;
  }
}

void render_text(const f32 size, const std::string& text, const glm::vec2& position, const glm::vec4& color) {
  render_text(renderer.default_font, size, text, position, color);
}
/////////////////////////////////////////////////////////////////////////////////
