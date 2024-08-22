#include "object.h"
#include "graphics/renderer.h"
#include "physics/physics_body.h"
#include "physics/collider.h"
#include "physics/physics_world.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "resources/resource_manager.h"
#include "resources/texture.h"

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
Object* object_create(const glm::vec3& pos, const glm::vec3& coll_scale, const PhysicsBodyType type, const std::string& texture_id) {
  Object* obj = new Object{};

  PhysicsBodyDesc desc = {
    .position = pos, 
    .type = type,
  };
  obj->body = physics_world_add_body(desc);
  obj->collider = BoxCollider{.half_size = coll_scale / 2.0f};
  physics_body_add_collider(obj->body, COLLIDER_BOX, &obj->collider);

  obj->mesh = mesh_create();
  obj->material = material_load(resources_get_texture(texture_id), nullptr, resources_get_shader("default_shader-3d"));
  
  obj->is_active = true;

  return obj;
}

void object_destroy(Object* obj) {
  // Why are you trying to delete a null object???
  if(!obj) {
    return;
  }

  mesh_destroy(obj->mesh);
  delete obj;
}

void object_render(Object* obj) {
  // Don't render an inactive object. 
  // This is better for performance
  if(!obj->is_active) {
    return;
  }

  render_mesh(obj->body->transform, obj->mesh, obj->material);
}
/////////////////////////////////////////////////////////////////////////////////
