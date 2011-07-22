#pragma once

#define MAX_POINTERS 16
#define MAX_HANDLERS 256

#include "mathlib.h"

struct node_t;
struct scene_t;
struct gui_t;

typedef struct node_t control_t;

typedef enum gui_action_t
{
   ACTION_NONE    = (0),
   ACTION_DOWN    = (1<<0),
   ACTION_UP      = (1<<1),
   ACTION_CLICK   = (1<<2),
   ACTION_MOVE    = (1<<3),
   ACTION_ENTER   = (1<<4),
   ACTION_LEAVE   = (1<<5),
} gui_action_t;

typedef enum control_state_t
{
   CONTROL_NORMAL = 0,
   CONTROL_PRESSED,
   CONTROL_DISABLED,
   CONTROL_SELECTED,
} control_state_t;

typedef void (*handler_pf)(struct gui_t* gui, control_t* control, gui_action_t action, const struct vec2f_t* point, void* user_data);

typedef struct handler_t
{
   handler_pf handler;
   long action_filter;
   void* user_data;
} handler_t;

typedef struct pointer_t
{
   long id;
   struct vec2f_t point;
   control_t* control;

   enum pointer_state_t
   {
      POINTER_UP = 0,
      POINTER_DOWN,
   } state;
} pointer_t;

typedef struct gui_t
{
   long nhandlers;

   struct scene_t* scene;
   struct handler_t handlers[MAX_HANDLERS];
   struct pointer_t pointers[MAX_POINTERS];
} gui_t;

void gui_reset(gui_t* gui);
void gui_add_handler(gui_t* gui, handler_pf handler, long action_filter, void* user_data);
void gui_set_control_state(gui_t* gui, control_t* control, control_state_t state);
void gui_dispatch_pointer_down(gui_t* gui, long pointer_id, const struct vec2f_t* point);
void gui_dispatch_pointer_up(gui_t* gui, long pointer_id, const struct vec2f_t* point);
void gui_dispatch_pointer_move(gui_t* gui, long pointer_id, const struct vec2f_t* movement);
control_t* gui_get_control(gui_t* gui, const char* name);

