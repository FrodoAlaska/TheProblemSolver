#pragma once

#include "physics/physics_body.h"
#include "physics/collider.h"
#include "resources/material.h"
#include "resources/mesh.h"

#include <glm/vec3.hpp>

#include <string>

// Object
/////////////////////////////////////////////////////////////////////////////////
struct Object {
  PhysicsBody* body; 
  BoxCollider collider;
  Mesh* mesh;
  Material* material;

  bool is_active;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Object* object_create(const glm::vec3& pos, const glm::vec3& coll_scale, const PhysicsBodyType type, const std::string& texture_id);
void object_destroy(Object* obj);
void object_render(Object* obj);
/////////////////////////////////////////////////////////////////////////////////
