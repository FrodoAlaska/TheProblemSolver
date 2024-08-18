#include "game_state.h"
#include "audio/audio_system.h"
#include "audio/music_type.h"
#include "audio/sound_type.h"
#include "core/clock.h"
#include "core/input.h"
#include "graphics/renderer2d.h"
#include "math/rand.h"
#include "math/transform.h"
#include "particles.h"
#include "physics/collider.h"
#include "physics/physics_body.h"
#include "physics/ray.h"
#include "tasks_menu.h"
#include "count_timer.h"
#include "target_spawner.h"
#include "entities/object.h"
#include "hit_manager.h"
#include "engine/core/window.h"
#include "ui/ui_anchor.h"
#include "ui/ui_text.h"
#include "engine/physics/physics_world.h"
#include "engine/resources/resource_manager.h"

#include <vector>

static bool is_debug = false;

// Private functions 
/////////////////////////////////////////////////////////////////////////////////
static void check_collisions(GameState* game) {
  // Gun VS. Bottles
  for(u32 i = 0; i < game->targets.size(); i++) {
    Target* target = game->targets[i];
    if(!target->is_active) {
      continue;
    }

    Ray ray = {
      .position = game->camera.position, 
      .direction = game->camera.direction,
    };

    RayIntersection intersect = ray_intersect(&ray, &target->body->transform, &target->collider);
    if(!intersect.has_intersected) {
      continue;
    }
    
    // Bottle sound
    audio_system_play(SOUND_BOTTLE_BREAK, random_f32(0.8f, 1.0f));
     
    // There's a HIT!!
    game->score += hit_manager_calc_hit_score(&game->hit_manager, target->body->transform.position, intersect.intersection_point);
    target_spawner_hit(&game->target_spawner, target, ray);

    // Emit some particles 
    particles_emit(intersect.intersection_point);

    // Critical hit?? 
    if(hit_manager_is_critical(&game->hit_manager, target->body->transform.position, intersect.intersection_point)) {
      game->hit_manager.in_combo = true;
      game->hit_manager.total_combo++;

      audio_system_play(SOUND_BOTTLE_CRITICAL, 1.0f);
    }
    else { // Just a normal hit. Not a critical hit
      hit_manager_end_combo(&game->hit_manager);
    }

    break; // The ray is more likely (and more desirably) to hit one object
  }

  // Gun VS. Box 
  // (Index starts at 1 to skip the ground)
  for(u32 i = 1; i < game->objects.size(); i++) {
    Object* obj = game->objects[i];

    Ray ray = {
      .position = game->camera.position, 
      .direction = game->camera.direction,
    };

    RayIntersection intersect = ray_intersect(&ray, &obj->body->transform, &obj->collider);
    if(!intersect.has_intersected) {
      continue;
    }

    audio_system_play(SOUND_BOX_HIT, 1.0f);
  }
}

static void pause_screen_render(GameState* game) {
  ui_text_render(&game->pause_text);
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
void game_state_init(GameState* game) {
  // Textures init 
  game->crosshair = resources_get_texture("crosshair");

  // Camera init 
  game->camera = camera_create(glm::vec3(30.0f, 0.0f, 7.58f), glm::vec3(-3.0f, 0.0f, 0.0f));
  
  // Ground 
  game->objects.push_back(object_create(glm::vec3(50.0f, -3.3f, 3.0f), glm::vec3(50.0f, 0.1f, 50.0f), PHYSICS_BODY_STATIC, "ground_texture"));

  // Create the platforms 
  for(u32 i = 0; i < 6; i++) {
    game->objects.push_back(object_create(glm::vec3(50.0f, -1.75f, i * 3.0f), glm::vec3(2.0f), PHYSICS_BODY_STATIC, "platform_texture"));
  }

  // Particles init 
  particles_init();

  // Systems and managers init
  target_spawner_init(&game->target_spawner, game->targets);
  count_timer_create(&game->timer, 0, 10, true);
  hit_manager_init(&game->hit_manager);
  task_menu_init(&game->task_menu);

  // Pause screen init 
  game->is_paused = false; 
  ui_text_create(&game->pause_text, renderer2d_get_default_font(), "PAUSED", 30.0f, UI_ANCHOR_CENTER, glm::vec4(0, 0.8f, 0, 1), glm::vec2(0.0f, -20.0f));
}

void game_state_update(GameState* game) {
  // Pausing/unpausing the game
  if(input_key_pressed(KEY_ESCAPE)) {
    game->is_paused = !game->is_paused;
  }

  // Don't update anything when paused
  if(game->is_paused) {
    return;
  }

  if(input_key_pressed(KEY_T)) {
    game->task_menu.is_active = !game->task_menu.is_active;
  }

  if(input_key_pressed(KEY_Q)) {
    is_debug = !is_debug;
  }

  // Physics update
  physics_world_update(gclock_delta_time());

  // Camera update
  camera_update(&game->camera);
  camera_move(&game->camera);
 
  // Systems and managers update
  target_spawner_update(&game->target_spawner);
  count_timer_update(&game->timer);
  hit_manager_update(&game->hit_manager);

  // Checking if the current balance is sufficient enough 
  // for completing a task
  task_menu_update(&game->task_menu, &game->score);

  // Particles update
  particles_update();

  // Player shooting the gun
  if(!input_button_pressed(MOUSE_BUTTON_LEFT)) {
    return;
  }

  // Gun shot sound
  audio_system_play(SOUND_GUN_SHOT, random_f32(0.7f, 1.0f));

  // Checking for ray collisions against the targets.
  check_collisions(game);
    
  // Increase timer every three combos 
  if((game->hit_manager.total_combo % 3) == 0 && game->score != 0 && game->hit_manager.total_combo != 0) {
    count_timer_increase(&game->timer, 10);
  } 

  // The player shot no objects, therefore end the combo 
  if(game->hit_manager.total_combo == 0) {
    hit_manager_end_combo(&game->hit_manager);
  }
}

void game_state_render(GameState* game) {
  // Render the particles
  particles_render();

  // Render the objects
  for(auto& obj : game->objects) {
    object_render(obj);
  }

  // Render the targets 
  for(auto& target : game->targets) {
    target_render(target);
  }
}

void game_state_render_ui(GameState* game) {
  // Rendering the score 
  render_text(25.0f, std::to_string(game->score) + "$", glm::vec2(10.0f, 15.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

  count_timer_render(&game->timer);

  // Rendering crosshair 
  // render_texture(game->crosshair, window_get_size() / 2.0f - 4.0f, glm::vec2(96.0f), glm::vec4(1.0f));
  render_quad(window_get_size() / 2.0f - 2.0f, glm::vec2(4.0f), glm::vec4(0, 0, 0, 1));
    
  task_menu_render(&game->task_menu);

  if(game->is_paused) {
    pause_screen_render(game);
  }
}

void game_state_reset(GameState* game) {
  game->is_paused = false;

  // Spawner reset 
  target_spawner_reset(&game->target_spawner);

  // Timer reset
  count_timer_reset(&game->timer);

  // Combo reset 
  game->hit_manager.total_combo = 0; 
  game->hit_manager.combo_timer = 0.0f;

  // Tasks reset 
  task_menu_reset(&game->task_menu);

  // Input reset
  input_cursor_show(false);
  input_set_mouse_offset(glm::vec2(0.0f));

  // Camera reset
  game->camera.yaw = -90.0f;
  game->camera.pitch = 0.0f;
  
  // Audio reset
  audio_system_stop(MUSIC_MENU);
  audio_system_play(MUSIC_BACKGROUND, 1.0f);

  // Particles reset 
  particles_reset();
}
/////////////////////////////////////////////////////////////////////////////////
