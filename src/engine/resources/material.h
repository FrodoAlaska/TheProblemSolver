#pragma once

#include "graphics/shader.h"
#include "resources/texture.h"

#include <glm/glm.hpp>

// MaterialMapType
/////////////////////////////////////////////////////////////////////////////////
enum MaterialMapType {
  MATERIAL_MAP_DIFFUSE,
  MATERIAL_MAP_SPECULAR,
};
/////////////////////////////////////////////////////////////////////////////////

// Material
/////////////////////////////////////////////////////////////////////////////////
struct Material {
  Texture* diffuse_map;
  Texture* specular_map;
  
  Shader* shader;
  glm::vec4 color;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
Material* material_load(Texture* diffuse, Texture* specular, Shader* shader);
void material_unload(Material* mat);

// Bind the shader and render textures whithin the maps
void material_use(Material* mat);

// NOTE: The 'material_set_*' functions below do NOT bind the shader before setting the 
// uniforms. You must use the 'material_use' function beforehand.
// Set the color uniform in the shader
void material_set_color(Material* mat, const glm::vec4& color);

// Set the model matrix in the shader
void material_set_model(Material* mat, const glm::mat4& model);
/////////////////////////////////////////////////////////////////////////////////
