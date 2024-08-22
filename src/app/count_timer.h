#pragma once

#include "defines.h"
#include "ui/ui_text.h"

#include <glm/glm.hpp>

#include <string>

// CountTimer
/////////////////////////////////////////////////////////////////////////////////
struct CountTimer {
  i32 seconds, minutes;
  i32 initial_seconds, initial_minutes;
  
  i32 frame_counter;

  bool can_count, is_countdown; 

  UIText timer_text;
};
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void count_timer_create(CountTimer* timer, const i32 start_secs, const i32 start_mins, const bool countdown);
void count_timer_update(CountTimer* timer);
void count_timer_render(CountTimer* timer);
void count_timer_reset(CountTimer* timer);

void count_timer_increase(CountTimer* timer, const i32 secs);
void count_timer_decrease(CountTimer* timer, const i32 secs);
const std::string count_timer_to_str(CountTimer* timer);
const bool count_timer_has_runout(CountTimer* timer);
/////////////////////////////////////////////////////////////////////////////////
