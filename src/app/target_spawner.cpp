#include "target_spawner.h"
#include "defines.h"
#include "entities/object.h"
#include "physics/ray.h"

#include <glm/vec3.hpp>

#include <vector>
#include <queue>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define MAX_POSITIONS 5
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void spawn_target(TargetSpawner* spawner) {
  for(auto& obj : *spawner->objects) {
    // We only care about the inactive targets
    if(obj->is_active) {
      continue;
    }

    glm::vec3 new_pos = spawner->empty_seats.front();
    spawner->empty_seats.pop();

    // Spawn the target
    transform_translate(&obj->body->transform, new_pos);
    object_activate(obj, true);  
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void target_spawner_init(TargetSpawner* spawner, std::vector<Object*>& objs, const f32 counter_limit) {
  spawner->objects = &objs;

  spawner->spawn_counter = 0.0f; 
  spawner->spawn_counter_limit = counter_limit;

  // Preloading all of the positions that will be used 
  // and the targets too
  for(u32 i = 0; i < MAX_POSITIONS; i++) {
    spawner->empty_seats.push(glm::vec3(50.0f, -0.1f, i * 5.0f));

    spawner->objects->push_back(object_create(glm::vec3(1.0f), PhysicsBodyDesc {
      .position = glm::vec3(-1000.0f), 
      .type = PHYSICS_BODY_DYNAMIC,
    }, "target_texture", false));
  }

  spawner->last_position = glm::vec3(0.0f);

  spawner->can_spawn = true;
}

void target_spawner_hit(TargetSpawner* spawner, Object* obj, const Ray& ray) {
  // Add new empty seat 
  spawner->empty_seats.push(obj->body->transform.position);

  // Be with the force!!!
  physics_body_apply_linear_impulse(obj->body, -ray.direction * 50.0f);

  // Object will be deactivated later on
  spawner->inactive_objects.push(obj); 
}

void target_spawner_update(TargetSpawner* spawner) {
  if(!spawner->can_spawn) {
    return;
  }

  spawner->spawn_counter++; 
  if(spawner->spawn_counter >= spawner->spawn_counter_limit) {
    spawn_target(spawner);
    spawner->spawn_counter = 0.0f;
  }

  if(spawner->inactive_objects.empty()) {
    return;
  }

  spawner->frames++;
  if(spawner->frames >= spawner->wait_frames) {
    object_activate(spawner->inactive_objects.front(), false);
    spawner->inactive_objects.pop();

    spawner->frames = 0;
  }
}
/////////////////////////////////////////////////////////////////////////////////
