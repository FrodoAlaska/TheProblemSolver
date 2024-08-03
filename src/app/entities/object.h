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
Object* object_create(const glm::vec3& scale, const PhysicsBodyDesc desc, const std::string& texture_id, const bool active = true);
void object_destroy(Object* obj);
void object_render(Object* obj);

// Actives or deactives the object and its body
void object_activate(Object* obj, const bool active);
/////////////////////////////////////////////////////////////////////////////////
