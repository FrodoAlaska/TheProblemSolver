#pragma once

#include "defines.h"
#include "core/input.h"

#include <glm/vec2.hpp>

struct GLFWwindow;

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const bool window_create(const i32 width, const i32 height, const char* title); 
void window_destroy();
void window_poll_events();
void window_swap_buffers();

const bool window_should_close();
const glm::vec2 window_get_size();
const f64 window_get_time();
const f32 window_get_aspect_ratio();
GLFWwindow* window_get_handle(); 
const KeyCode window_get_exit_key();

void window_set_current_context();
void window_set_vsync(const bool vsync);
void window_set_close(const bool close);
void window_set_exit_key(KeyCode key);
/////////////////////////////////////////////////////////////////////////////////
