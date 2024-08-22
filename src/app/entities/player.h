#pragma once

#include "physics/physics_body.h"
#include "physics/collider.h"
#include "resources/mesh.h"

#include <glm/vec3.hpp>

// Player
/////////////////////////////////////////////////////////////////////////////////
/*
 * Again, much like the 'Object' struct, the 'Player' is just a generic way 
 * of defining what a "player" is in every game. This is just the usual suspects 
 * when it comes to creating a player object in a game. However, it's not the final 
 * look of a player. This might be a very long struct with many additions. Though, 
 * much like the 'Object', it's good for now.
*/
struct Player {
  PhysicsBody* body;
  BoxCollider collider; 
  Mesh* mesh;

  bool is_active;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
Player* player_create(const glm::vec3& start_pos);
void player_destroy(Player* player);
void player_update(Player* player);
void player_render(Player* player);
/////////////////////////////////////////////////////////////////////////////////
