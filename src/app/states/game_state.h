#pragma once

#include "defines.h"
#include "graphics/camera.h"
#include "tasks_menu.h"
#include "count_timer.h"
#include "target_spawner.h"
#include "entities/object.h"
#include "hit_manager.h"
#include "ui/ui_text.h"

#include <vector>

// GameState
/////////////////////////////////////////////////////////////////////////////////
struct GameState {
  Camera camera;

  std::vector<Object*> objects;
  std::vector<Object*> targets;

  TargetSpawner target_spawner;
  CountTimer timer;
  HitManager hit_manager;
  TaskMenu task_menu;

  u32 score = 0;

  bool is_paused; 
  UIText pause_text;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
void game_state_init(GameState* game);
void game_state_update(GameState* game);
void game_state_render(GameState* game);
void game_state_render_ui(GameState* game);
void game_state_reset(GameState* game);
/////////////////////////////////////////////////////////////////////////////////
