#include "mesh.h"
#include "defines.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <glad/gl.h>

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void setup_buffers(Mesh* mesh) {
  // Position - Normal - Texture coords
  // Back-face
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)});  
  mesh->vertices.push_back({glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)}); 
  mesh->vertices.push_back({glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)}); 
  mesh->vertices.push_back({glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)}); 

  // Front-face
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f, -0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f,  0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f,  0.5f, 0.5f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 1.0f)});

  // Left-face
  mesh->vertices.push_back({glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)});

  // Right-face
  mesh->vertices.push_back({glm::vec3(0.5f,  0.5f,  0.5f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3(0.5f,  0.5f, -0.5f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3(0.5f, -0.5f, -0.5f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3(0.5f, -0.5f,  0.5f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 0.0f)});

  // Top-face
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)});

  // Bottom-face
  mesh->vertices.push_back({glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f, 0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 1.0f)});
  mesh->vertices.push_back({glm::vec3( 0.5f, 0.5f,  0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)});
  mesh->vertices.push_back({glm::vec3(-0.5f, 0.5f,  0.5f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 0.0f)});

  mesh->indices = {
    // Back-face 
    0, 1, 2, 
    2, 3, 0, 

    // Front-face 
    4, 5, 6, 
    6, 7, 4, 

    // Left-face 
    10, 9, 8, 
    8, 11, 10, 
    
    // Right-face 
    14, 13, 12, 
    12, 15, 14,

    // Top-face 
    16, 17, 18, 
    18, 19, 16, 

    // Bottom-face 
    20, 21, 22, 
    22, 23, 20, 
  };
}

void setup_gl_buffers(Mesh* mesh) {
  // VAO
  glGenVertexArrays(1, &mesh->vao);
  glBindVertexArray(mesh->vao);

  // EBO
  glGenBuffers(1, &mesh->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * mesh->indices.size(), &mesh->indices[0], GL_STATIC_DRAW);
  
  // IBO - Instance buffer
  glGenBuffers(1, &mesh->ibo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->ibo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_MESH_INSTANCES, nullptr, GL_DYNAMIC_DRAW);

  // VBO
  glGenBuffers(1, &mesh->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * mesh->vertices.size(), &mesh->vertices[0], GL_STATIC_DRAW);
 
  // Attributes 
  // Position 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex3D), 0);
  
  // Normal 
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
  
  // Texture coords 
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texture_coords));
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Mesh* mesh_create() {
  Mesh* mesh = new Mesh{};
  setup_buffers(mesh);
  setup_gl_buffers(mesh);

  mesh->min = mesh->vertices[0].position;
  mesh->max = mesh->vertices[0].position;
  for(u32 i = 1; i < mesh->vertices.size(); i++) {
    glm::vec3 vertex = mesh->vertices[i].position;

    mesh->min = glm::min(mesh->min, vertex);
    mesh->max = glm::max(mesh->max, vertex);
  }

  return mesh;
}

Mesh* mesh_create(const std::vector<Vertex3D>& vertices, const std::vector<u32>& indices) {
  Mesh* mesh = new Mesh{};
  mesh->vertices = vertices;
  mesh->indices = indices;

  setup_gl_buffers(mesh);

  mesh->min = mesh->vertices[0].position;
  mesh->max = mesh->vertices[0].position;
  for(u32 i = 1; i < mesh->vertices.size(); i++) {
    glm::vec3 vertex = mesh->vertices[i].position;

    mesh->min = glm::min(mesh->min, vertex);
    mesh->max = glm::max(mesh->max, vertex);
  }

  return mesh;
}

void mesh_destroy(Mesh* mesh) {
  mesh->vertices.clear();
  mesh->indices.clear();

  delete mesh;
}
/////////////////////////////////////////////////////////////////////////////////
