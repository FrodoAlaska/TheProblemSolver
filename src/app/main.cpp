#include "engine/core/app_desc.h"
#include "engine/core/engine.h"
#include "app/app.h"

int main() {
  AppDesc desc = {
    .window_width = 1280, 
    .window_height = 720, 
    .window_title = "The Problem Solver", 
    
    .init_func = app_init,
    .shutdown_func = app_shutdown,
    .update_func = app_update, 
    .render_func = app_render, 

    .user_data = nullptr, 
    .has_editor = false,
  };

  engine_init(desc);
  engine_run(desc);
  engine_shutdown(desc);
}
