#include "game_state.h"
#include "core/input.h"
#include "tasks_menu.h"
#include "count_timer.h"
#include "target_spawner.h"
#include "entities/object.h"
#include "hit_manager.h"
#include "engine/core/window.h"

#include <vector>

// Private functions 
/////////////////////////////////////////////////////////////////////////////////
static void check_collisions(GameState* game) {
  for(u32 i = 0; i < game->targets.size(); i++) {
    Object* object = game->targets[i];

    Ray ray = {
      .position = game->camera.position, 
      .direction = game->camera.direction,
    };

    RayIntersection intersect = ray_intersect(&ray, &object->body->transform, &object->collider);
    if(!intersect.has_intersected) {
      continue;
    }
     
    // There's a HIT!!
    game->score += hit_manager_calc_hit_score(&game->hit_manager, object->body->transform.position, intersect.intersection_point);
    target_spawner_hit(&game->target_spawner, object, ray);

    // Critical hit?? 
    if(hit_manager_is_critical(&game->hit_manager, object->body->transform.position, intersect.intersection_point)) {
      game->hit_manager.in_combo = true;
      game->hit_manager.total_combo++;
    }
    else { // Just a normal hit. Not a critical hit
      hit_manager_end_combo(&game->hit_manager);
    }

    break; // The ray is more likely (and more desirably) to hit one object
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
void game_state_init(GameState* game) {
  // Camera init 
  game->camera = camera_create(glm::vec3(35.0f, 0.0f, 10.0f), glm::vec3(-3.0f, 0.0f, 0.0f));

  // Create the platform 
  game->objects.push_back(object_create(glm::vec3(1.5f, 50.0f, 22.0f), PhysicsBodyDesc{
    .position = glm::vec3(50.0f, -27.0f, 10.0f), 
    .type = PHYSICS_BODY_STATIC, 
  }, "platform_texture"));

  // Systems and managers init
  target_spawner_init(&game->target_spawner, game->targets, 20.0f);
  count_timer_create(&game->timer, 10, 0, true);
  hit_manager_init(&game->hit_manager);
  task_menu_init(&game->task_menu);
}

void game_state_update(GameState* game) {
  // Camera update
  camera_update(&game->camera);
  camera_move(&game->camera);
 
  // Systems and managers update
  target_spawner_update(&game->target_spawner);
  count_timer_update(&game->timer);
  hit_manager_update(&game->hit_manager);

  // Enabling/disabling the task menu
  if(input_key_pressed(KEY_T)) {
    game->task_menu.is_active = !game->task_menu.is_active;
  } 

  // Player shooting the gun
  if(!input_key_pressed(KEY_SPACE)) {
    return;
  }

  // Checking for ray collisions against the targets.
  check_collisions(game);
    
  // Increase timer every three combos 
  if((game->hit_manager.total_combo % 3) == 0 && game->score != 0) {
    count_timer_increase(&game->timer, 10);
  } 

  // The player shot no objects, therefore end the combo 
  if(game->hit_manager.total_combo == 0) {
    hit_manager_end_combo(&game->hit_manager);
  }
  
  // Checking if the current balance is sufficient enough 
  // for completing a task
  task_menu_update(&game->task_menu, &game->score);
}

void game_state_render(GameState* game) {
  // Render the objects
  for(auto& obj : game->objects) {
    object_render(obj);
  }

  // Render the targets 
  for(auto& target : game->targets) {
    object_render(target);
  }
}

void game_state_render_ui(GameState* game) {
  // Rendering the score 
  render_text(25.0f, std::to_string(game->score) + "$", glm::vec2(10.0f, 15.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

  // Rendering the timer
  count_timer_render(&game->timer);

  // Rendering radical 
  render_quad(window_get_size() / 2.0f - 8.0f, glm::vec2(16.0f), glm::vec4(1.0f));
 
  // Rendering the tasks
  task_menu_render(&game->task_menu);
}

void game_state_reset(GameState* game) {
  count_timer_reset(&game->timer);

  input_cursor_show(false);
  input_set_mouse_offset(glm::vec2(0.0f));

  game->camera.yaw = -90.0f;
  game->camera.pitch = 0.0f;
}
/////////////////////////////////////////////////////////////////////////////////
