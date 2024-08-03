#include "renderer.h"
#include "core/window.h"
#include "defines.h"
#include "graphics/camera.h"
#include "graphics/shader.h"
#include "math/vertex.h"
#include "resources/cubemap.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/model.h"
#include "math/transform.h"
#include "resources/resource_manager.h"
#include "resources/texture.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <string>
#include <cstdio>
#include <vector>

// ShaderType
/////////////////////////////////////////////////////////////////////////////////
enum ShaderType {
  SHADER_DEFAULT, 
  SHADER_INSTANCE,
  SHADER_CUBEMAP,
  SHADERS_MAX = 3,
};
/////////////////////////////////////////////////////////////////////////////////

// Renderer
/////////////////////////////////////////////////////////////////////////////////
struct Renderer {
  Shader* shaders[SHADERS_MAX];
  Shader* current_shader = nullptr;

  u32 ubo; // Uniform buffer
  u32 instance_count = 0;
  glm::mat4* transforms = nullptr;

  Mesh* cube_mesh = nullptr;
  Mesh* skybox_mesh = nullptr;
  Material* default_material = nullptr;
};

static Renderer renderer;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
bool gl_init() {
  // Loading GLAD
  if(!gladLoadGL(glfwGetProcAddress)) {
    printf("[ERROR]: Failed to initialize GLAD\n");
    return false;
  }
  else {
    window_set_current_context();
  }

  // Setting the GL viewport size
  glm::vec2 win_size = window_get_size();
  glViewport(0, 0, win_size.x, win_size.y);
 
  // Setting options
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_MULTISAMPLE); 
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); // Need this to enable back to front rendering for 2D 

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

static void load_shaders() {
  std::string default_code = 
    "@type vertex\n" 
    "\n"
    "#version 460 core\n"
    "\n"
    "// Layouts\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTextureCoords;\n"
    "\n"
    "// Uniform block\n"
    "layout(std140, binding = 0) uniform matrices {\n"
    "  mat4 u_view_projection;\n"
    "};\n"
    "\n"
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec3 normal;\n"
    "  vec2 texture_coords;\n"
    "} vs_out;\n"
    "\n"
    "uniform mat4 u_model;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = u_view_projection * u_model * vec4(aPos, 1.0f);\n"

    "  vs_out.normal = aNormal;\n"
    "  vs_out.texture_coords = aTextureCoords;\n"
    "}\n"
    "\n"
    "@type fragment\n"
    "#version 460 core\n"
    "\n"
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec3 normal;\n"
    "  vec2 texture_coords;\n"
    "} fs_in;\n"
    "\n"
    "// Outputs\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Uniforms\n"
    "uniform vec4 u_color;\n"
    "uniform sampler2D u_diffuse, u_specular;\n"
    "\n"
    "void main() {\n"
    "  frag_color = texture(u_diffuse, fs_in.texture_coords) * u_color;\n"
    "}\n";

  std::string inst_code = 
    "@type vertex\n"
    "#version 460 core\n"
    "\n"
    "// Layouts\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "layout (location = 3) in mat4 aModel;\n"
    "// layout (location = 4) in vec4 aColor;\n"
    "\n"
    "// Uniform block\n"
    "layout(std140, binding = 0) uniform matrices {\n"
    "  mat4 u_view_projection;\n"
    "};\n"
    "\n"
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec3 normal;\n"
    "  vec2 texture_coords;\n"
    "  vec4 color;\n"
    "} vs_out;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = u_view_projection * aModel * vec4(aPos, 1.0f);\n"
    "\n"
    "  vs_out.normal = aNormal;\n"
    "  vs_out.texture_coords = aTexCoords;\n"
    "  // vs_out.color = aColor;\n"
    "}\n"
    "\n"
    "@type fragment\n"
    "#version 460 core\n"
    "\n"
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec3 normal;\n"
    "  vec2 texture_coords;\n"
    "  vec4 color;\n"
    "} fs_in;\n"
    "\n"
    "// Outputs\n"
    "out vec4 frag_color;\n"
    "\n"
    "// Uniforms\n"
    "uniform vec4 u_color;\n"
    "\n"
    "void main() {\n"
    "  frag_color = vec4(1.0f);//fs_in.color;\n"
    "}";

  std::string cubemap_shader_code = 
    "@type vertex\n"
    "#version 460 core\n"
    "\n"
    "layout (location = 0) in vec3 aPos;\n"
    "\n"
    "// Outputs\n"
    "out VS_OUT {\n"
    "  vec3 texture_coords;\n"
    "} vs_out;\n"
    "// Uniforms\n"
    "uniform mat4 u_view, u_projection;"
    "\n"
    "void main() {\n"
    " vs_out.texture_coords = aPos; // The local position of the cubemap is actually its texture coords\n"
    " vec4 pos = u_projection * u_view * vec4(aPos, 1.0f);\n"
    " gl_Position = pos.xyww;\n"
    "}\n"
    "\n"
    "@type fragment\n"
    "#version 460 core\n"
    "\n"
    "out vec4 frag_color;\n" 
    "\n"
    "// Inputs\n"
    "in VS_OUT {\n"
    "  vec3 texture_coords;\n"
    "} fs_in;\n"
    "\n"
    "// Uniforms\n"
    "uniform samplerCube u_cubemap;\n"
    "\n"
    "void main() {\n"
    "  frag_color = texture(u_cubemap, fs_in.texture_coords);\n"
    "}";

  // Shaders loading
  renderer.shaders[SHADER_DEFAULT]  = resources_add_shader("default_shader-3d", "default.glsl", default_code);
  renderer.shaders[SHADER_INSTANCE] = resources_add_shader("instance_shader-3d", "instance.glsl", inst_code);
  renderer.shaders[SHADER_CUBEMAP] = resources_add_shader("cubemap_shader", "cubemap.glsl", cubemap_shader_code);
  renderer.current_shader           = renderer.shaders[SHADER_INSTANCE];
}

static void build_skybox_mesh() {
  std::vector<Vertex3D> vertices;
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  
  vertices.push_back({glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});
  vertices.push_back({glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0.0f), glm::vec3(0.0f)});

  renderer.skybox_mesh = mesh_create(vertices, std::vector<u32>());
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const bool renderer_create() {
  if(!gl_init()) {
    return false;
  }

  // Creating uniform buffer  
  glGenBuffers(1, &renderer.ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, renderer.ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(f32) * 1024, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, renderer.ubo);

  // Load all the default shaders
  load_shaders();

  // Setting up the skybox mesh
  build_skybox_mesh();

  // Creating the instance mesh
  renderer.cube_mesh = mesh_create();

  // Load the default material
  u32 pixels = 0xffffffff;
  Texture* diffuse = texture_load(1, 1, TEXTURE_FORMAT_RGBA, &pixels); 
  renderer.default_material = resources_add_material("default_material", diffuse, nullptr, renderer.shaders[SHADER_DEFAULT]);

  // Allocate the transforms array
  renderer.transforms = new glm::mat4[MAX_MESH_INSTANCES];

  // Cube Mesh model matrix layout 
  glBindVertexArray(renderer.cube_mesh->vao); 
  glBindBuffer(GL_ARRAY_BUFFER, renderer.cube_mesh->ibo);
  
  // 1st colomn
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(glm::mat4), 0);
  glVertexAttribDivisor(3, 1); // Enabling instancing for the mesh
  
  // 2nd colomn
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(sizeof(f32) * 4));
  glVertexAttribDivisor(4, 1);
  
  // 3rd colomn
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(sizeof(f32) * 8));
  glVertexAttribDivisor(5, 1);
  
  // 4th colomn
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(sizeof(f32) * 12));
  glVertexAttribDivisor(6, 1);

  return true;
}

void renderer_destroy() {
  delete[] renderer.transforms;
 
  mesh_destroy(renderer.cube_mesh);
}

void renderer_clear(const glm::vec4& color) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(color.r, color.g, color.b, color.a);
}

void renderer_begin(const Camera* cam) {
  // Upload the view projection matrices through the uniform buffer 
  glBindBuffer(GL_UNIFORM_BUFFER, renderer.ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cam->view_projection));
}

void renderer_end() {
  shader_bind(renderer.shaders[SHADER_INSTANCE]);

  // Upload the transform matrices to the instance buffer to be renderer 
  glBindBuffer(GL_ARRAY_BUFFER, renderer.cube_mesh->ibo); 
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * renderer.instance_count, renderer.transforms);

  // Render all of the instances of the mesh
  glBindVertexArray(renderer.cube_mesh->vao);
  glDrawElementsInstanced(GL_TRIANGLES, renderer.cube_mesh->indices.size(), GL_UNSIGNED_INT, 0, renderer.instance_count);

  // Clear the instances 
  renderer.instance_count = 0; 
}

void renderer_present() {
  window_swap_buffers();
}

void render_mesh(const Transform& transform, Mesh* mesh, Material* mat) {
  if(mat) {
    material_use(mat); 
    material_set_color(mat, mat->color);
    material_set_model(mat, transform.transform);
  }

  glBindVertexArray(mesh->vao);

  if(mesh->indices.size() > 0) {
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
  }
  else if(mesh->vertices.size() > 0) {
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());
  }
  else {
    fprintf(stderr, "[WARNING]: Cannot render mesh. Can't find vertices or indices");
  }
}

void render_mesh(const Transform& transform, Mesh* mesh, const glm::vec4& color) {
  renderer.default_material->color = color;
  render_mesh(transform, mesh, renderer.default_material);
}

void render_cube(const glm::vec3& position, const glm::vec3& scale, const f32& rotation, const glm::vec4& color) {
  // Empty the instance buffer and refill it again since we reached the max
  if(renderer.instance_count >= MAX_MESH_INSTANCES) {
    renderer_end();
  }

  glm::mat4 model(1.0f);
  model = glm::translate(model, position) * 
          glm::rotate(model, rotation, glm::vec3(1.0f)) *
          glm::scale(model, scale);
  
  renderer.transforms[renderer.instance_count] = model;
  renderer.instance_count++;
}

void render_cube(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color) {
  render_cube(position, scale, 0.0f, color);
}

void render_model(const Transform& transform, Model* model) {
  if(!model) {
    return;
    // @TODO: Warn logger or assert here???? 
  }

  for(u32 i = 0; i < model->meshes.size(); i++) {
    render_mesh(transform, model->meshes[i], model->materials[model->material_ids[i]]); 
  }
}

void render_cubemap(CubeMap* cm, const Camera* cam) {
  if(!cm) {
    return;
  }

  glDepthMask(GL_FALSE);

  shader_bind(renderer.shaders[SHADER_CUBEMAP]);
  shader_upload_mat4(renderer.shaders[SHADER_CUBEMAP], "u_view", glm::mat4(glm::mat3(cam->view)));
  shader_upload_mat4(renderer.shaders[SHADER_CUBEMAP], "u_projection", cam->projection);

  glBindVertexArray(renderer.skybox_mesh->vao); 
  cubemap_use(cm);
  glDrawArrays(GL_TRIANGLES, 0, renderer.skybox_mesh->vertices.size());

  glDepthMask(GL_TRUE);
}
/////////////////////////////////////////////////////////////////////////////////
