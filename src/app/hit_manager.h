#pragma once

#include "defines.h"

#include <glm/glm.hpp>

// HitManager
/////////////////////////////////////////////////////////////////////////////////
struct HitManager {
  f32 combo_timer;
  u32 total_combo;

  bool in_combo;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void hit_manager_init(HitManager* hit);
void hit_manager_update(HitManager* hit);
void hit_manager_end_combo(HitManager* hit);
const u32 hit_manager_calc_hit_score(HitManager* hit, const glm::vec3& obj_pos, const glm::vec3& intersect_point);
const bool hit_manager_is_critical(HitManager* hit, const glm::vec3& obj_pos, const glm::vec3& intersect_point);
/////////////////////////////////////////////////////////////////////////////////
