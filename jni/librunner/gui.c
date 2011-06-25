#include "gui.h"
#include "common.h"
#include "game.h"
#include "resman.h"
#include "shader.h"
#include "world.h"

extern game_t* game;

/*static int point_in_rect(const vec2f_t* point, const vec2f_t* pos, const vec2f_t* size)
{
   if (point->x < pos->x || point->y < pos->y || point->x > pos->x + size->x || point->y > pos->y + size->y)
   {
      return 0;
   }
   return 1;
}*/

static void test_button_click_handler(gui_t* gui, control_t* control, const vec2f_t* point, void* user_data)
{
   LOGI("TEST BUTTON CLICKED");
}

int gui_init(gui_t** pgui)
{
   gui_t* gui = (gui_t*)malloc(sizeof(gui_t) + sizeof(control_t) * 32 + sizeof(struct click_handler_t) * 32);
   memset(gui, 0, sizeof(gui_t));
   gui->controls = (control_t*)((char*)gui + sizeof(gui_t));
   gui->click_handlers = (struct click_handler_t*)((char*)gui->controls + sizeof(control_t) * 32);

   mat4_set_identity(&gui->camera.view);
   mat4_set_orthographic(&gui->camera.proj, 0.0f, 2.0f, 0.0f, 2.0f, 0.0f, 200.0f);

   gui->camera.view.m14 = 0.5f;
   gui->camera.view.m24 = 0.5f;
   gui->camera.view.m34 = -10.0f;

   control_t test_button;
   memset(&test_button, 0, sizeof(test_button));
   test_button.type = CONTROL_BUTTON;
   test_button.state = STATE_NORMAL;
   strcpy(test_button.name, "btnTest");
   strcpy(test_button.button.normal, "textures/btn_normal");
   strcpy(test_button.button.pressed, "textures/btn_pressed");
   strcpy(test_button.button.disabled, "textures/btn_disabled");
   test_button.position.x = 0.05f;
   test_button.position.y = 0.05f;
   test_button.size.x = 0.25f;
   test_button.size.y = 0.25f;
   gui_add_control(gui, &test_button);
   gui_set_click_handler(gui, "btnTest", test_button_click_handler, NULL);

   (*pgui) = gui;
   return 0;
}

void gui_free(gui_t* gui)
{
   free(gui);
}

void gui_add_control(gui_t* gui, const control_t* control)
{
   gui->controls[gui->ncontrols] = *control;
   ++gui->ncontrols;
}

static vec2f_t vec2(float x, float y)
{
   vec2f_t v =
   {
      .x = x,
      .y = y,
   };
   return v;
}

static vec3f_t vec3(float x, float y, float z)
{
   vec3f_t v =
   {
      .x = x,
      .y = y,
      .z = z,
   };
   return v;
}

void add_vertex(vertex_t* vertices, long* pnvertices, vec3f_t point, vec2f_t uv)
{
   long nvertices = *pnvertices;
   vertices[nvertices].point = point;
   vertices[nvertices].normal = vec3(0.0f, 0.0f, -1.0f);
   vertices[nvertices].uv = uv;
   (*pnvertices) = ++nvertices;
}

void button_render(const control_t* control, vertex_t* vertices, long* pnvertices)
{
   add_vertex(vertices, pnvertices, vec3(control->position.x, control->position.y, 0.0f), vec2(0.0f, 0.0f));
   add_vertex(vertices, pnvertices, vec3(control->position.x + control->size.x, control->position.y, 0.0f), vec2(1.0f, 0.0f));
   add_vertex(vertices, pnvertices, vec3(control->position.x + control->size.x, control->position.y + control->size.y, 0.0f), vec2(1.0f, 1.0f));
   add_vertex(vertices, pnvertices, vec3(control->position.x, control->position.y, 0.0f), vec2(0.0f, 0.0f));
   add_vertex(vertices, pnvertices, vec3(control->position.x + control->size.x, control->position.y + control->size.y, 0.0f), vec2(1.0f, 1.0f));
   add_vertex(vertices, pnvertices, vec3(control->position.x, control->position.y + control->size.y, 0.0f), vec2(0.0f, 1.0f));
}

void gui_render(const gui_t* gui)
{
   long nvertices = 0;
   vertex_t vertices[2048];

   long l = 0;
   const struct control_t* control = &gui->controls[0];
   for (; l < gui->ncontrols; ++l, ++control)
   {
      if ((control->state & STATE_VISIBLE) == 0)
         continue;

      switch (control->type)
      {
      case CONTROL_BUTTON:
         button_render(control, vertices, &nvertices);
         break;

      case CONTROL_LABEL:
         // TODO
         break;
      }
   }

   material_t* mat = resman_get_material(game->resman, "ButtonMaterial");
   if (mat == NULL)
      return;

   shader_t* shader = resman_get_shader(game->resman, mat->shader);
   if (shader == NULL)
      return;

   mat4f_t mvp;
   mat4_mult(&mvp, &gui->camera.proj, &gui->camera.view);

   material_bind(mat, 0);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, sizeof(vertex_t), &vertices[0].point);
   //shader_set_attrib_vertices(shader, "aNormal", 3, GL_FLOAT, sizeof(vertex_t), &vertices[0].normal);
   shader_set_attrib_vertices(shader, "aTexCoord", 2, GL_FLOAT, sizeof(vertex_t), &vertices[0].uv);
   glDrawArrays(GL_TRIANGLES, 0, nvertices);

   glDisable(GL_BLEND);
   material_unbind(mat);
}

int gui_dispatch_click(gui_t* gui, const vec2f_t* point)
{
   long l = 0;
   long k = 0;

   mat4f_t transform;
   mat4_mult(&transform, &gui->camera.proj, &gui->camera.view);
   mat4_invert(&transform);

   vec3f_t from = { .x = point->x, .z = -99999.0f, .y = point->y };
   vec3f_t to = { .x = point->x, .z = 99999.0f, .y = point->y };
   vec3f_t from_global;
   vec3f_t to_global;

   mat4_mult_vec3(&from_global, &transform, &from);
   mat4_mult_vec3(&to_global, &transform, &to);

   vec3f_t dir;
   vec3_sub(&dir, &to_global, &from_global);
   vec3_normalize(&dir);

   LOGI("FROM: %.2f %.2f %.2f -> %.2f %.2f %.2f", from.x, from.y, from.z, from_global.x, from_global.y, from_global.z);

   struct control_t* control = &gui->controls[0];
   for (; l < gui->ncontrols; ++l, ++control)
   {
      bbox_t bbox;
      bbox.min.x = control->position.x;
      bbox.min.y = control->position.y;
      bbox.min.z = -1.0f;

      bbox.max.x = control->position.x + control->size.x;
      bbox.max.y = control->position.y + control->size.y;
      bbox.max.z = 1.0f;

      float dist = 0.0f;
      if (bbox_ray_intersection(&bbox, &from_global, &dir, &dist))
      {
         for (k = 0; k < gui->nclick_handlers; ++k)
         {
            if (strcmp(control->name, gui->click_handlers[k].control_name) == 0)
            {
               (*gui->click_handlers[k].handler)(gui, control, point, gui->click_handlers[k].user_data);
            }
         }
         return 0;
      }
   }
   return -1;
}

int gui_set_click_handler(gui_t* gui, const char* control_name, gui_click_handler handler, void* user_data)
{
   long l = 0;
   struct control_t* control = &gui->controls[0];
   for (; l < gui->ncontrols; ++l, ++control)
   {
      if (strcmp(control->name, control_name) == 0)
      {
         strcpy(gui->click_handlers[gui->nclick_handlers].control_name, control_name);
         gui->click_handlers[gui->nclick_handlers].handler = handler;
         gui->click_handlers[gui->nclick_handlers].user_data = user_data;
         ++gui->nclick_handlers;
         LOGI("Click handler for control '%s' has been successfully set", control_name);
         return 0;
      }
   }
   LOGE("Unable to set click handler. Control '%s' not found", control_name);
   return -1;
}

