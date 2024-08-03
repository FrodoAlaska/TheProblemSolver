#pragma once

#include "defines.h"

#include <string>

// Callbacks 
/////////////////////////////////////////////////////////////////////////////////
typedef bool (*app_init_func)(void* user_data);
typedef void (*app_shutdown_func)(void* user_data);
typedef void (*app_update_func)(void* user_data);
typedef void (*app_render_func)(void* user_data);
/////////////////////////////////////////////////////////////////////////////////

// AppDesc
/////////////////////////////////////////////////////////////////////////////////
struct AppDesc {
  i32 window_width, window_height;
  std::string window_title;

  app_init_func init_func;
  app_shutdown_func shutdown_func;
  app_update_func update_func;
  app_render_func render_func;

  void* user_data = nullptr;
  bool has_editor = false;
};
/////////////////////////////////////////////////////////////////////////////////
