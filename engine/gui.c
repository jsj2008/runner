#include "gui.h"
#include "game.h"
#include "world.h"
#include "common.h"
#include "resman.h"

extern game_t* game;

pointer_t* gui_get_pointer_by_id(gui_t* gui, long pointer_id)
{
   if (pointer_id >= MAX_POINTERS)
   {
      LOGE("Pointer_id (%ld) is greater than MAX_POINTERS (%d)", pointer_id, MAX_POINTERS);
      return NULL;
   }

   pointer_t* pointer = &gui->pointers[pointer_id];
   return pointer;
}

void gui_invoke_handlers(gui_t* gui, control_t* control, gui_action_t action, const vec2f_t* point)
{
   long l = 0;
   const handler_t* handler = &gui->handlers[0];
   for (l = 0; l < gui->nhandlers; ++l, ++handler)
   {
      if ((handler->action_filter & action) != 0)
      {
         (*handler->handler)(gui, control, action, point, handler->user_data);
      }
   }
}

void gui_reset(gui_t* gui)
{
   memset(gui, 0, sizeof(gui_t));
}

void gui_add_handler(gui_t* gui, handler_pf handler, long action_filter, void* user_data)
{
   struct handler_t* h = &gui->handlers[gui->nhandlers++];
   h->handler = handler;
   h->action_filter = action_filter;
   h->user_data = user_data;
}

void gui_set_control_state(gui_t* gui, control_t* control, control_state_t state)
{
   node_t* node = (node_t*)control;

   mesh_t* mesh = world_get_mesh(game->world, node->data);
   if (mesh != NULL)
   {
      mesh->active_uvmap = (int)state;
   }
}

void gui_dispatch_pointer_down(gui_t* gui, long pointer_id, const vec2f_t* point)
{
   pointer_t* pointer = gui_get_pointer_by_id(gui, pointer_id);
   if (pointer == NULL) return;

   pointer->state = POINTER_DOWN;
   pointer->point = *point;
   pointer->control = (control_t*)scene_pick_node(game->world, gui->scene, &pointer->point);

   if (pointer->control != NULL)
   {
      LOGI("POINTER DOWN ON %s", pointer->control->name);
      gui_invoke_handlers(gui, pointer->control, ACTION_DOWN, &pointer->point);
   }
}

void gui_dispatch_pointer_up(gui_t* gui, long pointer_id, const struct vec2f_t* point)
{
   pointer_t* pointer = gui_get_pointer_by_id(gui, pointer_id);
   if (pointer == NULL) return;

   pointer->state = POINTER_UP;
   pointer->point = *point;
   pointer->control = (control_t*)scene_pick_node(game->world, gui->scene, &pointer->point);

   if (pointer->control != NULL)
   {
      LOGI("POINTER UP ON %s", pointer->control->name);
      gui_invoke_handlers(gui, pointer->control, ACTION_UP, &pointer->point);
   }
}

void gui_dispatch_pointer_move(gui_t* gui, long pointer_id, const struct vec2f_t* point)
{
   pointer_t* pointer = gui_get_pointer_by_id(gui, pointer_id);
   if (pointer == NULL) return;

   control_t* old_control = pointer->control;

   pointer->point = *point;
   pointer->control = (control_t*)scene_pick_node(game->world, gui->scene, &pointer->point);

   if (old_control != pointer->control)
   {
      if (old_control != NULL)
      {
         LOGI("POINTER LEAVED FROM %s", old_control->name);
         gui_invoke_handlers(gui, old_control, ACTION_LEAVE, &pointer->point);
      }

      if (pointer->control != NULL)
      {
         LOGI("POINTER ENTERED TO %s", pointer->control->name);
         gui_invoke_handlers(gui, pointer->control, ACTION_ENTER, &pointer->point);
      }
   }

   if (pointer->control != NULL)
   {
      LOGI("POINTER MOVED OVER %s", pointer->control->name);
      gui_invoke_handlers(gui, pointer->control, ACTION_MOVE, &pointer->point);
   }
}

control_t* gui_get_control(gui_t* gui, const char* name)
{
   return (control_t*)scene_get_node(gui->scene, name);
}

