#pragma once

#include "gui.h"

struct physworld_t;
struct rigidbody_t;
struct world_t;
struct scene_t;
struct camera_t;
struct resman_t;
struct node_t;
struct vec2f_t;
struct game_t;

typedef struct game_t
{
   struct resman_t* resman;
   struct physworld_t* phys;
   struct world_t* world;
   struct scene_t* scene;
   struct rigidbody_t** bodies;
   struct camera_t* camera;
   struct gui_t gui;

   enum option_t
   {
      GAME_DRAW_MESHES = (1<<0),
      GAME_DRAW_CAMERAS = (1<<1),
      GAME_DRAW_LAMPS = (1<<2),
      GAME_DRAW_PHYSICS = (1<<3),
      GAME_UPDATE_PHYSICS = (1<<4),
   } game_options;
} game_t;

int game_init(game_t** pgame, const char* fname);
void game_free();
void game_update(game_t* game, float dt);
void game_render(const game_t* game);
void game_render_scene(const struct game_t* game, const struct scene_t* scene, const struct camera_t* camera);
void game_set_scene(game_t* game, const char* scene);
int game_is_option_set(const game_t* game, int option);
void game_set_option(game_t* game, int option);
void game_reset_option(game_t* game, int option);
void game_toggle_option(game_t* game, int option);

