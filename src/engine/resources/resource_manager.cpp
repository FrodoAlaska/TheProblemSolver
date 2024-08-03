#include "resource_manager.h"
#include "graphics/shader.h"
#include "resources/cubemap.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/model.h"
#include "resources/texture.h"
#include "resources/font.h"
#include "math/vertex.h"
#include "defines.h"

#include <string>
#include <unordered_map>
#include <vector>

// ResourceManager
/////////////////////////////////////////////////////////////////////////////////
struct ResourceManager {
  std::string res_path; 

  std::unordered_map<std::string, Shader*> shaders;
  std::unordered_map<std::string, Texture*> textures;
  std::unordered_map<std::string, Font*> fonts;
  std::unordered_map<std::string, Mesh*> meshes;
  std::unordered_map<std::string, Material*> materials;
  std::unordered_map<std::string, Model*> models;
  std::unordered_map<std::string, CubeMap*> cubemaps;
};

static ResourceManager s_res_man;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static Shader* find_shader(const std::string& id) {
  if(s_res_man.shaders.find(id) != s_res_man.shaders.end()) {
    return s_res_man.shaders[id];
  }

  return nullptr;
}

static Texture* find_texture(const std::string& id) {
  if(s_res_man.textures.find(id) != s_res_man.textures.end()) {
    return s_res_man.textures[id];
  }

  return nullptr;
}

static Font* find_font(const std::string& id) {
  if(s_res_man.fonts.find(id) != s_res_man.fonts.end()) {
    return s_res_man.fonts[id];
  }

  return nullptr;
}

static Mesh* find_mesh(const std::string& id) {
  if(s_res_man.meshes.find(id) != s_res_man.meshes.end()) {
    return s_res_man.meshes[id];
  }

  return nullptr;
}

static Material* find_material(const std::string& id) {
  if(s_res_man.materials.find(id) != s_res_man.materials.end()) {
    return s_res_man.materials[id];
  }

  return nullptr;
}

static Model* find_model(const std::string& id) {
  if(s_res_man.models.find(id) != s_res_man.models.end()) {
    return s_res_man.models[id];
  }

  return nullptr;
}

static CubeMap* find_cubemap(const std::string& id) {
  if(s_res_man.cubemaps.find(id) != s_res_man.cubemaps.end()) {
    return s_res_man.cubemaps[id];
  }

  return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void resources_init(const std::string res_path) {
  s_res_man.res_path = res_path;
}

void resources_shutdown() {
  for(auto& [key, value] : s_res_man.shaders) {
    shader_unload(value);
  }
  s_res_man.shaders.clear();

  for(auto& [key, value] : s_res_man.textures) {
    texture_unload(value);
  }
  s_res_man.textures.clear();
  
  for(auto& [key, value] : s_res_man.fonts) {
    font_unload(value);
  }
  s_res_man.fonts.clear();
  
  for(auto& [key, value] : s_res_man.meshes) {
    mesh_destroy(value);
  }
  s_res_man.meshes.clear();
  
  for(auto& [key, value] : s_res_man.materials) {
    material_unload(value);
  }
  s_res_man.materials.clear();
}

Shader* resources_add_shader(const std::string& id, const std::string& path) {
  std::string full_path = s_res_man.res_path + path; 
  
  s_res_man.shaders[id] = shader_load(full_path);
  return s_res_man.shaders[id];
}

Shader* resources_add_shader(const std::string& id, const std::string& shader_name, const std::string& shader_code) {
  s_res_man.shaders[id] = shader_load(shader_name, shader_code);
  return s_res_man.shaders[id];
}

Texture* resources_add_texture(const std::string& id, const std::string& path) {
  std::string full_path = s_res_man.res_path + path; 
  
  s_res_man.textures[id] = texture_load(full_path);
  return s_res_man.textures[id];
}

Texture* resources_add_texture(const std::string& id, i32 width, i32 height, TextureFormat format, void* pixels) {
  s_res_man.textures[id] = texture_load(width, height, format, pixels);
  return s_res_man.textures[id];
}

Font* resources_add_font(const std::string& path, const std::string& id) {
  std::string full_path = s_res_man.res_path + path; 
  
  s_res_man.fonts[id] = font_load(full_path, 256.0f);
  return s_res_man.fonts[id];
}

Mesh* resources_add_mesh(const std::string& id) {
  s_res_man.meshes[id] = mesh_create();
  return s_res_man.meshes[id];
}

Mesh* resources_add_mesh(const std::string& id, const std::vector<Vertex3D>& vertices, const std::vector<u32>& indices) {
  s_res_man.meshes[id] = mesh_create(vertices, indices);
  return s_res_man.meshes[id];
}

Material* resources_add_material(const std::string& id, 
                                 Texture* diffuse, 
                                 Texture* specular, 
                                 Shader* shader) {
  s_res_man.materials[id] = material_load(diffuse, specular, shader);
  return s_res_man.materials[id];
}

Model* resources_add_model(const std::string& id, const std::string& path) {
  std::string full_path = s_res_man.res_path + path; 

  s_res_man.models[id] = model_load(path);
  return s_res_man.models[id];
}

CubeMap* resources_add_cubemap(const std::string& id, const std::string& path) {
  std::string full_path = s_res_man.res_path + path; 

  s_res_man.cubemaps[id] = cubemap_load(path);
  return s_res_man.cubemaps[id];
}

Shader* resources_get_shader(const std::string& id) {
  return find_shader(id);
}

Texture* resources_get_texture(const std::string& id) {
  return find_texture(id);
}

Font* resources_get_font(const std::string& id) {
  return find_font(id);
}

Mesh* resources_get_mesh(const std::string& id) {
  return find_mesh(id);
}

Material* resources_get_material(const std::string& id) {
  return find_material(id);
}

Model* resources_get_model(const std::string& id) {
  return find_model(id);
}

CubeMap* resources_get_cubemap(const std::string& id) {
  return find_cubemap(id);
}

bool resources_remove_shader(const std::string& id) {
  Shader* shader = find_shader(id);
  if(shader) {
    // Make sure to unload the resource before removing it
    shader_unload(shader);
    s_res_man.shaders.erase(id);

    return true;
  }

  return false;
}

bool resources_remove_texture(const std::string& id) {
  Texture* texture = find_texture(id);
  if(texture) {
    texture_unload(texture);
    s_res_man.textures.erase(id);
    
    return true;
  }

  return false;
}

bool resources_remove_font(const std::string& id) {
  Font* font = find_font(id);
  if(font) {
    font_unload(font);
    s_res_man.fonts.erase(id);
    
    return true;
  }

  return false;
}

bool resources_remove_mesh(const std::string& id) {
  Mesh* mesh = find_mesh(id);
  if(mesh) {
    mesh_destroy(mesh);
    s_res_man.fonts.erase(id);
    
    return true;
  }

  return false;
}

bool resources_remove_material(const std::string& id) {
  Material* material = find_material(id);
  if(material) {
    material_unload(material);
    s_res_man.materials.erase(id);
    
    return true;
  }

  return false;
}

bool resources_remove_model(const std::string& id) {
  Model* model = find_model(id);
  if(model) {
    model_unload(model);
    s_res_man.models.erase(id);
    
    return true;
  }

  return false;
}

bool resources_remove_cubemap(const std::string& id) {
  CubeMap* cm = find_cubemap(id);
  if(cm) {
    cubemap_unload(cm);
    s_res_man.cubemaps.erase(id);
    
    return true;
  }

  return false;
}
/////////////////////////////////////////////////////////////////////////////////
