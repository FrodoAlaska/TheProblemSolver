#include "tasks_menu.h"
#include "audio/audio_system.h"
#include "audio/sound_type.h"
#include "core/window.h"
#include "defines.h"
#include "graphics/renderer2d.h"
#include "ui/ui_anchor.h"
#include "ui/ui_text.h"

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void task_menu_init(TaskMenu* menu) {
  menu->tasks[0] = Task{850, "pay bar tab"};
  menu->tasks[1] = Task{1500, "stupid dress for janice"};
  menu->tasks[2] = Task{3800, "useless alimony"};
  menu->tasks[3] = Task{6200, "that nice carriage"};
  menu->tasks[4] = Task{9700, "nice ring for alice the hooker"};
  menu->tasks[5] = Task{15000, "jason's expensive college tuition"};
  menu->tasks[6] = Task{25200, "debt for the fat loan shark"};
  menu->tasks[7] = Task{54570, "That house on the river"};

  menu->current_task = 0;
  menu->board_size = glm::vec2(0.0f, 0.0f);

  Font* font = renderer2d_get_default_font();

  f32 longest_text = 0.0f;
    
  ui_text_create(&menu->texts[0],
                 font,
                 "TASKS", 
                 40.0f, 
                 UI_ANCHOR_TOP_CENTER, 
                 glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 
                 glm::vec2(0.0f, 80.0f));

  for(u32 i = 1; i < TEXTS_MAX; i++) {
    ui_text_create(&menu->texts[i],
                   font,
                   std::to_string(menu->tasks[i - 1].cost) + "$: " + menu->tasks[i - 1].desc, 
                   25.0f, 
                   UI_ANCHOR_CENTER, 
                   glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 
                   glm::vec2(0.0f, i * 40.0f));
    menu->texts[i].position.y -= 230.0f; 

    UIText text = menu->texts[i];
    glm::vec2 text_size = ui_text_measure_size(&text);

    menu->strikethroughs[i - 1] = Rect {
      .x = text.position.x + text_size.x / 2.0f,
      .y = text.position.y,

      .width = text_size.x, 
      .height = 4.0f,
    };
    

    if(text_size.x > longest_text) {
      longest_text = text_size.x;
    }

    menu->board_size.y += text_size.y;  
  }

  menu->board_size.y += 270.0f;
  menu->board_size.x = longest_text + 35.0f;

  menu->is_active = false;
  menu->has_completed_all = false;
}

void task_menu_update(TaskMenu* menu, u32* current_balance) {
  // All tasks completed
  if(menu->current_task >= TASKS_MAX) {
    menu->has_completed_all = true;
    return;
  }
  
  // Get the current task
  Task task = menu->tasks[menu->current_task];

  // The current task can be completed
  if(*current_balance >= task.cost) {
    *current_balance -= task.cost;

    // Task complete sound
    audio_system_play(SOUND_TASK_COMPLETE, 1.0f);

    // Check on the next task next time. 
    menu->current_task++; 

    // Give the player a 10% bonus for completing the task 
    *current_balance += task.cost * 0.1f;
  }
}

void task_menu_render(TaskMenu* menu) {
  if(menu->current_task >= TASKS_MAX) {
    return;
  }

  // Display task cost and description
  Task task = menu->tasks[menu->current_task];
  std::string task_str = "Task: " + std::to_string(task.cost) + "$ - " + task.desc;
  render_text(25.0f, task_str, glm::vec2(10.0f, 45.0f), glm::vec4(1.0f));
  
  render_text(25.0f, "[T] TASKS", glm::vec2(10.0f, 70.0f), glm::vec4(1.0f));
  
  if(!menu->is_active) {
    return;
  }
  
  f32 board_x = window_get_size().x / 2.0f;
  f32 board_y = menu->texts[0].position.y + menu->board_size.y / 2.0f - 50.0f;
  
  render_quad(glm::vec2(board_x, board_y), menu->board_size + 8.0f, glm::vec4(0.0f, 0.1f, 0.0f, 1.0f));
  render_quad(glm::vec2(board_x, board_y), menu->board_size, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f));

  for(u32 i = 0; i < TEXTS_MAX; i++) {
    ui_text_render(&menu->texts[i]);
  }
  
  for(u32 i = 0; i < menu->current_task; i++) {
    Rect st = menu->strikethroughs[i];
    render_quad(glm::vec2(st.x, st.y), glm::vec2(st.width, st.height), glm::vec4(1.0f));
  }
}

void task_menu_reset(TaskMenu* menu) {
  menu->current_task = 0; 
  menu->is_active = false;
  menu->has_completed_all = false;
}
/////////////////////////////////////////////////////////////////////////////////
