#include "model.h"
#include "defines.h"
#include "graphics/renderer.h"
#include "math/vertex.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "resources/resource_manager.h"
#include "resources/texture.h"

#include <tinyobjloader/tiny_obj_loader.h>
#include <glm/glm.hpp>

#include <cstdio>
#include <vector>
#include <string>

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static const std::vector<Material*> load_materials(const std::vector<tinyobj::material_t>& materials, const tinyobj::ObjReaderConfig& cfg) {
  std::vector<Material*> mats; 

  // Loop over the materials 
  for(u32 i = 0; i < materials.size(); i++) {
    Texture* diffuse = nullptr;
    Texture* specular = nullptr; 
    
    // Loading the diffuse texture
    std::string diff = materials[i].diffuse_texname;
    if(!diff.empty()) {
      diffuse = texture_load(cfg.mtl_search_path + diff);
    }
    else {
      glm::vec4 col(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2], materials[i].dissolve);  
      diffuse = texture_load(1, 1, TEXTURE_FORMAT_RGBA, &col);
    }
   
    // Loading the specular texture
    std::string spec = materials[i].specular_texname;
    if(!spec.empty()) {
      specular = texture_load(cfg.mtl_search_path + spec);
    }
    else {
      glm::vec4 col(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2], materials[i].dissolve);  
      specular = texture_load(1, 1, TEXTURE_FORMAT_RGBA, &col);
    }

    // New material added!
    mats.push_back(material_load(diffuse, specular, resources_get_shader("default_shader-3d")));
  }

  return mats;
}

static void load_model_data(Model* model, const tinyobj::ObjReader& reader, const tinyobj::ObjReaderConfig cfg) {
  auto& shape = reader.GetShapes();
  auto& attrib = reader.GetAttrib();
  const auto& materials = reader.GetMaterials();

  model->materials = load_materials(materials, cfg); 
  std::vector<Vertex3D> vertices;
  std::vector<u32> indices;

  // Loop over the shapes
  for(u32 i = 0; i < shape.size(); i++) {
    usizei index_offset = 0;

    // Loop over the faces
    for(u32 j = 0; j < shape[i].mesh.num_face_vertices.size(); j++) {
      usizei face_vertex = shape[i].mesh.num_face_vertices[j];
     
      // Loop over the vertices in the face 
      for(u32 x = 0; x < face_vertex; x++) {
        Vertex3D vert;

        // Index 
        tinyobj::index_t index = shape[i].mesh.indices[index_offset + x]; 

        // Vertex position
        tinyobj::real_t vert_x = attrib.vertices[3 * usizei(index.vertex_index) + 0]; 
        tinyobj::real_t vert_y = attrib.vertices[3 * usizei(index.vertex_index) + 1]; 
        tinyobj::real_t vert_z = attrib.vertices[3 * usizei(index.vertex_index) + 2];
        vert.position = glm::vec3(vert_x, vert_y, vert_z);

        // Vertex normal (if it exists)
        if(index.normal_index >= 0) {
          tinyobj::real_t normal_x = attrib.normals[3 * usizei(index.normal_index) + 0];
          tinyobj::real_t normal_y = attrib.normals[3 * usizei(index.normal_index) + 1];
          tinyobj::real_t normal_z = attrib.normals[3 * usizei(index.normal_index) + 2];
          vert.normal = glm::vec3(vert_x, vert_y, vert_z);
        }
        else {
          vert.normal = glm::vec3(0.0f);
        }

        // Vertex texture coords (if it exists)
        if(index.texcoord_index >= 0) {
          tinyobj::real_t u = attrib.texcoords[2 * usizei(index.texcoord_index) + 0];
          tinyobj::real_t v = attrib.texcoords[2 * usizei(index.texcoord_index) + 1];
          vert.texture_coords = glm::vec2(u, v);
        } 
        else {
          vert.texture_coords = glm::vec2(0.0f);
        }

        // New vertex
        vertices.push_back(vert);
      }

      // Advance to the next vertices
      index_offset += face_vertex;
    }
  
    /*
    * NOTE: This ID will be used when rendering the model. Each mesh needs a specific 
    * map/texture from the material. The id is used to index into the 'model->materials'
    * memeber when rendering to know the exact texture needed for this mesh to render.
    *
    * However, the material ids are stored for each face. Not each mesh. So, to extract 
    * only the relvant id per mesh needed, we need to '%' with the total length of the faces.
    */ 
    i32 id = shape[i].mesh.material_ids[i % shape[i].mesh.num_face_vertices.size()];
    
    model->material_ids.push_back(id);
    model->meshes.push_back(mesh_create(vertices, indices));
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Model* model_load(const std::string& path) {
  Model* model = new Model{};

  tinyobj::ObjReaderConfig reader_cfg;
  reader_cfg.mtl_search_path = path.substr(0, path.find_last_of('/')) + '/';
  reader_cfg.triangulate = true;
  reader_cfg.vertex_color = true;

  tinyobj::ObjReader reader;
  if(!reader.ParseFromFile(path)) {
    if(!reader.Error().empty()) {
      fprintf(stderr, "[OBJ-ERROR]: %s", reader.Error().c_str());
    }

    return model;
  }

  load_model_data(model, reader, reader_cfg);
  return model;
}

void model_unload(Model* model) {
  for(auto& mesh : model->meshes) {
    mesh_destroy(mesh);
  }
  model->materials.clear();

  for(auto& mat : model->materials) {
    material_unload(mat);
  }
  model->materials.clear();

  delete model;
}
/////////////////////////////////////////////////////////////////////////////////
