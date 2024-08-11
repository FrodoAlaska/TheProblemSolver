#include "tasks_menu.h"
#include "audio/audio_system.h"
#include "audio/sound_type.h"
#include "defines.h"
#include "graphics/renderer2d.h"
#include "ui/ui_anchor.h"
#include "ui/ui_canvas.h"
#include "ui/ui_text.h"

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
void task_menu_init(TaskMenu* menu) {
  menu->tasks[0] = Task{100, "pay bar tab"};
  menu->tasks[1] = Task{500, "alimony"};
  // menu->tasks[2] = Task{5000, "stupid dress for janice"};
  // menu->tasks[3] = Task{10000, "that red car"};
  // menu->tasks[4] = Task{12000, "nice ring for alice hooker"};
  // menu->tasks[5] = Task{20000, "jason's expensive college tuition"};
  // menu->tasks[6] = Task{50000, "nice clothes"};
  // menu->tasks[7] = Task{74560, "debt for loan shark"};
  // menu->tasks[8] = Task{100000, "house on river"};

  menu->current_task = 0;
  menu->canvas = ui_canvas_create(renderer2d_get_default_font());

  ui_canvas_begin(menu->canvas, glm::vec2(0.0f, 20.0f), UI_ANCHOR_CENTER);
  for(u32 i = 0; i < TASKS_MAX; i++) {
    ui_canvas_push_text(menu->canvas, std::to_string(menu->tasks[i].cost) + "$: " + menu->tasks[i].desc, 20.0f, glm::vec4(1.0f));
  }
  ui_canvas_end(menu->canvas);

  menu->is_active = false;
  menu->has_completed_all = false;
}

void task_menu_update(TaskMenu* menu, u32* current_balance) {
  // All tasks completed
  if(menu->current_task == TASKS_MAX) {
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

    // Add the strikethrough to indicate the task was completed
    UIText text =  menu->canvas->texts[menu->current_task];
    glm::vec2 text_size = ui_text_measure_size(&text);
    // menu->strikethroughs[menu->current_task] = Rect {
    //   .x = text.position.x + text_size.x / 2.0f,
    //   .y = text.position.y,
    //
    //   .width = text_size.x, 
    //   .height = 2.0f,
    // };

    // Check on the next task next time. 
    menu->current_task++; 

    // Give the player a 10% bonus for completing the task 
    *current_balance += task.cost * 0.1f;
  }
}

void task_menu_render(TaskMenu* menu) {
  // Display task cost and description
  Task task = menu->tasks[menu->current_task];
  std::string task_str = "Task: " + std::to_string(task.cost) + "$ - " + task.desc;
  render_text(20.0f, task_str, glm::vec2(10.0f, 35.0f), glm::vec4(1.0f));

  if(!menu->is_active) {
    return;
  }

  for(u32 i = 0; i < menu->current_task; i++) {
    Rect st = menu->strikethroughs[i];
    render_quad(glm::vec2(st.x, st.y), glm::vec2(st.width, st.height), glm::vec4(1.0f));
  }

  ui_canvas_render(menu->canvas);
}
/////////////////////////////////////////////////////////////////////////////////
