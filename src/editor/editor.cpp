#include "editor.h"
#include "engine/core/window.h"
#include "engine/graphics/camera.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>
#include <glm/vec3.hpp>

#include <cstdio>

// Editor 
/////////////////////////////////////////////////////////////////////////////////
struct Editor {
  Camera editor_cam;
  bool is_active;
};

static Editor s_editor;
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void editor_init(const glm::vec3& camera_pos, const glm::vec3& camera_target, const bool initial_active) {
  // Set up ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO io = ImGui::GetIO(); (void)io;

  // Setting context flags
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Dark mode WOOOOOOOAH! 
  ImGui::StyleColorsDark();

  // Setting up the glfw backend
  if(!ImGui_ImplGlfw_InitForOpenGL(window_get_handle(), true)) {
    fprintf(stderr, "[ERROR]: Failed to initialize GLFW for ImGui");
  }
  
  // Setting up the opengl backend
  if(!ImGui_ImplOpenGL3_Init("#version 460 core")) {
    fprintf(stderr, "[ERROR]: Failed to initialize OpenGL for ImGui");
  }

  // Create editor camera 
  s_editor.editor_cam = camera_create(camera_pos, camera_target); 
  s_editor.is_active = initial_active;
}

void editor_shutdown() {
  if(!s_editor.is_active) {
    return;
  }

  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();
}

void editor_begin() {
  if(!s_editor.is_active) {
    return;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();
}

void editor_end() {
  if(!s_editor.is_active) {
    return;
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void editor_set_active(bool active) {
  s_editor.is_active = active;
}

const bool editor_active() {
  return s_editor.is_active;
}

const Camera& editor_get_camera() {
  return s_editor.editor_cam;
}
/////////////////////////////////////////////////////////////////////////////////
