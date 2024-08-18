#include "app.h"
#include "audio/audio_system.h"
#include "audio/music_type.h"
#include "audio/sound_type.h"
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
  // Adding font 
  Font* font = resources_add_font("fonts/Kleader.ttf", "default_font");

  // Adding textures 
  resources_add_texture("ground_texture", "textures/sand_texture.png");
  resources_add_texture("platform_texture", "textures/container.png");
  resources_add_texture("crosshair", "textures/crosshair006.png");
  resources_add_cubemap("desert_cubemap", "cubemaps/desert_cubemap/");

  // Adding music 
  audio_system_add_music(MUSIC_MENU, "assets/music/menu_theme.mp3");
  audio_system_add_music(MUSIC_BACKGROUND, "assets/music/background_music.mp3");
  audio_system_add_music(MUSIC_WIN, "assets/music/victory_song.mp3");
  
  // Adding SFX
  audio_system_add_sound(SOUND_GUN_SHOT, "assets/sfx/gun_shot.mp3");
  audio_system_add_sound(SOUND_BOTTLE_BREAK, "assets/sfx/bottle_break.mp3");
  audio_system_add_sound(SOUND_BOTTLE_SPAWN, "assets/sfx/bottle_spawn.mp3");
  audio_system_add_sound(SOUND_BOTTLE_CRITICAL, "assets/sfx/bottle_critical.mp3");
  audio_system_add_sound(SOUND_BOX_HIT, "assets/sfx/box_hit.wav");
  audio_system_add_sound(SOUND_TASK_COMPLETE, "assets/sfx/task_complete.wav");
  audio_system_add_sound(SOUND_UI_CLICK, "assets/sfx/ui_click.mp3");

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
