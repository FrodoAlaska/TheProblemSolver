#pragma once

#include "defines.h"
#include "resources/font.h"
#include "states/state_type.h"
#include "ui/ui_canvas.h"

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void settings_state_create(UICanvas* canvas, const Font* font, StateType* current_state);
void settings_state_update();
void settings_state_render();

const f32 settings_get_music_volume();
const f32 settings_get_sound_volume();
const f32 settings_get_sensitivity();
/////////////////////////////////////////////////////////////////////////////////
