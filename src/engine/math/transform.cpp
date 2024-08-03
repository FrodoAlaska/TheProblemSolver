#include "transform.h"
#include "defines.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void update_transform(Transform* transform) {
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), transform->position);
  glm::mat4 rotate = glm::mat4_cast(transform->rotation);
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform->scale);

  transform->transform = translate * rotate * scale;
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void transform_create(Transform* transform, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) {
  transform->position = pos; 
  transform->rotation = rot; 
  transform->scale    = scale;

  transform->transform = glm::mat4(1.0f);
  update_transform(transform);
}

void transform_translate(Transform* transform, const glm::vec3& pos) {
  transform->position = pos; 
  update_transform(transform);
}

void transform_rotate(Transform* transform, const glm::quat& rotation) {
  transform->rotation = glm::normalize(rotation);
  update_transform(transform);
}

void transform_rotate(Transform* transform, const f32 angle, const glm::vec3& axis) {
  transform_rotate(transform, glm::angleAxis(angle, axis));
}

void transform_rotate(Transform* transform, const glm::vec4& rotation) {
  transform_rotate(transform, rotation.w, glm::vec3(rotation));
}

void transform_scale(Transform* transform, const glm::vec3& scale) {
  transform->scale = scale; 
  update_transform(transform);
}
/////////////////////////////////////////////////////////////////////////////////
