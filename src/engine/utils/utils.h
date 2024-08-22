#pragma once

#include "engine/graphics/camera.h"

#include <glm/glm.hpp>

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const glm::vec3 screen_to_world(const glm::vec3& position, const Camera* camera);
const std::string vec3_to_string(const glm::vec3& vec);
/////////////////////////////////////////////////////////////////////////////////
