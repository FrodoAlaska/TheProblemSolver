#include "hit_manager.h"
#include "defines.h"

#include <glm/glm.hpp>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define CRITICAL_HIT     80
#define COMBO_TIME_LIMIT 450.0f
#define MAX_HIT_POINTS   200
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void hit_manager_init(HitManager* hit) {
  hit->combo_timer = 0.0f; 
  hit->total_combo = 0;
  hit->in_combo = false;
}

void hit_manager_update(HitManager* hit) {
  if(!hit->in_combo) {
    return;
  }

  hit->combo_timer++;
  if(hit->combo_timer >= COMBO_TIME_LIMIT) {
    hit_manager_end_combo(hit);
  }
}

void hit_manager_end_combo(HitManager* hit) {
  hit->in_combo = false;
  hit->combo_timer = 0.0f;

  hit->total_combo = 0;
}

const u32 hit_manager_calc_hit_score(HitManager* hit, const glm::vec3& obj_pos, const glm::vec3& intersect_point) {
  f32 dist = glm::distance(obj_pos, intersect_point);
  return MAX_HIT_POINTS - (MAX_HIT_POINTS * dist);
}

const bool hit_manager_is_critical(HitManager* hit, const glm::vec3& obj_pos, const glm::vec3& intersect_point) {
  return hit_manager_calc_hit_score(hit, obj_pos, intersect_point) >= CRITICAL_HIT;
}
/////////////////////////////////////////////////////////////////////////////////
