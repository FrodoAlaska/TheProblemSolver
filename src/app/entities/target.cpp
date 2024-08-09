#include "target.h"
#include "graphics/renderer.h"
#include "physics/physics_body.h"
#include "physics/collider.h"
#include "math/transform.h"
#include "physics/physics_world.h"
#include "resources/resource_manager.h"

#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Target* target_create(const glm::vec3& pos) {
  Target* target = new Target{};

  transform_create(&target->transform, pos);
  transform_scale(&target->transform, glm::vec3(0.2f));

  PhysicsBodyDesc desc = {
    .position = target->transform.position, 
    .type = PHYSICS_BODY_DYNAMIC, 
  };

  target->body = physics_world_add_body(desc);
  target->collider = BoxCollider{.half_size = glm::vec3(0.5f, 1.26f, 0.35f) / 2.0f};
  physics_body_add_collider(target->body, COLLIDER_BOX, &target->collider);

  target->model = resources_add_model("bottle", "models/Bottle/BeerBottle.obj");
  target->is_active = true;

  return target;
}

void target_destroy(Target* target) {
  if(target) {
    delete target;
  }
}

void target_render(Target* target) {
  if(!target->is_active) {
    return;
  }

  glm::vec3 body_trans = target->body->transform.position;
  transform_translate(&target->transform, body_trans + glm::vec3(0.0f, -0.66f, 3.262f));
  render_model(target->transform, target->model);
}

void target_active(Target* target, const bool active) {
  target->is_active = active; 
  target->body->is_active = active; 

  target->body->linear_velocity = glm::vec3(0.0f);
  target->body->angular_velocity = glm::vec3(0.0f);
}
/////////////////////////////////////////////////////////////////////////////////
