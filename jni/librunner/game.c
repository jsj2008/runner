#include "game.h"
#include "physworld.h"
#include "world.h"
#include "shader.h"
#include "common.h"
#include "resman.h"

void game_update(struct game_t* game, float dt)
{
   physworld_update(game->phys, dt);

   long l = 0;
   struct node_t* node = &game->scene->nodes[0];
   for (l = 0; l < game->scene->nnodes; ++l, ++node)
   {
      if (game->bodies[l] != NULL)
      {
         rigidbody_get_transform(game->bodies[l], &node->transform);

         if (node->type == NODE_CAMERA)
         {
            struct camera_t* camera = world_get_camera(game->world, node->data);
            mat4_inverted(&camera->view, &node->transform);
            mat4_set_perspective(&camera->proj, camera->fovy, camera->aspect, camera->znear, camera->zfar);
         }
      }
   }
}

void game_render(const struct game_t* game)
{
   long l = 0;
   struct node_t* node = &game->scene->nodes[0];
   for (l = 0; l < game->scene->nnodes; ++l, ++node)
   {
      if (node->type == NODE_MESH)
      {
         struct mesh_t* mesh = resman_get_mesh(game->resman, node->data);
         world_render_mesh(game->world, game->camera, mesh, &node->transform);
      }
   }
}

int game_init(game_t** pgame, const char* fname)
{
   world_t* world = NULL;

   if (world_load_from_file(&world, fname) != 0)
   {
      return -1;
   }

   game_t* game = (game_t*)malloc(sizeof(game_t));
   memset(game, 0, sizeof(game_t));
   game->world = world;
   game_set_scene(game, world->scenes[0].name);

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

   world_free(game->world);
   free(game);
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
      if (rigidbody_create(&game->bodies[l], &node->phys, mesh, &node->transform) == 0)
      {
         LOGI("physworld_add_rigidbody");
         physworld_add_rigidbody(game->phys, game->bodies[l]);
      }
   }
}

