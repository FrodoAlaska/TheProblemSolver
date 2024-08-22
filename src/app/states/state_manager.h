#pragma once

#include "resources/cubemap.h"
#include "resources/font.h"
#include "resources/texture.h"
#include "states/game_state.h"
#include "states/state_type.h"
#include "ui/ui_canvas.h"

// StateManager 
/////////////////////////////////////////////////////////////////////////////////
struct StateManger {
  StateType current_state;

  CubeMap* cubemap;
  Texture* title_texture;

  UICanvas* states[STATES_MAX]; 
  GameState game_state;

  u32 startup_timer = 0;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions 
/////////////////////////////////////////////////////////////////////////////////
void state_manager_init(StateManger* state, Font* font);
void state_manager_shutdown(StateManger* state);

void state_manager_update(StateManger* state);
void state_manager_render(StateManger* state);
void state_manager_render_ui(StateManger* state);
/////////////////////////////////////////////////////////////////////////////////
