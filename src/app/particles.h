#pragma once

#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void particles_init();
void particles_reset();
void particles_update();
void particles_render();

void particles_emit(const glm::vec3& pos);
/////////////////////////////////////////////////////////////////////////////////