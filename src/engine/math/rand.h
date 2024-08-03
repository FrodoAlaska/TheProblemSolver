#pragma once

#include "defines.h"

#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const u32 random_u32();
const u32 random_u32(const u32 min, const u32 max);
const f32 random_f32(const f32 min, const f32 max);
const glm::vec3 random_vec3(const glm::vec3& min, const glm::vec3& max);
/////////////////////////////////////////////////////////////////////////////////
