#include "game.h"
#include "physworld.h"
#include "world.h"
#include "shader.h"
#include "common.h"
#include "resman.h"
#include "gui.h"

void game_update(struct game_t* game, float dt)
{
   if (game_is_option_set(game, GAME_UPDATE_PHYSICS))
   {
      physworld_update(game->phys, dt);
   }
}

void game_render(const struct game_t* game)
{
   long l = 0;
   struct node_t* node = &game->scene->nodes[0];
   for (l = 0; l < game->scene->nnodes; ++l, ++node)
   {
      switch (node->type)
      {
      case NODE_MESH:
      {
         if (game_is_option_set(game, GAME_DRAW_MESHES))
         {
            struct mesh_t* mesh = resman_get_mesh(game->resman, node->data);
            world_render_mesh(game->world, game->camera, mesh, &node->transform);
         }
         break;
      }
      case NODE_CAMERA:
      {
         if (game_is_option_set(game, GAME_DRAW_CAMERAS))
         {
            struct camera_t* camera = world_get_camera(game->world, node->data);
            world_render_camera(game->world, game->camera, camera, &node->transform);
         }
         break;
      }
      case NODE_LAMP:
      {
         if (game_is_option_set(game, GAME_DRAW_LAMPS))
         {
            struct lamp_t* lamp = world_get_lamp(game->world, node->data);
            world_render_lamp(game->world, game->camera, lamp, &node->transform);
         }
         break;
      }
      }
   }

   if (game_is_option_set(game, GAME_DRAW_PHYSICS))
   {
      physworld_render(game->phys, game->camera);
   }

   gui_render(game->gui);
}

int game_init(game_t** pgame, const char* fname)
{
   world_t* world = NULL;

   if (world_load_from_file(&world, fname) != 0)
   {
      return -1;
   }

   gui_t* gui = NULL;
   if (gui_init(&gui) != 0)
   {
      world_free(world);
      return -1;
   }

   game_t* game = (game_t*)malloc(sizeof(game_t));
   memset(game, 0, sizeof(game_t));
   game->world = world;
   game->gui = gui;
   game_set_scene(game, /*world->scenes[0].name*/"w01d01s01");
   game_set_option(game, GAME_DRAW_MESHES | GAME_DRAW_LAMPS | GAME_UPDATE_PHYSICS);

   if (resman_init(&game->resman, game->world) != 0)
   {
      game_free(game);
      return -1;
   }

   (*pgame) = game;
   return 0;
}

void game_reset_physics(game_t* game)
{
   LOGI("game_reset_physics");
   if (game->bodies != NULL)
   {
      long l = 0;
      for (l = 0; l < game->scene->nnodes; ++l)
      {
         rigidbody_free(game->bodies[l]);
      }
      free(game->bodies);
      game->bodies = NULL;
   }

   if (game->phys != NULL)
   {
      physworld_free(game->phys);
      game->phys = NULL;
   }
}

void game_free(game_t* game)
{
   LOGI("game_free");
   game_reset_physics(game);

   if (game->resman != NULL)
   {
      resman_show(game->resman);

      LOGI("Releasing old resources");
      resman_free(game->resman);
      game->resman = NULL;
   }

   gui_free(game->gui);
   world_free(game->world);
   free(game);
}

void node_transform_setter(const rigidbody_t* b, const mat4f_t* transform, void* user_data)
{
   struct node_t* node = (struct node_t*)(user_data);
   //rigidbody_get_transform(b, &node->transform);
   node->transform = *transform;
}

void node_transform_getter(const rigidbody_t* b, mat4f_t* transform, void* user_data)
{
   struct node_t* node = (struct node_t*)(user_data);
   *transform = node->transform;
}

void game_set_scene(game_t* game, const char* scenename)
{
   LOGI("game_set_scene: '%s'", scenename);
   long l = 0;

   game_reset_physics(game);

   game->scene = world_get_scene(game->world, scenename);
   if (game->scene == NULL)
   {
      LOGE("Unable to find scene '%s'", scenename);
      return;
   }

   node_t* camera_node = scene_get_node(game->scene, game->scene->camera);
   if (camera_node == NULL)
   {
      LOGE("Unable to find camera node '%s'", game->scene->camera);
      return;
   }
   game->camera = world_get_camera(game->world, camera_node->data);

   if (physworld_create(&game->phys) != 0)
   {
      LOGE("Unable to create physworld");
      return;
   }

   struct camera_t* camera = game->camera;
   mat4_inverted(&camera->view, &camera_node->transform);
   mat4_set_perspective(&camera->proj, camera->fovy / 2.0f, camera->aspect, camera->znear, camera->zfar);

   game->bodies = (rigidbody_t**)malloc(game->scene->nnodes * sizeof(rigidbody_t*));
   memset(&game->bodies[0], 0, game->scene->nnodes * sizeof(rigidbody_t*));

   physworld_set_gravity(game->phys, &game->scene->gravity);
   struct node_t* node = &game->scene->nodes[0];
   for (l = 0; l < game->scene->nnodes; ++l, ++node)
   {
      if (node->phys.type == PHYS_NOCOLLISION)
         continue;

      struct mesh_t* mesh = world_get_mesh(game->world, node->data);

      LOGI("rigidbody_create");
      if (rigidbody_create(&game->bodies[l], &node->phys, mesh, node_transform_setter, node_transform_getter, node) == 0)
      {
         LOGI("physworld_add_rigidbody");
         physworld_add_rigidbody(game->phys, game->bodies[l]);
      }
   }
}

int game_is_option_set(const game_t* game, int option)
{
   return (game->game_options & option);
}

void game_set_option(game_t* game, int option)
{
   game->game_options |= option;
}

