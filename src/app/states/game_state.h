#pragma once

#include "defines.h"
#include "entities/target.h"
#include "graphics/camera.h"
#include "resources/texture.h"
#include "states/state_type.h"
#include "tasks_menu.h"
#include "count_timer.h"
#include "target_spawner.h"
#include "entities/object.h"
#include "hit_manager.h"
#include "ui/ui_button.h"
#include "ui/ui_text.h"

#include <vector>

// GameState
/////////////////////////////////////////////////////////////////////////////////
struct GameState {
  Camera camera;

  std::vector<Object*> objects;
  std::vector<Target*> targets;

  TargetSpawner target_spawner;
  CountTimer timer;
  HitManager hit_manager;
  TaskMenu task_menu;

  u32 score = 0;

  bool is_paused; 
  UIText pause_text;
  UIButton menu_button;
  
  Texture* crosshair;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
void game_state_init(GameState* game);
void game_state_update(GameState* game, StateType* current_state);
void game_state_render(GameState* game);
void game_state_render_ui(GameState* game);
void game_state_reset(GameState* game);
/////////////////////////////////////////////////////////////////////////////////
