#include "settings_state.h"
#include "audio/audio_system.h"
#include "core/window.h"
#include "defines.h"
#include "resources/font.h"
#include "states/state_type.h"
#include "ui/ui_canvas.h"
#include "core/input.h"
#include "graphics/renderer2d.h"
#include "ui/ui_text.h"
#include "utils/utils_file.h"

#include <string>

// DEFS
/////////////////////////////////////////////////////////////////////////////////
#define MUSIC_VOL_INDEX 0 
#define SOUND_VOL_INDEX 1 
#define SENS_INDEX      2 

#define VALUES_MAX 3
/////////////////////////////////////////////////////////////////////////////////

// SettingsState
/////////////////////////////////////////////////////////////////////////////////
struct SettingsState {
  u32 values[VALUES_MAX]; 
  UICanvas* canvas;

  u32 selector = 1;
};

static SettingsState s_settings;
/////////////////////////////////////////////////////////////////////////////////

// Private functions
/////////////////////////////////////////////////////////////////////////////////
static void apply_settings() {
  audio_system_set_volume(settings_get_sound_volume(), settings_get_music_volume());
  window_set_sensitivity(settings_get_sensitivity());
}
/////////////////////////////////////////////////////////////////////////////////

// Callbacks
/////////////////////////////////////////////////////////////////////////////////
static void settings_button_change_scene(UIButton* button, const UIButtonState button_state, void* user_data) {
  if(button_state != BUTTON_STATE_PRESSED) {
    return;
  }

  StateType* current_state = (StateType*)user_data;
  UIText text = s_settings.canvas->texts[s_settings.selector];

  if(button->text.str == "MENU") {
    *current_state = STATE_MENU; 
  }
  else if(button->text.str == "APPLY") {
    apply_settings(); 
    file_write_binary("settings.bin", s_settings.values, sizeof(s_settings.values));
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void settings_state_create(UICanvas* canvas, const Font* font, StateType* current_state) {
  s_settings.canvas = canvas;

  ui_canvas_push_text(canvas, "<SETTINGS>", 50.0f, glm::vec4(1.0f), UI_ANCHOR_TOP_CENTER, glm::vec2(-5.0f, 20.0f));
  ui_canvas_push_button(canvas, 
                        "MENU", 
                        30.0f, 
                        glm::vec4(1.0f), 
                        glm::vec4(0, 0, 0, 1), 
                        (void*)current_state, 
                        settings_button_change_scene, 
                        UI_ANCHOR_BOTTOM_LEFT, 
                        glm::vec2(0.0f, 10.0f));

  glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  ui_canvas_begin(canvas, glm::vec2(0.0f, 70.0f), UI_ANCHOR_CENTER_LEFT);
  ui_canvas_push_text(canvas, "MUSIC VOLUME: ", 30.0f, color);
  ui_canvas_push_text(canvas, "SOUND EFFECTS VOLUME: ", 30.0f, color);
  ui_canvas_push_text(canvas, "SENSITIVITY: ", 30.0f, color);
  ui_canvas_end(canvas);

  ui_canvas_push_button(canvas, 
                        "APPLY", 
                        30.0f, 
                        glm::vec4(1.0f), 
                        color, 
                        (void*)current_state, 
                        settings_button_change_scene, 
                        UI_ANCHOR_BOTTOM_CENTER, 
                        glm::vec2(0.0f, -120.0f));

  // Setting the default values
  file_read_binary("settings.bin", s_settings.values, sizeof(s_settings.values));
  apply_settings();
}

void settings_state_update() {
  // Move the selector up and down the options
  if(input_key_pressed(KEY_UP)) {
    s_settings.selector--; 
  }
  else if(input_key_pressed(KEY_DOWN)) {
    s_settings.selector++; 
  }

  // Clamp the values between 1 and max texts
  u32 max_texts = s_settings.canvas->texts.size() - 1;
  if(s_settings.selector > max_texts) {
    s_settings.selector = 1;
  }
  else if(s_settings.selector < 1) {
    s_settings.selector = max_texts;
  }

  // Edit the option currently selected
  if(input_key_pressed(KEY_LEFT)) {
    s_settings.values[s_settings.selector - 1]--;
  }
  else if(input_key_pressed(KEY_RIGHT)) {
    s_settings.values[s_settings.selector - 1]++;
  }

  // Clamp the value between 0 and 1
  u32* value = &s_settings.values[s_settings.selector - 1];
  if(*value < 1) {
    *value = 1;
  }
  else if(*value > 10) {
    *value = 10;
  }
}

void settings_state_render() {
  ui_text_set_string(&s_settings.canvas->texts[1], "MUSIC VOLUME: " + std::to_string(s_settings.values[MUSIC_VOL_INDEX]));
  ui_text_set_string(&s_settings.canvas->texts[2], "SOUND EFFECT VOLUME: " + std::to_string(s_settings.values[SOUND_VOL_INDEX]));
  ui_text_set_string(&s_settings.canvas->texts[3], "SENSITIVITY: " + std::to_string(s_settings.values[SENS_INDEX]));
 
  UIText text = s_settings.canvas->texts[s_settings.selector];

  glm::vec2 size = text.size + 15.0f;
  glm::vec2 pos = text.position + glm::vec2(size.x - size.x / 2.0f - 8.0f, 0.0f);

  render_quad(pos, size + 10.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  render_quad(pos, size, glm::vec4(1.0f));

  ui_canvas_render(s_settings.canvas);
}

const f32 settings_get_music_volume() {
  return s_settings.values[MUSIC_VOL_INDEX] * 0.1f;
}

const f32 settings_get_sound_volume() {
  return s_settings.values[SOUND_VOL_INDEX] * 0.1f;
}

const f32 settings_get_sensitivity() {
  return s_settings.values[SENS_INDEX] * 0.01f;
}
/////////////////////////////////////////////////////////////////////////////////
