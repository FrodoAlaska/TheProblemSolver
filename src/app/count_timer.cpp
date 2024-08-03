#include "count_timer.h"
#include "defines.h"
#include "engine/core/clock.h"
#include "ui/ui_anchor.h"
#include "ui/ui_text.h"

#include <glm/glm.hpp>

#include <string>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define FRAME_IN_SEC 60
#define SEC_IN_MIN   60 
#define MIN_IN_HOUR  60
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void seconds_tick(CountTimer* timer) {
  if(timer->is_countdown) {
    count_timer_decrease(timer, 1);
  }
  else {
    count_timer_increase(timer, 1);
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void count_timer_create(CountTimer* timer, const i32 start_secs, const i32 start_mins, const i32 start_hours, const bool countdown) {
  timer->seconds = start_secs; 
  timer->minutes = start_mins;
  timer->hours   = start_hours; 
  timer->frame_counter = 0; 

  timer->can_count = true;
  timer->is_countdown = countdown;

  ui_text_create(&timer->timer_text, nullptr, count_timer_to_str(timer), 25.0f, UI_ANCHOR_TOP_CENTER, glm::vec4(1.0f));
}

void count_timer_update(CountTimer* timer) {
  if(!timer->can_count) {
    return;
  }

  timer->frame_counter++;

  i64 frame_in_sec = (i64)glm::floor(gclock_fps());
  if(frame_in_sec == 0) { // Trying to avoid division by 0
    return;
  }

  // Increasing the seconds everytime we reach the target frames (for example every 60 frames)
  if((timer->frame_counter % frame_in_sec) == 0) {
    seconds_tick(timer);
  }

  // Increasing the mintutes every 60 seconds
  if(timer->seconds == SEC_IN_MIN) {
    timer->minutes++;
    timer->seconds = 0;
  }

  // Increasing the hours every 60 minutes
  if(timer->minutes == MIN_IN_HOUR) {
    timer->hours++;
    timer->minutes = 0;
  }
}

void count_timer_render(CountTimer* timer) {
  ui_text_set_string(&timer->timer_text, count_timer_to_str(timer));
  ui_text_render(&timer->timer_text);
}

void count_timer_reset(CountTimer* timer) {
  timer->seconds = 0; 
  timer->minutes = 0;
  timer->hours = 0; 
  timer->frame_counter = 0;
}

void count_timer_increase(CountTimer* timer, const i32 secs) {
  // Seconds can't be over 60
  if((timer->seconds + secs) > SEC_IN_MIN) {
    // Getting the overall total seconds
    u32 over_secs = timer->seconds + secs; 

    // Getting the extra added seconds
    over_secs -= SEC_IN_MIN; 

    // Adding a minute to the timer and adding the extra seconds left over
    timer->seconds = 0; 
    timer->seconds += over_secs; 
    timer->minutes++;

    return;
  }

  timer->seconds += secs;
}

void count_timer_decrease(CountTimer* timer, const i32 secs) {

  // You can't remove more than the current seconds without 
  // decreasing the minutes as well
  if(timer->minutes > 0 && secs > timer->seconds) {
    i32 diff_secs = secs - timer->seconds;
    // i32 total_secs = secs - diff_secs;

    timer->seconds = SEC_IN_MIN - diff_secs;
    timer->minutes--;

    return;
  }

  timer->seconds -= secs;

  // Clamp the seconds to 0
  if(timer->seconds < 0) {
    timer->seconds = 0;
  }
}

const std::string count_timer_to_str(CountTimer* timer) {
  std::string zero_min = "0"; 
  std::string zero_sec = "0"; 

  if(timer->minutes > 9) {
    zero_min = "";
  }

  if(timer->seconds > 9) {
    zero_sec = "";
  }

  return zero_min + std::to_string(timer->minutes) + ":" + 
  zero_sec + std::to_string(timer->seconds);
}
/////////////////////////////////////////////////////////////////////////////////
