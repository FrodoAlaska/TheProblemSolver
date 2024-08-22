#include "particles.h"
#include "defines.h"
#include "graphics/renderer.h"
#include "math/rand.h"
#include "math/transform.h"
#include "physics/physics_world.h"
#include "physics/physics_body.h"

#include <glm/vec3.hpp>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define PARTICLES_MAX  32
#define FADE_TIME      100.0f
#define PARTICLE_FORCE 500.0f
/////////////////////////////////////////////////////////////////////////////////

// ParticleManager
/////////////////////////////////////////////////////////////////////////////////
struct ParticleManager {
  PhysicsBody* particles[PARTICLES_MAX];

  f32 timer;
  bool is_active;
};

static ParticleManager s_particles;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void activate_particles(const glm::vec3& pos) {
  for(u32 i = 0; i < PARTICLES_MAX; i++) {
    PhysicsBody* particle = s_particles.particles[i];
    particle->is_active = true;
  
    glm::vec3 dir = random_vec3(glm::vec3(-1.0f), glm::vec3(1.0f));

    transform_translate(&particle->transform, pos);
    physics_body_apply_linear_force(particle, glm::vec3(PARTICLE_FORCE) * dir);
  }
}

static void deactivate_particles() {
  for(u32 i = 0; i < PARTICLES_MAX; i++) {
    PhysicsBody* particle = s_particles.particles[i];
    particle->is_active = false;

    particle->linear_velocity = glm::vec3(0.0f);
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void particles_init() {
  for(u32 i = 0; i < PARTICLES_MAX; i++) {
    PhysicsBodyDesc desc = {
      .position = glm::vec3(-1000.0f), 
      .type = PHYSICS_BODY_DYNAMIC, 
      .is_active = false
    };

    s_particles.particles[i] = physics_world_add_body(desc);
  }

  s_particles.timer = 0.0f; 
  s_particles.is_active = false;
}

void particles_reset() {
  for(u32 i = 0; i < PARTICLES_MAX; i++) {
    s_particles.particles[i]->is_active = false;
  }

  s_particles.timer = 0.0f; 
  s_particles.is_active = false;
}

void particles_update() {
  if(!s_particles.is_active) {
    return;
  }

  s_particles.timer++; 
  if(s_particles.timer >= FADE_TIME) {
    s_particles.is_active = false;
    s_particles.timer = 0.0f; 

    deactivate_particles();
  }
}

void particles_render() {
  if(!s_particles.is_active) {
    return;
  }

  for(u32 i = 0; i < PARTICLES_MAX; i++) {
    render_cube(s_particles.particles[i]->transform.position, glm::vec3(0.1f), glm::vec4(1.0f));
  }
}

void particles_emit(const glm::vec3& pos) {
  activate_particles(pos);
  s_particles.is_active = true;
}
/////////////////////////////////////////////////////////////////////////////////
