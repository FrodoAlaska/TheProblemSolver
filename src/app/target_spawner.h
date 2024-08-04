#pragma once

#include "defines.h"
#include "entities/object.h"
#include "physics/ray.h"

#include <glm/vec3.hpp>

#include <vector>
#include <queue>

// TargetSpawner
/////////////////////////////////////////////////////////////////////////////////
struct TargetSpawner {
  std::vector<Object*>* objects; 
  std::queue<Object*> inactive_objects;

  f32 spawn_counter, spawn_counter_limit;
  std::queue<glm::vec3> empty_seats;

  bool can_spawn;

  u32 frames; 
  u32 wait_frames;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void target_spawner_init(TargetSpawner* spawner, std::vector<Object*>& objs);
void target_spawner_hit(TargetSpawner* spawner, Object* obj, const Ray& ray);
void target_spawner_update(TargetSpawner* spawner);
void target_spawner_reset(TargetSpawner* spawner);
/////////////////////////////////////////////////////////////////////////////////
