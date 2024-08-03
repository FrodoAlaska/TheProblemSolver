#include "collider.h"
#include "collision_data.h"
#include "defines.h"
#include "math/transform.h"

#include <glm/vec3.hpp>

#include <cfloat>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
CollisionData collider_colliding(Collider* coll_a, const Transform* trans_a, Collider* coll_b, const Transform* trans_b) {
  // ORing the two types together to determine if both colliders are the same or not 
  i32 types = (i32)(coll_a->type | coll_b->type); 
  
  CollisionData data;
  CollisionPoint point;

  // No colliders 
  if(!coll_a->data || !coll_b->data) {
    data.point.has_collided = false;
    return data;
  }

  // Both are boxes
  if(types == COLLIDER_BOX) {
    point = aabb_colliding_ex((BoxCollider*)coll_a->data, trans_a, (BoxCollider*)coll_b->data, trans_b);
  } 
  // Both are spheres
  else if(types == COLLIDER_SPHERE) {
    point = sphere_colliding((SphereCollider*)coll_a->data, trans_a, (SphereCollider*)coll_b->data, trans_b);
  } 

  // One of them is a sphere and the other is a box.
  // Just make sure which is which
  if(coll_a->type == COLLIDER_SPHERE && coll_b->type == COLLIDER_BOX) {
    point = sphere_aabb_colliding((SphereCollider*)coll_a->data, trans_a, (BoxCollider*)coll_b->data, trans_b);
  } 
  else if(coll_b->type == COLLIDER_SPHERE && coll_a->type == COLLIDER_BOX) {
    point = sphere_aabb_colliding((SphereCollider*)coll_b->data, trans_a, (BoxCollider*)coll_a->data, trans_b);
  } 

  // Just make sure the bodies exist
  data.body_a = coll_a->body;
  data.body_b = coll_b->body;
  data.point = point; 

  return data;
}

CollisionPoint sphere_colliding(SphereCollider* sphere_a, const Transform* trans_a, SphereCollider* sphere_b, const Transform* trans_b) {
  f32 radii = sphere_a->radius + sphere_b->radius;
  glm::vec3 diff = trans_b->position - trans_a->position;
  f32 diff_len = diff.length();

  // Not colliding!
  if(diff_len > radii) {
    return CollisionPoint{.has_collided = false};
  }

  glm::vec3 normal = glm::normalize(diff);

  return CollisionPoint {
    .collision_point_a = normal * sphere_a->radius, 
    .collision_point_b = -normal * sphere_b->radius, 

    .normal = normal,
    .depth = radii - diff_len, 
    .has_collided = true,
  };
}

CollisionPoint sphere_aabb_colliding(SphereCollider* sphere, const Transform* sphere_trans, BoxCollider* box, const Transform* box_trans) {
  glm::vec3 diff = box_trans->position - sphere_trans->position;
  glm::vec3 closest_point = glm::clamp(diff, -box->half_size, box->half_size);
  
  glm::vec3 point = diff - closest_point; 
  f32 point_dist = point.length();

  // Sphere and box are not intersecting
  if(point_dist > sphere->radius) {
    return CollisionPoint{.has_collided = false};
  }

  glm::vec3 normal = glm::normalize(point);

  return CollisionPoint {
    // Sphere
    .collision_point_a = -normal * sphere->radius, 
   
    // Box
    .collision_point_b = glm::vec3(0.0f), 

    .normal = normal,
    .depth = sphere->radius - point_dist, 
    .has_collided = true,
  };
}

bool aabb_colliding(const glm::vec3& pos_a, const glm::vec3& size_a, const glm::vec3& pos_b, const glm::vec3& size_b) {
  glm::vec3 sum_size = size_a + size_b;
  glm::vec3 diff = pos_b - pos_a; 

  // The sum of the sizes is greater than the difference between
  // the two boxes on ALL axises
  if(glm::abs(diff.x) < sum_size.x && glm::abs(diff.y) < sum_size.y && glm::abs(diff.z) < sum_size.z) {
    return true;
  }

  return false;
}

CollisionPoint aabb_colliding_ex(BoxCollider* box_a, const Transform* trans_a, BoxCollider* box_b, const Transform* trans_b) {
  glm::vec3 min_a = trans_a->position - box_a->half_size;
  glm::vec3 max_a = trans_a->position + box_a->half_size;
  
  glm::vec3 min_b = trans_b->position - box_b->half_size;
  glm::vec3 max_b = trans_b->position + box_b->half_size;
 
  bool colliding = aabb_colliding(trans_a->position, box_a->half_size, trans_b->position, box_b->half_size);
  if(!colliding) {
    return CollisionPoint{.has_collided = false};
  }

  // All possible axises
  glm::vec3 axises[6] = {
    glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
  };

  // Compiling the distances between the minimums and maximums of both boxes
  f32 dists[6] = {
    (max_b.x - min_a.x), // Distance between A's left and B's right
    (max_a.x - min_b.x), // Distance between A's right and  B's left
    (max_b.y - min_a.y), // Distance between A's top and B's bottom
    
    (max_a.y - min_b.y), // Distance between A's bottom and B's top
    (max_b.z - min_a.z), // Distance between A's far and B's near
    (max_a.z - min_b.z), // Distance between A's near and B's far
  };

  f32 depth = FLT_MAX;
  glm::vec3 normal = glm::vec3(0.0f);

  // Trying to determine which has the least depth out of the distances.
  // The least wins. 
  for(u32 i = 0; i < 6; i++) {
    if(dists[i] < depth) {
      depth = dists[i];
      normal = axises[i];
    }
  } 

  return CollisionPoint{
    // @TODO: Get the exact collision point of the aabb collisions
    .collision_point_a = glm::vec3(0.0f),
    .collision_point_b = glm::vec3(0.0f), 

    .normal = normal, 
    .depth = depth,
    .has_collided = true,
  };
}
/////////////////////////////////////////////////////////////////////////////////
