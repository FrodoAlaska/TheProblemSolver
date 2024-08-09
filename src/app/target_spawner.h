#pragma once

#include "defines.h"
#include "entities/target.h"
#include "physics/ray.h"

#include <glm/vec3.hpp>

#include <vector>
#include <queue>

// TargetSpawner
/////////////////////////////////////////////////////////////////////////////////
struct TargetSpawner {
  std::vector<Target*>* objects; 
  std::queue<glm::vec3> empty_seats;

  f32 spawn_counter, spawn_counter_limit;
  bool can_spawn;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void target_spawner_init(TargetSpawner* spawner, std::vector<Target*>& objs);
void target_spawner_hit(TargetSpawner* spawner, Target* target, const Ray& ray);
void target_spawner_update(TargetSpawner* spawner);
void target_spawner_reset(TargetSpawner* spawner);
/////////////////////////////////////////////////////////////////////////////////
