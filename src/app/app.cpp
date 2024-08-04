#include "app.h"
#include "core/input.h"
#include "core/window.h"
#include "engine/graphics/renderer.h"
#include "engine/graphics/renderer2d.h"
#include "resources/resource_manager.h"
#include "states/state_manager.h"

#include <glm/glm.hpp>

#include <string>

// App
/////////////////////////////////////////////////////////////////////////////////
struct App {
  StateManger state_manager;
};

static App s_app;
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
bool app_init(void* user_data) {
  // Adding resources/assets 
  Font* font = resources_add_font("fonts/Kleader.ttf", "default_font");
  resources_add_texture("platform_texture", "textures/stone_texture.jpg");
  resources_add_texture("target_texture", "textures/container.jpg");

  // Setting the default font 
  renderer2d_set_default_font(font);

  // State manager init
  state_manager_init(&s_app.state_manager, font);

  window_set_exit_key(KEY_F1);
  
  return true;
}

void app_shutdown(void* user_data) {
  state_manager_shutdown(&s_app.state_manager);
}

void app_update(void* user_data) {
  state_manager_update(&s_app.state_manager);
}

void app_render(void* user_data) {
  renderer_clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  
  state_manager_render(&s_app.state_manager);
  state_manager_render_ui(&s_app.state_manager);
 
  renderer_present();
}
/////////////////////////////////////////////////////////////////////////////////
