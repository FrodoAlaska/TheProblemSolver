#include "ray.h"
#include "defines.h"
#include "math/transform.h"
#include "physics/collider.h"

#include <cmath>
#include <glm/vec3.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const RayIntersection ray_intersect(const Ray* ray, const Transform* transform, BoxCollider* box) {
  box->min = transform->position - box->half_size; 
  box->max = transform->position + box->half_size; 
  glm::vec3 tvals(-1.0f);
   
  // Checking which sides of the box to check against
  for(u32 i = 0; i < 3; i++) {
    // Get the negative sides of the cube if the direction of the ray 
    // is positive
    if(ray->direction[i] > 0) {
      tvals[i] = (box->min[i] - ray->position[i]) / ray->direction[i]; 
    }
    // The positive sides of the cube 
    else if(ray->direction[i] < 0) {
      tvals[i] = (box->max[i] - ray->position[i]) / ray->direction[i];
    }
  }
    
  f32 best_t = 0.0f;

  // Try to get the max value
  if(tvals.x > tvals.y && tvals.x > tvals.z) {
    best_t = tvals.x;
  } 
  else if(tvals.y > tvals.x && tvals.y > tvals.z) {
    best_t = tvals.y;
  }
  else if(tvals.z > tvals.x && tvals.z > tvals.y) {
    best_t = tvals.z;
  }
    
  // The value is behind the ray
  if(best_t < 0.0f) {
    return RayIntersection{.has_intersected = false};
  }
    
  float epsilon = 0.0001f; // Just to get some floating point precision
  glm::vec3 intersection = ray->position + (ray->direction * best_t); // Getting the intersection point
  
  for(u32 i = 0; i < 3; i++) {
    // The best intersection that was found doesn't even touch the box!!!
    // It's outside of the bounds of the box
    if((intersection[i] + epsilon) < box->min[i] || intersection[i] > box->max[i]) {
      return RayIntersection{.has_intersected = false};
    }
  }

  return RayIntersection{
    .intersection_point = intersection, 
    .distance = best_t, 
    .has_intersected = true
  };
}

const RayIntersection ray_intersect(const Ray* ray, const Transform* transform, SphereCollider* sphere) {
  // Get the direction between the ray and the sphere 
  glm::vec3 dir = ray->position - transform->position; 

  // Project the direction on the ray's direction 
  f32 sphere_proj = glm::dot(dir, ray->direction);

  // The ray is pointing in the other direction not at the sphere
  if(sphere_proj < 0.0f) {
    return RayIntersection{.has_intersected = false};
  } 

  // Get the closes point to the ray
  glm::vec3 point = ray->position + (ray->direction * sphere_proj);


  f32 sphere_dist = (point - transform->position).length();
  if(sphere_dist > sphere->radius) { // No collisions with the sphere
    return RayIntersection{.has_intersected = false};
  }  

  // Getting the exact point of intersection
  f32 offset = sqrt((sphere_dist * sphere_dist) - (sphere->radius * sphere->radius));

  return RayIntersection{
    .intersection_point = ray->position + (ray->direction * sphere_proj),
    .distance = sphere_proj - offset,
    .has_intersected = true,
  };
}
/////////////////////////////////////////////////////////////////////////////////
