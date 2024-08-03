#include "app.h"
#include "core/input.h"
#include "core/window.h"
#include "engine/graphics/renderer.h"
#include "engine/graphics/renderer2d.h"
#include "engine/graphics/camera.h"
#include "editor/editor.h"
#include "math/transform.h"
#include "physics/physics_body.h"
#include "physics/ray.h"
#include "resources/resource_manager.h"

#include "tasks_menu.h"
#include "count_timer.h"
#include "target_spawner.h"
#include "entities/object.h"
#include "hit_manager.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

// App
/////////////////////////////////////////////////////////////////////////////////
struct App {
  Camera camera;
  Camera* current_cam;

  std::vector<Object*> objects;
  std::vector<Object*> targets;

  TargetSpawner target_spawner;
  CountTimer timer;
  HitManager hit_manager;
  TaskMenu task_menu;

  u32 score = 0;
};

static App s_app;
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
bool app_init(void* user_data) {
  // Adding resources/assets 
  resources_add_font("fonts/Kleader.ttf", "default_font");
  resources_add_texture("platform_texture", "textures/stone_texture.jpg");
  resources_add_texture("target_texture", "textures/container.jpg");

  // Setting the default font 
  renderer2d_set_default_font(resources_get_font("default_font"));

  // Disable the cursor
  input_cursor_show(false);

  // Camera init 
  s_app.camera = camera_create(glm::vec3(35.0f, 0.0f, 10.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
  
  // Editor init
  editor_init(glm::vec3(10.0f, 5.0f, 10.0f), glm::vec3(0.0f, 1.0f, -3.0f), false);

  // This can be switched between the game camera and editor camera
  s_app.current_cam = &s_app.camera;

  // Create the platform 
  s_app.objects.push_back(object_create(glm::vec3(1.5f, 50.0f, 22.0f), PhysicsBodyDesc{
    .position = glm::vec3(50.0f, -27.0f, 10.0f), 
    .type = PHYSICS_BODY_STATIC, 
  }, "platform_texture"));

  target_spawner_init(&s_app.target_spawner, s_app.targets, 20.0f);
  count_timer_create(&s_app.timer, 0, 1, 0, true);
  hit_manager_init(&s_app.hit_manager);
  task_menu_init(&s_app.task_menu);

  return true;
}

void app_shutdown(void* user_data) {
  editor_shutdown();
}

void app_update(void* user_data) {
  camera_update(s_app.current_cam);
  camera_move(s_app.current_cam);

  target_spawner_update(&s_app.target_spawner);
  count_timer_update(&s_app.timer);
  hit_manager_update(&s_app.hit_manager);

  if(input_key_pressed(KEY_T)) {
    s_app.task_menu.is_active = !s_app.task_menu.is_active;
  } 

  // Player shooting the gun
  if(!input_key_pressed(KEY_SPACE)) {
    return;
  }

  // Checking for ray collisions against the targets.
  for(u32 i = 0; i < s_app.targets.size(); i++) {
    Object* object = s_app.targets[i];

    Ray ray = {
      .position = s_app.current_cam->position, 
      .direction = s_app.current_cam->direction,
    };

    RayIntersection intersect = ray_intersect(&ray, &object->body->transform, &object->collider);
    if(!intersect.has_intersected) {
      continue;
    }
     
    // There's a HIT!!
    s_app.score += hit_manager_calc_hit_score(&s_app.hit_manager, object->body->transform.position, intersect.intersection_point);
    target_spawner_hit(&s_app.target_spawner, object, ray);

    // Critical hit?? 
    if(hit_manager_is_critical(&s_app.hit_manager, object->body->transform.position, intersect.intersection_point)) {
      s_app.hit_manager.in_combo = true;
      s_app.hit_manager.total_combo++;
    }
    else { // Just a normal hit. Not a critical hit
      hit_manager_end_combo(&s_app.hit_manager);
    }

    break; // The ray is more likely (and more desirably) to hit one object
  }
    
  // Increase timer every three combos 
  if((s_app.hit_manager.total_combo % 3) == 0) {
    count_timer_increase(&s_app.timer, 10);
  } 

  // The player shot no objects, therefore end the combo 
  if(s_app.hit_manager.total_combo == 0) {
    hit_manager_end_combo(&s_app.hit_manager);
  }
  
  // Checking if the current balance is sufficient enough 
  // for completing a task
  task_menu_update(&s_app.task_menu, &s_app.score);
}

void app_render(void* user_data) {
  renderer_clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  editor_begin();

  renderer_begin(s_app.current_cam);

  // Render the objects
  for(auto& obj : s_app.objects) {
    object_render(obj);
  }

  // Render the targets 
  for(auto& target : s_app.targets) {
    object_render(target);
  }
  
  renderer_end();
  
  renderer2d_begin();

  // Rendering the score 
  render_text(25.0f, std::to_string(s_app.score) + "$", glm::vec2(10.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

  // Rendering the timer
  count_timer_render(&s_app.timer);

  // Rendering radical 
  render_quad(window_get_size() / 2.0f - 8.0f, glm::vec2(16.0f), glm::vec4(1.0f));
 
  // Rendering the tasks
  task_menu_render(&s_app.task_menu);

  renderer2d_end();
 
  editor_end();
  renderer_present();
}
/////////////////////////////////////////////////////////////////////////////////
