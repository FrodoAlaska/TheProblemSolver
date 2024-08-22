#include "shader.h"
#include "defines.h"
#include "utils/utils_file.h"

#include <cstring>
#include <string>
#include <cstdio>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glad/gl.h>

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void read_shader(const std::string& shader_str, Shader* shader) {
  usizei vert_iden_pos = shader_str.find("@type vertex"); 
  usizei frag_iden_pos = shader_str.find("@type fragment");

  if(vert_iden_pos == shader_str.npos) {
    printf("[ERROR]: No vertex identifier found in shader \'%s\'\n", shader->name.c_str());
    shader->vert_src = "";
  }
  if(frag_iden_pos == shader_str.npos) {
    printf("[ERROR]: No fragment identifier found in shader \'%s\'\n", shader->name.c_str());
    shader->frag_src = "";
  }

  usizei vert_iden_size = strlen("@type vertex");
  usizei frag_iden_size = strlen("@type fragment");
  shader->vert_src = shader_str.substr(vert_iden_pos + vert_iden_size, frag_iden_pos - vert_iden_size);
  shader->frag_src = shader_str.substr(frag_iden_pos + frag_iden_size);
}

static void check_compile_error(const u32 shader_id) {
  int success;
  char log_info[512];

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); 

  if(!success) {
    glGetShaderInfoLog(shader_id, 512, nullptr, log_info);
    printf("[SHADER-ERROR]: %s\n", log_info);
  }
}

static void check_linker_error(const Shader* shader) {
  int success;
  char log_info[512];

  glGetProgramiv(shader->id, GL_COMPILE_STATUS, &success); 

  if(!success) {
    glGetProgramInfoLog(shader->id, 512, nullptr, log_info);
    printf("[SHADER-ERROR]: %s\n", log_info);
  }
}

static unsigned int get_uniform_location(const Shader* shader, const std::string& name) {
  u32 uni_loc = 0;

  uni_loc = glGetUniformLocation(shader->id, name.c_str());
  if(uni_loc == -1) {
    printf("[SHADER-ERROR]: Could not find variable \'%s\' in shader \'%s\'\n", name.c_str(), shader->name.c_str());
  }

  return uni_loc;
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Shader* shader_load(const std::string& shader_name, const std::string& shader_code) {
  Shader* shader = new Shader{};
  shader->name = shader_name;

  // Read the whole shader code and turn it into two seperate strings for 
  // OpenGL to compile and link
  read_shader(shader_code, shader);
  
  // Vertex shader
  const char* vert_src = shader->vert_src.c_str(); 
  shader->vert_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader->vert_id, 1, &vert_src, 0);
  glCompileShader(shader->vert_id);
  check_compile_error(shader->vert_id);
  
  // Fragment shader
  const char* frag_src = shader->frag_src.c_str();
  shader->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader->frag_id, 1, &frag_src, 0);
  glCompileShader(shader->frag_id);
  check_compile_error(shader->frag_id);

  // Linking 
  shader->id = glCreateProgram();
  glAttachShader(shader->id, shader->vert_id);
  glAttachShader(shader->id, shader->frag_id);
  glLinkProgram(shader->id);
  check_linker_error(shader);

  // Detaching 
  glDetachShader(shader->id, shader->vert_id);
  glDetachShader(shader->id, shader->frag_id);
  glDeleteShader(shader->vert_id);
  glDeleteShader(shader->frag_id);

  return shader;
}

Shader* shader_load(const std::string& path) {
  std::string contents; 
  file_read_string(path, &contents);

  return shader_load(path.substr(path.find_last_of('/') + 1), contents);
}

void shader_unload(Shader* shader) {
  if(!shader) {
    return;
  }

  glDeleteProgram(shader->id);
  delete shader;

  shader = nullptr;
}

void shader_bind(Shader* shader) {
  glUseProgram(shader->id);
}

void shader_upload_int(Shader* shader, const std::string& name, const i32 value) {
  glUniform1i(get_uniform_location(shader, name), value);
}

void shader_upload_int_index(Shader* shader, const std::string& name, const u32 index, const i32 value) {
  std::string index_name = std::string(name + "[" + std::to_string(index) + "]");
  glUniform1i(get_uniform_location(shader, index_name), value);
}

void shader_upload_int_arr(Shader* shader, const std::string& name, const i32* values, const usizei size) {
  glUniform1iv(get_uniform_location(shader, name), size, values); 
}

void shader_upload_float(Shader* shader, const std::string& name, const f32 value) {
  glUniform1f(get_uniform_location(shader, name), value);
}

void shader_upload_mat4(Shader* shader, const std::string& name, const glm::mat4& value) {
  glUniformMatrix4fv(get_uniform_location(shader, name), 1, GL_FALSE, glm::value_ptr(value));
}

void shader_upload_vec4(Shader* shader, const std::string& name, const glm::vec4& value) {
  glUniform4f(get_uniform_location(shader, name), value.x, value.y, value.z, value.w);
}

void shader_upload_vec3(Shader* shader, const std::string& name, const glm::vec3& value) {
  glUniform3f(get_uniform_location(shader, name), value.x, value.y, value.z);
}

void shader_upload_mat4_index(Shader* shader, const std::string& name, const i32 index, const glm::mat4& value) {
  std::string index_name = std::string(name + "[" + std::to_string(index) + "]");
  glUniformMatrix4fv(get_uniform_location(shader, index_name), 1, GL_FALSE, glm::value_ptr(value));
}

void shader_upload_vec4_index(Shader* shader, const std::string& name, const i32 index, const glm::vec4& value) {
  std::string index_name = std::string(name + "[" + std::to_string(index) + "]");
  glUniform4f(get_uniform_location(shader, index_name), value.x, value.y, value.z, value.w);
}

void shader_upload_vec3_index(Shader* shader, const std::string& name, const i32 index, const glm::vec3& value) {
  std::string index_name = std::string(name + "[" + std::to_string(index) + "]");
  glUniform3f(get_uniform_location(shader, index_name), value.x, value.y, value.z);
}
/////////////////////////////////////////////////////////////////////////////////
