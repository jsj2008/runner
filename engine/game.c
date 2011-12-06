#include "game.h"
#include "world.h"
#include "shader.h"
#include "common.h"
#include "resman.h"
#include "gl_defs.h"
#include <physics.h>
#include <timestamp.h>

camera_t* setup_camera(const struct game_t* game, const struct scene_t* scene, const char* camera_name)
{
   node_t* camera_node = scene_get_node(scene, camera_name);
   if (camera_node == NULL)
   {
      LOGE("Unable to find camera node '%s'", camera_name);
      return NULL;
   }

   camera_t* camera = world_get_camera(game->world, camera_node->data);
   mat4_inverted(&camera->view, &camera_node->transform);

   if (camera->type == CAMERA_PERSPECTIVE)
   {
      mat4_set_perspective(&camera->proj, camera->fovy / 2.0f, camera->aspect, camera->znear, camera->zfar);
   }
   else
   {
      float halfscale = 5.0f;
      camera->aspect = 1280.0f/800.0f;
      mat4_set_orthographic(&camera->proj, -halfscale, halfscale, -halfscale/camera->aspect, halfscale/camera->aspect, camera->znear, camera->zfar);
   }

   return camera;
}

void game_update(struct game_t* game, float dt)
{
   if (game_is_option_set(game, GAME_UPDATE_PHYSICS))
   {
      timestamp_t delta;
      timestamp_set(&delta);

      float interval = 1.0f / 60.0f;
      int maxSteps = (int)((dt / interval) + 1.0f);
      physics_world_step(game->phys, dt, maxSteps, interval);

      LOGD("Physics update time: %ld ms", timestamp_elapsed(&delta));
   }
}

#define MAX_LINES 8192
int nlines;
vec3f_t vertices[MAX_LINES * 2];
vec3f_t colors[MAX_LINES * 2];

void dd_draw_line(const vec3f_t* from, const vec3f_t* to, const vec3f_t* color)
{
   if (nlines >= MAX_LINES)
      return;

   vertices[nlines * 2] = *from;
   vertices[nlines * 2 + 1] = *to;
   colors[nlines * 2] = *color;
   colors[nlines * 2 + 1] = *color;
   ++nlines;
}

void game_render_physics(const struct game_t* game, const struct physics_world_t* world, const struct camera_t* camera)
{
   material_t* material = resman_get_material(game->resman, "PhysicsMaterial");
   if (material == NULL)
      return;

   shader_t* shader = resman_get_shader(game->resman, material->shader);
   if (shader == NULL)
      return;

   nlines = 0;
   physics_world_debug_draw(world);

   mat4f_t mvp;
   mat4_mult(&mvp, &camera->proj, &camera->view);

   shader_use(shader);
   shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, 0, &vertices[0]);
   shader_set_attrib_vertices(shader, "aColor", 3, GL_FLOAT, 0, &colors[0]);

   glLineWidth(2);
   glDrawArrays(GL_LINES, 0, nlines * 2);

   shader_unuse(shader);
}

void game_render(const struct game_t* game)
{
   timestamp_t delta;
   timestamp_set(&delta);

   game_render_scene(game, game->scene, game->camera);

   if (game_is_option_set(game, GAME_DRAW_PHYSICS))
   {
      game_render_physics(game, game->phys, game->camera);
   }

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   camera_t* gui_camera = setup_camera(game, game->gui.scene, game->gui.scene->camera);
   game_render_scene(game, game->gui.scene, gui_camera);

   glDisable(GL_BLEND);

   LOGD("Render time: %ld ms", timestamp_elapsed(&delta));
}

void game_render_scene(const struct game_t* game, const struct scene_t* scene, const struct camera_t* camera)
{
   long l = 0;
   struct node_t* node = &scene->nodes[0];
   for (l = 0; l < scene->nnodes; ++l, ++node)
   {
      switch (node->type)
      {
      case NODE_MESH:
      {
         if (game_is_option_set(game, GAME_DRAW_MESHES))
         {
            struct mesh_t* mesh = resman_get_mesh(game->resman, node->data);
            world_render_mesh(game->world, camera, mesh, &node->transform);
         }
         break;
      }
      case NODE_CAMERA:
      {
         if (game_is_option_set(game, GAME_DRAW_CAMERAS))
         {
            struct camera_t* cam = world_get_camera(game->world, node->data);
            world_render_camera(game->world, camera, cam, &node->transform);
         }
         break;
      }
      case NODE_LAMP:
      {
         if (game_is_option_set(game, GAME_DRAW_LAMPS))
         {
            struct lamp_t* lamp = world_get_lamp(game->world, node->data);
            world_render_lamp(game->world, camera, lamp, &node->transform);
         }
         break;
      }
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
   gui_reset(&game->gui);
   game->world = world;
   game->gui.scene = world_get_scene(world, "GUI_SCN_Default");;
   game_set_scene(game, /*world->scenes[0].name*/"w01d01s01");
   game_set_option(game, GAME_DRAW_MESHES | GAME_DRAW_LAMPS | GAME_UPDATE_PHYSICS);

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
         physics_rigid_body_delete(game->bodies[l]);
      }
      free(game->bodies);
      game->bodies = NULL;
   }

   if (game->phys != NULL)
   {
      physics_world_delete(game->phys);
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

int game_restore(game_t* game)
{
   LOGI("Restoring game resources");

   if (game->resman != NULL)
   {
      resman_free(game->resman);
   }
   return resman_init(&game->resman, game->world);
}

void node_transform_setter(const struct physics_rigid_body_t* b, const mat4f_t* transform, void* user_data)
{
   struct node_t* node = (struct node_t*)(user_data);
   //rigidbody_get_transform(b, &node->transform);
   node->transform = *transform;
}

void node_transform_getter(const struct physics_rigid_body_t* b, mat4f_t* transform, void* user_data)
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

   game->camera = setup_camera(game, game->scene, game->scene->camera);
   if (game->camera == NULL)
   {
      return;
   }

   vec3f_t worldMin = { -1000.0f, -1000.0f, -1000.0f };
   vec3f_t worldMax = { 1000.0f, 1000.0f, 1000.0f };
   if (physics_world_create(&game->phys, &worldMin, &worldMax, dd_draw_line) != 0)
   {
      LOGE("Unable to create physworld");
      return;
   }

   game->bodies = (struct physics_rigid_body_t**)malloc(game->scene->nnodes * sizeof(struct physics_rigid_body_t*));
   memset(&game->bodies[0], 0, game->scene->nnodes * sizeof(struct physics_rigid_body_t*));

   physics_world_set_gravity(game->phys, &game->scene->gravity);
   struct node_t* node = &game->scene->nodes[0];
   for (l = 0; l < game->scene->nnodes; ++l, ++node)
   {
      if (node->phys.type == PHYS_NOCOLLISION)
         continue;

      struct mesh_t* mesh = world_get_mesh(game->world, node->data);

      LOGI("physics_rigid_body_create");
      if (physics_rigid_body_create(&game->bodies[l], &node->phys, mesh, node_transform_setter, node_transform_getter, node) == 0)
      {
         LOGI("physics_world_add_rigid_body");
         physics_world_add_rigid_body(game->phys, game->bodies[l]);
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

void game_reset_option(game_t* game, int option)
{
   game->game_options &= ~option;
}

void game_toggle_option(game_t* game, int option)
{
   if (game_is_option_set(game, option))
   {
      game_reset_option(game, option);
   }
   else
   {
      game_set_option(game, option);
   }
}

