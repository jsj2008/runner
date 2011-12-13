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

typedef struct pointer_info_t
{
   enum
   {
      POINTER_INFO_UP = 0,
      POINTER_INFO_DOWN,
   } state;

   float x;
   float y;
   timestamp_t last_update;
} pointer_info_t;

typedef struct key_info_t
{
   enum
   {
      KEY_INFO_UP = 0,
      KEY_INFO_DOWN,
   } state;

   timestamp_t last_update;
} key_info_t;

#define MAX_POINTERS 16
#define MAX_KEYS 256

static pointer_info_t pointers[MAX_POINTERS] = {0};
static key_info_t keys[MAX_KEYS] = {0};

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

   const float speed = 1.0f;
   camera_t* camera = game->camera;
   mat4f_t view = camera->view;

   vec3f_t right = { view.m11, view.m12, view.m13 };
   vec3f_t up = { view.m21, view.m22, view.m23 };
   vec3f_t forward = { view.m31, view.m32, view.m33 };
   vec3f_t origin_enc = { view.m14, view.m24, view.m34 };
   vec3_normalize(&right);
   vec3_normalize(&up);
   vec3_normalize(&forward);

   vec3f_t movement = {0};
   if (keys[KEY_UP].state == KEY_INFO_DOWN)
   {
      vec3_add(&movement, &movement, &forward);
   }
   if (keys[KEY_DOWN].state == KEY_INFO_DOWN)
   {
      vec3_sub(&movement, &movement, &forward);
   }
   if (keys[KEY_RIGHT].state == KEY_INFO_DOWN)
   {
      vec3_sub(&movement, &movement, &right);
   }
   if (keys[KEY_LEFT].state == KEY_INFO_DOWN)
   {
      vec3_add(&movement, &movement, &right);
   }

   mat4f_t translation = {0};
   mat4_set_identity(&translation);
   translation.m14 = movement.x;
   translation.m24 = movement.y;
   translation.m34 = movement.z;

   mat4_mult(&camera->view, &view, &translation);

   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   float dt = timers_update();
   game_update(game, dt);

   skybox_render();
   game_render(game);

   glFinish();

   return 0;
}

void pointer_down(int pointerId, float x, float y)
{
   LOGD("pointer #%d down: %.2f %.2f", pointerId, x, y);

   pointer_info_t* pointer = &pointers[pointerId];
   timestamp_set(&pointer->last_update);
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
   timestamp_set(&pointer->last_update);
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

   pointer_info_t* pointer = &pointers[pointerId];
   long elapsed = timestamp_update(&pointer->last_update);
   if (pointer->state == POINTER_INFO_DOWN)
   {
      camera_t* camera = game->camera;

      mat4f_t transform = {0};
      mat4_mult(&transform, &camera->proj, &camera->view);

      vec4f_t origin = { 0.0f, 0.0f, 0.0f, 1.0f };
      vec4f_t origin_screen = {0};
      mat4_mult_vec4(&origin_screen, &transform, &origin);
      origin_screen.w = 1.0f / origin_screen.w;
      origin_screen.x *= origin_screen.w;
      origin_screen.y *= origin_screen.w;
      origin_screen.z *= origin_screen.w;

      //LOGI("origin: [%.2f %.2f %.2f %.2f]", origin_screen.x, origin_screen.y, origin_screen.z, origin_screen.w);

      vec4f_t from =
      {
         .x = pointer->x * 2.0f - 1.0f,
         .y = 1.0f - pointer->y * 2.0f,
         .z = origin_screen.z,
         .w = 1.0f,
      };

      vec4f_t to =
      {
         .x = x * 2.0f - 1.0f,
         .y = 1.0f - y * 2.0f,
         .z = origin_screen.z,
         .w = 1.0f,
      };

      vec4f_t from_scene = {0};
      vec4f_t to_scene = {0};

      mat4_invert(&transform);
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

      mat4f_t translation = {0};
      mat4_set_identity(&translation);
      translation.m14 = to_scene.x - from_scene.x;
      translation.m24 = to_scene.y - from_scene.y;
      translation.m34 = to_scene.z - from_scene.z;

      mat4f_t view = camera->view;
      mat4_mult(&camera->view, &view, &translation);
   }
   pointer->x = x;
   pointer->y = y;

   vec2f_t point =
   {
      .x = x * 2.0f - 1.0f,
      .y = 1.0f - y * 2.0f,
   };
   gui_dispatch_pointer_move(&game->gui, pointerId, &point);
}

void key_up(int key_code)
{
   LOGI("Key up: %d", key_code);

   key_info_t* key = &keys[key_code];
   timestamp_set(&key->last_update);
   key->state = KEY_INFO_UP;

   if (key_code == KEY_BACK)
   {
      done = 1;
   }
}

void key_down(int key_code)
{
   LOGI("Key down: %d", key_code);

   key_info_t* key = &keys[key_code];
   timestamp_set(&key->last_update);
   key->state = KEY_INFO_DOWN;
}

