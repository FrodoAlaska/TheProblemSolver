#pragma once

#include "physics/physics_body.h"
#include "defines.h"

#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void physics_world_create(const glm::vec3& gravity);
void physics_world_destroy();

void physics_world_set_gravity(const glm::vec3& gravity);
void physics_world_update(f32 dt);

PhysicsBody* physics_world_add_body(const PhysicsBodyDesc& desc);
/////////////////////////////////////////////////////////////////////////////////
