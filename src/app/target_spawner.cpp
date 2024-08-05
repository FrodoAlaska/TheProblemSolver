#include "target_spawner.h"
#include "defines.h"
#include "entities/object.h"
#include "math/transform.h"
#include "physics/ray.h"

#include <glm/vec3.hpp>

#include <vector>
#include <queue>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define MAX_TARGETS 5
#define MAX_COUNTER_LIMIT 120.0f
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void spawn_targets(TargetSpawner* spawner) {
  for(auto& obj : *spawner->objects) {
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
void target_spawner_init(TargetSpawner* spawner, std::vector<Object*>& objs) {
  spawner->objects = &objs;

  spawner->spawn_counter = 0.0f; 
  spawner->spawn_counter_limit = MAX_COUNTER_LIMIT;

  // Preloading all of the positions that will be used 
  // and the targets too
  for(u32 i = 0; i < MAX_TARGETS; i++) {
    spawner->objects->push_back(object_create(glm::vec3(1.0f), PhysicsBodyDesc {
      .position = glm::vec3(50.0f, -0.1f, i * 5.0f), 
      .type = PHYSICS_BODY_DYNAMIC,
    }, "target_texture"));
  }

  spawner->can_spawn = false;

  spawner->frames = 0; // Just counts the frames
  spawner->wait_frames = 300; // How much to wait before deactivating the target
}

void target_spawner_hit(TargetSpawner* spawner, Object* obj, const Ray& ray) {
  // Add new empty seat 
  glm::vec3 object_pos = obj->body->transform.position;
  spawner->empty_seats.push(glm::vec3(object_pos.x, -0.1f, object_pos.z));

  // Be with the force!!!
  physics_body_apply_linear_impulse(obj->body, -ray.direction * 50.0f);

  // Object will be deactivated later on
  spawner->inactive_objects.push(obj); 
}

void target_spawner_update(TargetSpawner* spawner) {
  // Spawn the targets when all of them have been shot 
  if(spawner->empty_seats.size() == MAX_TARGETS) {
    // For aesthetical reasons, the targets will not spawn immediately. 
    // Instead there should be a smaller timer after which all the 
    // targets will spawn. 
    spawner->can_spawn = true;
  }

  if(!spawner->can_spawn) {
    return;
  }

  spawner->spawn_counter++;
  if(spawner->spawn_counter >= spawner->spawn_counter_limit) {
    spawn_targets(spawner);

    spawner->spawn_counter = 0.0f;
    spawner->can_spawn = false;
  }
}

void target_spawner_reset(TargetSpawner* spawner) {
  spawner->frames = 0;
  spawner->spawn_counter = 0; 
  spawner->spawn_counter_limit = MAX_COUNTER_LIMIT;
  spawner->can_spawn = false;

  // Resetting all of the objects to their original positions
  for(u32 i = 0; i < MAX_TARGETS; i++) {
    spawner->objects->at(i)->body->linear_velocity = glm::vec3(0.0f);
    transform_translate(&spawner->objects->at(i)->body->transform, glm::vec3(50.0f, -0.1f, i * 5.0f)); 
  }
}
/////////////////////////////////////////////////////////////////////////////////
