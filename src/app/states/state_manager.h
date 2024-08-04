#pragma once

#include "resources/font.h"
#include "states/game_state.h"
#include "states/state_type.h"
#include "ui/ui_canvas.h"

// StateManager 
/////////////////////////////////////////////////////////////////////////////////
struct StateManger {
  StateType current_state;

  UICanvas* states[STATES_MAX]; 
  GameState game_state;
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