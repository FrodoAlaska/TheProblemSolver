#pragma once

#include "defines.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Transform
/////////////////////////////////////////////////////////////////////////////////
struct Transform {
  glm::vec3 position, scale;
  glm::quat rotation;
  glm::mat4 transform;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void transform_create(Transform* transform, 
                      const glm::vec3& pos, 
                      const glm::quat& rot = glm::quat(0.0f, 0.0f, 0.0f, 0.0f), 
                      const glm::vec3& scale = glm::vec3(1.0f));
void transform_translate(Transform* transform, const glm::vec3& pos);
void transform_rotate(Transform* transform, const glm::quat& rotation);
void transform_rotate(Transform* transform, const f32 angle, const glm::vec3& axis);
void transform_rotate(Transform* transform, const glm::vec4& rotation);
void transform_scale(Transform* transform, const glm::vec3& scale);
/////////////////////////////////////////////////////////////////////////////////
