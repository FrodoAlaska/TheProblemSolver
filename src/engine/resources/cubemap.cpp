#include "cubemap.h"
#include "defines.h"

#include <stb_image/stb_image.h>
#include <glad/gl.h>

#include <cstdio>
#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
CubeMap* cubemap_load(const std::string& path) {
  CubeMap* cm = new CubeMap{};

  glGenTextures(1, &cm->id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cm->id);

  // @TODO: This is bad. Find a way for the user to write these instead of hard coding it
  std::string ext = ".png";
  std::string texture_paths[] = {
    path + "right" + ext, 
    path + "left" + ext, 
    path + "top" + ext, 
    path + "bottom" + ext, 
    path + "front" + ext, 
    path + "back" + ext, 
  };

  // Load all of the 6 faces of the cubemap (i.e load all the 6 textures)
  for(u32 i = 0; i < 6; i++) {
    u8* data = stbi_load(texture_paths[i].c_str(), &cm->width, &cm->height, &cm->num_channels, 0);
    if(!data) {
      fprintf(stderr, "[ERROR]: Could not load cubemap texture at \'%s\'\n", texture_paths[i].c_str());
      stbi_image_free(data);

      continue;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, cm->width, cm->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return cm;
}

void cubemap_unload(CubeMap* cm) {
  if(!cm) {
    return;
  }

  delete cm;
}

void cubemap_use(CubeMap* cm) {
  if(!cm) {
    return;
  }

  glBindTexture(GL_TEXTURE_CUBE_MAP, cm->id);
}
/////////////////////////////////////////////////////////////////////////////////
