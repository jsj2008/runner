#pragma once

struct physworld_t;
struct rigidbody_t;
struct world_t;
struct scene_t;
struct camera_t;
struct resman_t;
struct node_t;
struct vec2f_t;
struct game_t;

typedef void (*click_handler_pf)(struct game_t* game, const struct node_t* node, const struct vec2f_t* point, void* user_data);

typedef struct click_handler_t
{
   char node_name[64];
   click_handler_pf handler;
   void* user_data;
} click_handler_t;

typedef struct game_t
{
   struct resman_t* resman;
   struct physworld_t* phys;
   struct world_t* world;
   struct scene_t* scene;
   struct rigidbody_t** bodies;
   struct camera_t* camera;
   struct scene_t* gui;

   unsigned long nclick_handlers;
   click_handler_t click_handlers[64];

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

void game_add_click_handler(game_t* game, const char* node_name, click_handler_pf handler, void* user_data);
void game_dispatch_click(game_t* game, const struct vec2f_t* point);

