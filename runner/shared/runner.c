#include "runner.h"
#include <game.h>
#include <common.h>
#include <math.h>
#include <stream.h>
#include <camera.h>
#include <shader.h>
#include <material.h>
#include <resman.h>
#include <game.h>
#include <world.h>
#include <material.h>
#include <timestamp.h>
#include <keys.h>
#include <gl_defs.h>

static GLfloat skybox_vertices[] =
{
   -1.0f, -1.0f, 1.0f,
   1.0f, -1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 1.0f,
};

static GLfloat skybox_colors[] =
{
   0.5f, 0.0f, 0.0f,
   0.5f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.0f,
};

static GLubyte skybox_indices[] =
{
   0, 1, 2,
   0, 2, 3,
};

static GLfloat skybox_tex_coords[] =
{
   0.25f, 0.1f,
   0.75f, 0.1f,
   0.75f, 0.9f,
   0.25f, 0.9f,
};

game_t* game = NULL;

vec3f_t* vec3(float x, float y, float z)
{
   static vec3f_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   return &tmp;
}

quat_t* quat(float x, float y, float z, float w)
{
   static quat_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   tmp.w = w;
   return &tmp;
}

static long frames = 0;
static long total_frames = 0;
static timestamp_t prev_time = {0};
static timestamp_t fps_time = {0};
static int done = 0;

void update_control_state(int option, gui_t* gui, control_t* control)
{
   if (control == NULL)
   {
      return;
   }

   int state = CONTROL_NORMAL;
   if (game_is_option_set(game, option))
   {
      state = CONTROL_PRESSED;
   }

   gui_set_control_state(gui, control, state);
}

void toggle_game_option(int option, gui_t* gui, control_t* control)
{
   game_toggle_option(game, option);
   update_control_state(option, gui, control);
}

void update_control(int option, gui_t* gui, const char* name)
{
   control_t* control = gui_get_control(gui, name);
   update_control_state(option, gui, control);
}

int get_game_option(const control_t* control)
{
   if (strcmp(control->name, "GUI_BTN_DrawPhysics") == 0)
   {
      return GAME_DRAW_PHYSICS;
   }
   else if (strcmp(control->name, "GUI_BTN_DrawLamps") == 0)
   {
      return GAME_DRAW_LAMPS;
   }
   else if (strcmp(control->name, "GUI_BTN_DrawMeshes") == 0)
   {
      return GAME_DRAW_MESHES;
   }
   else if (strcmp(control->name, "GUI_BTN_EnablePhysics") == 0)
   {
      return GAME_UPDATE_PHYSICS;
   }
   return 0;
}

void on_gui_action(gui_t* gui, control_t* control, gui_action_t action, const vec2f_t* point, void* user_data)
{
   LOGI("on_gui_action");

   int option = get_game_option(control);

   switch (action)
   {
   case ACTION_DOWN:
   case ACTION_ENTER:
      gui_set_control_state(gui, control, CONTROL_SELECTED);
      break;
   case ACTION_UP:
      toggle_game_option(option, gui, control);
      break;

   case ACTION_LEAVE:
      update_control_state(option, gui, control);
      break;
   default:
      break;
   }
}

void timers_init()
{
   frames = 0;
   total_frames = 0;
   timestamp_set(&prev_time);
   timestamp_set(&fps_time);
}

float timers_update()
{
   ++total_frames;
   ++frames;

   long diff = timestamp_diff(&prev_time, &fps_time);
   if (diff >= 5000)
   {
      timestamp_set(&fps_time);

      LOGI("Total: %4ld Frames: %4ld Diff: %ld FPS: %.2f", total_frames, frames, diff, (float)frames * 1000.0f / (float)diff);
      frames = 0;
   }
   long elapsed = timestamp_update(&prev_time);
   return (float)elapsed / 1000.0f;
}

int init(void* iodata)
{
   LOGI("init");

   done = 0;

   if (stream_init(iodata) != 0)
   {
      LOGE("Error initializing I/O system");
      return -1;
   }

   if (game_init(&game, "levels/w01d01.runner") != 0)
      return -1;

   gui_add_handler(&game->gui, on_gui_action, ACTION_DOWN | ACTION_UP | ACTION_ENTER | ACTION_LEAVE, NULL);
   update_control(GAME_UPDATE_PHYSICS, &game->gui, "GUI_BTN_EnablePhysics");
   update_control(GAME_DRAW_PHYSICS, &game->gui, "GUI_BTN_DrawPhysics");
   update_control(GAME_DRAW_LAMPS, &game->gui, "GUI_BTN_DrawLamps");
   update_control(GAME_DRAW_MESHES, &game->gui, "GUI_BTN_DrawMeshes");

   timers_init();

   return 0;
}

int restore()
{
   LOGI("restore");

   outGLString("Version", GL_VERSION);
   outGLString("Vendor", GL_VENDOR);
   outGLString("Renderer", GL_RENDERER);
   outGLString("Extensions", GL_EXTENSIONS);

   glClearColor(1.0f, 0.1f, 0.1f, 1.0f);
   glEnable(GL_DEPTH_TEST);
   checkGLError("glEnable GL_DEPTH_TEST");

   glEnable(GL_CULL_FACE);
   checkGLError("glEnable GL_CULL_FACE");

   glCullFace(GL_BACK);
   checkGLError("glCullFace");

   glFrontFace(GL_CCW);
   checkGLError("glFrontFace");

   return game_restore(game);
}

void shutdown()
{
   LOGI("shutdown");

   if (game != NULL)
   {
      game_free(game);
      game = NULL;
   }
}

void resize(int width, int height)
{
   LOGI("resize %dx%d", width, height);

   glViewport(0, 0, width, height);
   checkGLError("glViewport");

   game->camera->aspect = (float)width/(float)height;
   glDepthRange(game->camera->znear, game->camera->zfar);
   checkGLError("glDepthRange");
}

void activated()
{
   LOGI("activated");
}

void deactivated()
{
   LOGI("deactivated");
}

void skybox_render()
{
   material_t* mtl = resman_get_material(game->resman, "SkyboxMaterial");
   if (mtl == NULL)
      return;

   shader_t* shader = resman_get_shader(game->resman, mtl->shader);

   material_bind(mtl, 0);
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, 0, skybox_vertices);
   shader_set_attrib_vertices(shader, "aTexCoord", 2, GL_FLOAT, 0, skybox_tex_coords);
   shader_set_attrib_vertices(shader, "aColor", 3, GL_FLOAT, 0, skybox_colors);

   glCullFace(GL_BACK);
   glDepthFunc(GL_ALWAYS);

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, skybox_indices);
   glDepthFunc(GL_LESS);

   material_unbind(mtl);
}

int update()
{
   if (done)
   {
      return 1;
   }

   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   float dt = timers_update();
   game_update(game, dt);

   skybox_render();
   game_render(game);

   glFinish();

   return 0;
}

typedef struct pointer_info_t
{
   enum
   {
      POINTER_INFO_UP = 0,
      POINTER_INFO_DOWN,
   } state;

   float x;
   float y;
   float last_update;
} pointer_info_t;

#define MAX_POINTERS 16

static pointer_info_t pointers[MAX_POINTERS] = {0};

static float get_current_time()
{
   struct timeval cur_time = { 0 };
   gettimeofday(&cur_time, NULL);
   return (float)cur_time.tv_sec + (float)cur_time.tv_usec / 1000000.0f;
}

void pointer_down(int pointerId, float x, float y)
{
   LOGD("pointer #%d down: %.2f %.2f", pointerId, x, y);

   pointer_info_t* pointer = &pointers[pointerId];
   pointer->last_update = get_current_time();
   pointer->state = POINTER_INFO_DOWN;
   pointer->x = x;
   pointer->y = y;

   vec2f_t point =
   {
      .x = x * 2.0f - 1.0f,
      .y = 1.0f - y * 2.0f,
   };

   gui_dispatch_pointer_down(&game->gui, pointerId, &point);
}

void pointer_up(int pointerId, float x, float y)
{
   LOGD("pointer #%d up: %.2f %.2f", pointerId, x, y);

   pointer_info_t* pointer = &pointers[pointerId];
   pointer->last_update = get_current_time();
   pointer->state = POINTER_INFO_UP;
   pointer->x = x;
   pointer->y = y;

   vec2f_t point =
   {
      .x = x * 2.0f - 1.0f,
      .y = 1.0f - y * 2.0f,
   };
   gui_dispatch_pointer_up(&game->gui, pointerId, &point);
}

void pointer_move(int pointerId, float x, float y)
{
   LOGD("pointer #%d move: %.2f %.2f", pointerId, x, y);

   float current_time = get_current_time();
   pointer_info_t* pointer = &pointers[pointerId];
   if (pointer->state == POINTER_INFO_DOWN)
   {
      vec4f_t from =
      {
         .x = pointer->x * 2.0f - 1.0f,
         .y = 1.0f - pointer->y * 2.0f,
         .z = 0.99f,
         .w = 1.0f,
      };

      vec4f_t to =
      {
         .x = x * 2.0f - 1.0f,
         .y = 1.0f - y * 2.0f,
         .z = 0.99f,
         .w = 1.0f,
      };

      camera_t* camera = game->camera;

      mat4f_t transform = {0};
      mat4_mult(&transform, &camera->proj, &camera->view);
      mat4_invert(&transform);

      vec4f_t from_scene = {0};
      vec4f_t to_scene = {0};
      mat4_mult_vec4(&from_scene, &transform, &from);
      mat4_mult_vec4(&to_scene, &transform, &to);

      from_scene.w = 1.0f / from_scene.w;
      from_scene.x *= from_scene.w;
      from_scene.y *= from_scene.w;
      from_scene.z *= from_scene.w;

      to_scene.w = 1.0f / to_scene.w;
      to_scene.x *= to_scene.w;
      to_scene.y *= to_scene.w;
      to_scene.z *= to_scene.w;

      vec3f_t diff = { to_scene.x - from_scene.x, to_scene.y - from_scene.y, to_scene.z - from_scene.z };

      mat4f_t rotation = camera->view;
      rotation.m14 = 0.0f;
      rotation.m24 = 0.0f;
      rotation.m34 = 0.0f;

      vec3f_t diff2;
      mat4_mult_vec3(&diff2, &rotation, &diff);

      camera->view.m14 += diff2.x;
      camera->view.m24 += diff2.y;
      camera->view.m34 += diff2.z;
   }
   pointer->last_update = current_time;
   pointer->x = x;
   pointer->y = y;

   vec2f_t point =
   {
      .x = x * 2.0f - 1.0f,
      .y = 1.0f - y * 2.0f,
   };
   gui_dispatch_pointer_move(&game->gui, pointerId, &point);
}

void key_up(int key)
{
   LOGI("Key up: %d", key);

   if (key == KEY_BACK)
   {
      done = 1;
   }
}

void key_down(int key)
{
   LOGI("Key down: %d", key);
}

