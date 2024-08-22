#pragma once

#include "engine/graphics/camera.h"

#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void editor_init(const glm::vec3& camera_pos, const glm::vec3& camera_target, const bool initial_active);
void editor_shutdown();
void editor_begin();
void editor_end();

void editor_set_active(bool active);

const bool editor_active();
const Camera& editor_get_camera();
/////////////////////////////////////////////////////////////////////////////////
