#pragma once

#include "physics/physics_body.h"
#include "physics/collider.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "math/transform.h"

#include <glm/vec3.hpp>

#include <string>

// Object
/////////////////////////////////////////////////////////////////////////////////
struct Object {
  Transform transform;
  PhysicsBody* body; 
  BoxCollider collider;
  Mesh* mesh;
  Material* material;

  bool is_active;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Object* object_create(const Transform transform, const glm::vec3& coll_scale, const PhysicsBodyType type, const std::string& texture_id);
void object_destroy(Object* obj);
void object_render(Object* obj);

// Actives or deactives the object and its body
void object_activate(Object* obj, const bool active);
/////////////////////////////////////////////////////////////////////////////////
