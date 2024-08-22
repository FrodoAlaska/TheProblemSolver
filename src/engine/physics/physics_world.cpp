#include "physics_world.h"
#include "core/event.h"
#include "math/transform.h"
#include "physics/collider.h"
#include "physics/collision_data.h"
#include "physics/physics_body.h"
#include "defines.h"
#include "utils/utils.h"

#include <cstdio>
#include <glm/vec3.hpp>

#include <vector>

// PhysicsWorld
/////////////////////////////////////////////////////////////////////////////////
struct PhysicsWorld {
  glm::vec3 gravity;
  
  std::vector<PhysicsBody*> bodies;
  std::vector<CollisionData> collisions;
};

static PhysicsWorld* s_world;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void check_collisions() {
  for(u32 i = 0; i < s_world->bodies.size(); i++) {
    // Skip inactive bodies
    PhysicsBody* body_a = s_world->bodies[i];
    if(!body_a->is_active) {
      continue;
    }

    for(u32 j = i + 1; j < s_world->bodies.size(); j++) {
      // Don't check for the same bodies 
      if(i == j) {
        continue;
      }

      PhysicsBody* body_b = s_world->bodies[j];
      if(!body_b->is_active) {
        continue;
      }

      CollisionData data = collider_colliding(&body_a->collider, &body_a->transform, &body_b->collider, &body_b->transform);
      
      // When a collision happens, an even gets dispatched to whoever cares to listen. 
      // The collision data also gets added to a vector to be resolved later.
      if(data.point.has_collided) {
        event_dispatch(EVENT_ENTITY_COLLISION, EventDesc{.coll_data = data});
        s_world->collisions.push_back(data);
      }
    }
  }
}

static void resolve_collisions() {
  for(auto& collision : s_world->collisions) {
    PhysicsBody* body_a = collision.body_a;
    PhysicsBody* body_b = collision.body_b;
 
    f32 sum_mass = body_a->inverse_mass + body_b->inverse_mass;

    // Move the bodies away from each other.
    // We're basically pushing the two bodies away from each other taking into account the normal, depth, and masses.
    // Heavier bodies (those with more mass) will be pushed away less than lighter bodies. 
    if(body_a->type != PHYSICS_BODY_STATIC) {
      glm::vec3 new_pos_a = body_a->transform.position - ((collision.point.normal * collision.point.depth) * (body_a->inverse_mass / sum_mass));
      transform_translate(&body_a->transform, new_pos_a); 
    }

    if(body_b->type != PHYSICS_BODY_STATIC) {
      glm::vec3 new_pos_b = body_b->transform.position + ((collision.point.normal * collision.point.depth) * (body_b->inverse_mass / sum_mass));
      transform_translate(&body_b->transform, new_pos_b); 
    }

    // Integrate the Impulse Method for collision response
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glm::vec3 rel_pos_a = collision.point.collision_point_a - body_a->transform.position; 
    glm::vec3 rel_pos_b = collision.point.collision_point_b - body_b->transform.position; 

    glm::vec3 ang_vel_a = glm::cross(body_a->angular_velocity, rel_pos_a);
    glm::vec3 ang_vel_b = glm::cross(body_b->angular_velocity, rel_pos_b);

    glm::vec3 full_vel_a = body_a->linear_velocity + ang_vel_a;
    glm::vec3 full_vel_b = body_b->linear_velocity + ang_vel_b;

    glm::vec3 contact_vel = full_vel_b - full_vel_a;

    glm::vec3 inertia_a = glm::cross(body_a->inertia_tensor * glm::cross(rel_pos_a, collision.point.normal), rel_pos_a);
    glm::vec3 inertia_b = glm::cross(body_b->inertia_tensor * glm::cross(rel_pos_b, collision.point.normal), rel_pos_b);
    f32 angular_effect = glm::dot(inertia_a + inertia_b, collision.point.normal); 

    f32 restitution = body_a->restitution * body_b->restitution;

    f32 impulse_force = glm::dot(contact_vel, collision.point.normal);
    f32 impulse = (-(1.0f + restitution) * impulse_force) / sum_mass;// + angular_effect);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // The resulting impulse
    glm::vec3 full_impulse = impulse * collision.point.normal;

    // Giving impulse to the two bodies based on the mass
    physics_body_apply_linear_impulse(body_a, -full_impulse); 
    physics_body_apply_linear_impulse(body_b, full_impulse); 

    // @TODO: Have to get the exact collision point in order for this to work
    // physics_body_apply_angular_impulse(body_a, glm::cross(rel_pos_a, -full_impulse));
    // physics_body_apply_angular_impulse(body_b, glm::cross(rel_pos_b, full_impulse));
  }

  // Empty out the collisions after resolving all of them
  if(!s_world->collisions.empty()) {
    s_world->collisions.clear();
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void physics_world_create(const glm::vec3& gravity) {
  s_world = new PhysicsWorld{}; 
  s_world->gravity = gravity;
}

void physics_world_destroy() {
  for(auto& body : s_world->bodies) {
    physics_body_destroy(body);
  }
  s_world->bodies.clear();

  delete s_world;
}

void physics_world_set_gravity(const glm::vec3& gravity) {
  s_world->gravity = gravity;
}

void physics_world_update(f32 dt) {
  /*
   * NOTE:
   * This physics system uses the Semi-Implicit Euler integration system. 
   * It is perhaps not the best/accurate integration out there. However, 
   * it does satisfy the needs of a real-time game physics simulation. 
   * It's fast, easy to use, and accurate enough for game simulations. 
   *
   * Here's a link for more information: 
   * https://en.wikipedia.org/wiki/Semi-implicit_Euler_method
   */

  f32 damp_factor = 1.0f - 0.95f;
  f32 frame_damp = glm::pow(damp_factor, dt);

  for(auto& body : s_world->bodies) {
    // Inactive and static bodies do not need to be updated 
    if(!body->is_active || body->type == PHYSICS_BODY_STATIC) {
      continue;
    }

    // Apply the acceleration
    glm::vec3 acceleration = body->force * body->inverse_mass;

    // Don't apply gravity to infinitely heavy bodies
    if(body->inverse_mass < 0) {  
      acceleration += s_world->gravity; 
    }

    // Semi-Implicit Euler in effect
    body->linear_velocity += acceleration * dt;
    // body->linear_velocity *= frame_damp; // Applying some linear damping
    body->transform.position += body->linear_velocity * dt;
    
    // Adding angular velocity 
    glm::vec3 angular_accel = body->torque * body->inertia_tensor;
    body->angular_velocity += angular_accel * dt;
    body->angular_velocity *= frame_damp; // Apply some damping to the angular velocity as well 

    // Adding the rotation to the body 
    glm::quat orientation = body->transform.rotation;
    orientation += (glm::quat(0.0f, body->angular_velocity * dt * 0.5f) * orientation);
    orientation = glm::normalize(orientation);

    // Moving the body by the new position/displacment and rotating it as weel
    transform_translate(&body->transform, body->transform.position); 
    transform_rotate(&body->transform, orientation);

    // Clear all forces accumulated this frame
    body->force = glm::vec3(0.0f); 
  } 

  check_collisions();
  resolve_collisions();
}

PhysicsBody* physics_world_add_body(const PhysicsBodyDesc& desc) {
  PhysicsBody* body = physics_body_create(desc);
  s_world->bodies.push_back(body);
  
  return body;
}
/////////////////////////////////////////////////////////////////////////////////
