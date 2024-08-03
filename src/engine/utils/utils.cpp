#include "utils.h"
#include <glm/glm.hpp>

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const glm::vec3 screen_to_world(const glm::vec3& position, const Camera* camera) {
  glm::vec4 world_pos = glm::vec4(position, 1.0f) * glm::inverse(camera->view_projection);
  return glm::vec3(world_pos.x, world_pos.y, world_pos.z);
}

const std::string vec3_to_string(const glm::vec3& vec) {
  return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}
/////////////////////////////////////////////////////////////////////////////////
