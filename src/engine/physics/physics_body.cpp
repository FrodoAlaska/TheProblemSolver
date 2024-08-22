#include "physics_body.h"
#include "defines.h"
#include "math/transform.h"
#include "physics/collider.h"

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

// Private functions 
/////////////////////////////////////////////////////////////////////////////////
static void build_cube_tensor(PhysicsBody* body, const glm::vec3& scale) {
  f32 x = (1.0f / 12.0f) * body->mass * ((scale.z * scale.z) + (scale.y * scale.y));
  f32 y = (1.0f / 12.0f) * body->mass * ((scale.x * scale.x) + (scale.z * scale.z));
  f32 z = (1.0f / 12.0f) * body->mass * ((scale.x * scale.x) + (scale.y * scale.y));

  body->inertia_tensor = glm::mat3(x,    0.0f, 0.0f, 
                                   0.0f, y,    0.0f, 
                                   0.0f, 0.0f, z);
  body->inverse_inertia_tensor = glm::inverse(body->inertia_tensor);
}

static void build_sphere_tensor(PhysicsBody* body, const f32 radius) {
  f32 i = 2.0f / 5.0f * body->mass * (radius * radius);

  body->inertia_tensor = glm::mat3(i,    0.0f, 0.0f, 
                                   0.0f, i,    0.0f, 
                                   0.0f, 0.0f, i);
  body->inverse_inertia_tensor = glm::inverse(body->inertia_tensor);
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
PhysicsBody* physics_body_create(const PhysicsBodyDesc& desc) {
  PhysicsBody* body = new PhysicsBody{};
  
  transform_create(&body->transform, desc.position);
  body->type = desc.type;
  
  body->force = glm::vec3(0.0f);
  body->torque = glm::vec3(0.0f);

  body->linear_velocity = glm::vec3(0.0f);
  body->angular_velocity = glm::vec3(0.0f);

  body->inertia_tensor = glm::mat3(1.0f);
  body->inverse_inertia_tensor = glm::mat3(-1.0f);

  body->mass = desc.mass;
  body->inverse_mass = desc.mass * -1;
  body->restitution = desc.restitution; 

  body->is_active = desc.is_active;
  body->user_data = desc.user_data;

  return body;
}

void physics_body_destroy(PhysicsBody* body) {
  if(!body) {
    return;
  } 

  delete body;
}

void physics_body_add_collider(PhysicsBody* body, ColliderType type, void* collider) {
  body->collider.type = type;
  body->collider.data = collider;
  body->collider.body = body;

  // Determining the size of the size of the collider 
  switch(type) {
    case COLLIDER_BOX: {
      BoxCollider* coll = (BoxCollider*)collider;
      transform_scale(&body->transform, coll->half_size * 2.0f);
      build_cube_tensor(body, coll->half_size);
    }
      break;
    case COLLIDER_SPHERE:
      SphereCollider* coll = (SphereCollider*)collider;
      transform_scale(&body->transform, glm::vec3(coll->radius)); // TODO: What?? Does this even work??
      build_sphere_tensor(body, coll->radius); 
      break;
  }
}

void physics_body_apply_force_at(PhysicsBody* body, const glm::vec3& force, const glm::vec3& pos) {
  if(body->type == PHYSICS_BODY_STATIC) {
    return;
  }

  glm::vec3 local_pos = pos - body->transform.position;

  body->force += force; 
  body->torque += glm::cross(local_pos, -force); 
}

void physics_body_apply_linear_force(PhysicsBody* body, const glm::vec3& force) {
  if(body->type == PHYSICS_BODY_STATIC) {
    return;
  }

  body->force += force;
}

void physics_body_apply_angular_force(PhysicsBody* body, const glm::vec3& force) {
  if(body->type == PHYSICS_BODY_STATIC) {
    return;
  }

  body->torque += force;
}

void physics_body_apply_linear_impulse(PhysicsBody* body, const glm::vec3& force) {
  if(body->type == PHYSICS_BODY_STATIC) {
    return;
  }

  body->linear_velocity += force * body->inverse_mass;
}

void physics_body_apply_angular_impulse(PhysicsBody* body, const glm::vec3& force) {
  if(body->type == PHYSICS_BODY_STATIC) {
    return;
  }

  body->angular_velocity += body->inverse_inertia_tensor * force;
}
/////////////////////////////////////////////////////////////////////////////////
