#include "target_spawner.h"
#include "audio/audio_system.h"
#include "audio/sound_type.h"
#include "defines.h"
#include "entities/target.h"
#include "math/transform.h"
#include "physics/ray.h"

#include <glm/vec3.hpp>

#include <vector>
#include <queue>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define MAX_TARGETS 6
#define MAX_COUNTER_LIMIT 50.0f
#define TARGET_START_POS glm::vec3(50.0f, 1.0f, i * 3.0f)
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void spawn_targets(TargetSpawner* spawner) {
  audio_system_play(SOUND_BOTTLE_SPAWN, 1.0f);

  for(u32 i = 0; i < MAX_TARGETS; i++) {
    Target* target = spawner->objects->at(i);

    glm::vec3 new_pos = TARGET_START_POS;
    spawner->empty_seats.pop();

    // Spawn the target
    transform_translate(&target->body->transform, new_pos);
    transform_translate(&target->transform, new_pos);
    target_active(target, true);  

    spawner->current_targets++;
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void target_spawner_init(TargetSpawner* spawner, std::vector<Target*>& targets) {
  spawner->objects = &targets;

  // Preloading all of the positions that will be used 
  // and the targets too
  for(u32 i = 0; i < MAX_TARGETS; i++) {
    spawner->objects->push_back(target_create(TARGET_START_POS));
  }

  spawner->spawn_counter = 0.0f; 
  spawner->spawn_counter_limit = MAX_COUNTER_LIMIT;
  spawner->can_spawn = false;
  spawner->current_targets = 6;
}

void target_spawner_hit(TargetSpawner* spawner, Target* target, const Ray& ray) {
  // Add new empty seat 
  glm::vec3 object_pos = target->transform.position;
  spawner->empty_seats.push(glm::vec3(object_pos.x, -0.1f, object_pos.z));
  
  spawner->current_targets--;

  target_active(target, false);
}

void target_spawner_update(TargetSpawner* spawner) {
  // Spawn the targets when all of them have been shot 
  if(spawner->current_targets == 0) {
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
  spawner->spawn_counter = 0; 
  spawner->spawn_counter_limit = MAX_COUNTER_LIMIT;
  spawner->can_spawn = false;
  spawner->current_targets = 6;

  // Resetting all of the objects to their original positions
  for(u32 i = 0; i < MAX_TARGETS; i++) {
    transform_translate(&spawner->objects->at(i)->body->transform, TARGET_START_POS); 
    transform_translate(&spawner->objects->at(i)->transform, TARGET_START_POS); 

    target_active(spawner->objects->at(i), true);
  }
}
/////////////////////////////////////////////////////////////////////////////////
