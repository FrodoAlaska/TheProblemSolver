#include "player.h"
#include "graphics/renderer.h"
#include "physics/physics_world.h"
#include "physics/physics_body.h"
#include "physics/collider.h"
#include "resources/mesh.h"

#include <glm/vec3.hpp>

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
Player* player_create(const glm::vec3& start_pos) {
  Player* player = new Player{};
  
  player->collider = BoxCollider{.half_size = glm::vec3(0.5f)};
  player->body = physics_world_add_body(PhysicsBodyDesc{
    .position = start_pos, 
    .type = PHYSICS_BODY_DYNAMIC, 
    .user_data = nullptr, 
    .mass = 1.0f, 
    .restitution = 1.0f, 
    .is_active = true
  });
  physics_body_add_collider(player->body, COLLIDER_BOX, &player->collider); 

  player->mesh = mesh_create();
  player->is_active = true;

  return player;
}

void player_destroy(Player* player) {
  if(!player) {
    return;
  }

  mesh_destroy(player->mesh);
  delete player;
}

void player_update(Player* player) {
  if(!player->is_active) {
    return;
  }
}

void player_render(Player* player) {
  if(!player->is_active) {
    return;
  }

  render_mesh(player->body->transform, player->mesh, glm::vec4(1.0f));
}
/////////////////////////////////////////////////////////////////////////////////
