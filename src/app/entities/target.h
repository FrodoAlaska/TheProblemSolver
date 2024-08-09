#pragma once

#include "physics/physics_body.h"
#include "physics/collider.h"
#include "math/transform.h"
#include "resources/model.h"

#include <glm/vec3.hpp>

// Target
/////////////////////////////////////////////////////////////////////////////////
struct Target {
  Transform transform;
  PhysicsBody* body; 
  BoxCollider collider;
  Model* model;

  bool is_active;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Target* target_create(const glm::vec3& pos);
void target_destroy(Target* target);
void target_render(Target* target);
void target_active(Target* target, const bool active);
/////////////////////////////////////////////////////////////////////////////////
