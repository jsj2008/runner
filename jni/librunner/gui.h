#pragma once

#include "mathlib.h"
#include "camera.h"

struct gui_t;
struct control_t;

typedef void (*gui_click_handler)(struct gui_t* gui, struct control_t* control, const vec2f_t* point, void* user_data);

struct button_t
{
   char normal[64];
   char pressed[64];
   char toggled[64];
   char disabled[64];
};

struct label_t
{
   char text[256];
};

typedef struct control_t
{
   char name[64];

   vec2f_t position;
   vec2f_t size;

   enum
   {
      CONTROL_BUTTON,
      CONTROL_LABEL,
   } type;

   enum
   {
      STATE_NONE = 0,
      STATE_ENABLED = (1<<0),
      STATE_VISIBLE = (1<<1),
      STATE_PRESSED = (1<<2),
      STATE_TOGGLED = (1<<3),
      STATE_NORMAL = STATE_ENABLED | STATE_VISIBLE,
   } state;

   union
   {
      struct button_t button;
      struct label_t label;
   };
} control_t;

typedef struct gui_t
{
   long ncontrols;
   struct control_t* controls;

   long nclick_handlers;
   struct click_handler_t
   {
      char control_name[64];
      gui_click_handler handler;
      void* user_data;
   } *click_handlers;

   camera_t camera;
} gui_t;

int gui_init(gui_t** pgui);
void gui_free(gui_t* gui);
void gui_add_control(gui_t* gui, const control_t* control);
void gui_render(const gui_t* gui);
int gui_dispatch_click(gui_t* gui, const vec2f_t* point);
int gui_set_click_handler(gui_t* gui, const char* control_name, gui_click_handler handler, void* user_data);

