#pragma once

struct physworld_t;
struct rigidbody_t;
struct world_t;
struct scene_t;
struct camera_t;
struct resman_t;

typedef struct game_t
{
   struct resman_t* resman;
   struct physworld_t* phys;
   struct world_t* world;
   struct scene_t* scene;
   struct rigidbody_t** bodies;
   struct camera_t* camera;
} game_t;

int game_init(game_t** pgame, const char* fname);
void game_free();
void game_update(game_t* game, float dt);
void game_render(const game_t* game);
void game_set_scene(game_t* game, const char* scene);

