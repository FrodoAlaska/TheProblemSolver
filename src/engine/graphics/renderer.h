#pragma once

#include "graphics/camera.h"
#include "resources/cubemap.h"
#include "resources/material.h"
#include "resources/mesh.h"
#include "math/transform.h"
#include "resources/model.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
const bool renderer_create();
void renderer_destroy();

void renderer_clear(const glm::vec4& color);
void renderer_begin(const Camera* cam);
void renderer_end();
void renderer_present();

// Render a mesh using the given material at the given transform
void render_mesh(const Transform& transform, Mesh* mesh, Material* mat);

// Render the mesh using the default basic material
void render_mesh(const Transform& transform, Mesh* mesh, const glm::vec4& color = glm::vec4(1.0f));

// Render an instanced cube
void render_cube(const glm::vec3& position, const glm::vec3& scale, const f32& rotation, const glm::vec4& color);
void render_cube(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color);

// Render a 3D model
// NOTE: This function will not render anything if the model is a 'nullptr'
void render_model(const Transform& transform, Model* model);

// Render the given cubemap
void render_cubemap(CubeMap* cm, const Camera* cam);
/////////////////////////////////////////////////////////////////////////////////
