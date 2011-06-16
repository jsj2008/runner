#include "world.h"
#include "common.h"
#include "stream.h"
#include "shader.h"
#include "resman.h"
#include "bbox.h"
#include "game.h"

struct file_header_t
{
   char magic[8];
   long version;
   long data_offset;
   long data_size;
};

int world_load_from_file(world_t** pworld, const char* fname)
{
   LOGI("Loading world from %s", fname);

   stream_t* f = NULL;
   if (stream_open_reader(&f, fname) != 0)
   {
      return -1;
   }

   struct file_header_t header;
   stream_read(f, &header, sizeof(header));
   if (memcmp(header.magic, "RNNRWRLD", sizeof(header.magic)) != 0)
   {
      LOGE("Invalid file signature: %s", header.magic);
      stream_close(f);
      return -1;
   }

   long fsize = stream_size(f);

   if (header.data_offset + header.data_size < fsize)
   {
      LOGE("Invalid file size [offset: %ld size: %ld filesize: %ld]", header.data_offset, header.data_size, fsize);
      stream_close(f);
      return -1;
   }

   LOGI("Header [offset: %ld size: %ld]", header.data_offset, header.data_size);

   stream_seek(f, header.data_offset, SEEK_SET);
   char* data = malloc(header.data_size);
   stream_read(f, data, header.data_size);
   stream_close(f);

   return world_init(pworld, data);
}

int world_init(world_t** pworld, char* data)
{
   world_t* world = (world_t*)data;

   world->cameras = (struct camera_t*)(data + (long)world->cameras);
   world->materials = (struct material_t*)(data + (long)world->materials);
   world->textures = (struct texture_t*)(data + (long)world->textures);
   world->meshes = (struct mesh_t*)(data + (long)world->meshes);
   world->lamps = (struct lamp_t*)(data + (long)world->lamps);
   world->scenes = (struct scene_t*)(data + (long)world->scenes);

   LOGI("World %s has %ld cameras %ld materials %ld textures %ld meshes %ld lamps %ld scenes",
        world->name, world->ncameras, world->nmaterials, world->ntextures, world->nmeshes, world->nlamps, world->nscenes);

   long l = 0;
   long k = 0;

   struct camera_t* camera = &world->cameras[0];
   for (l = 0; l < world->ncameras; ++l, ++camera)
   {
      LOGI("Camera '%s' type %d [%.2f -> %.2f]", camera->name, camera->type, camera->znear, camera->zfar);
   }

   struct material_t* material = &world->materials[0];
   for (l = 0; l < world->nmaterials; ++l, ++material)
   {
      LOGI("Material '%s' has shader '%s' and texture '%s'", material->name, material->shader, material->texture);
   }

   struct texture_t* texture = &world->textures[0];
   for (l = 0; l < world->ntextures; ++l, ++texture)
   {
      LOGI("Texture '%s' path '%s'", texture->name, texture->path);
   }

   struct lamp_t* lamp = &world->lamps[0];
   for (l = 0; l < world->nlamps; ++l, ++lamp)
   {
      LOGI("Lamp '%s' distance %.2f energy %.2f type %d falloff_type %d", lamp->name, lamp->distance, lamp->energy, lamp->type, lamp->falloff_type);
   }

   struct mesh_t* mesh = &world->meshes[0];
   for (l = 0; l < world->nmeshes; ++l, ++mesh)
   {
      LOGI("Mesh '%s' has %ld submeshes", mesh->name, mesh->nsubmeshes);
      mesh->submeshes = (struct submesh_t*)(data + (long)mesh->submeshes);

      struct submesh_t* submesh = &mesh->submeshes[0];
      for (k = 0; k < mesh->nsubmeshes; ++k, ++submesh)
      {
         LOGI("\tSubmesh with material '%s' has %ld indices and %ld vertices", submesh->material, submesh->nindices, submesh->nvertices);
         submesh->indices = (unsigned int*)(data + (long)submesh->indices);
         submesh->vertices = (struct vertex_t*)(data + (long)submesh->vertices);
      }
   }

   struct scene_t* scene = &world->scenes[0];
   for (l = 0; l < world->nscenes; ++l, ++scene)
   {
      LOGI("Scene '%s' has %ld root nodes and camera '%s'", scene->name, scene->nnodes, scene->camera);
      scene->nodes = (struct node_t*)(data + (long)scene->nodes);

      struct node_t* node = &scene->nodes[0];
      for (k = 0; k < scene->nnodes; ++k, ++node)
      {
         LOGI("\tNode '%s' type %d parent '%s' and uses data '%s'", node->name, node->type, node->parent_index >= 0 ? scene->nodes[node->parent_index].name : "[null]", node->data);
         mat4_show(&node->transform);
      }
   }

   (*pworld) = world;
   return 0;
}

void world_free(world_t* world)
{
   free(world);
}

camera_t* world_get_camera(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->ncameras; ++l)
   {
      if (strcmp(world->cameras[l].name, name) == 0)
         return &world->cameras[l];
   }
   return NULL;
}

material_t* world_get_material(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->nmaterials; ++l)
   {
      if (strcmp(world->materials[l].name, name) == 0)
         return &world->materials[l];
   }
   return NULL;
}

texture_t* world_get_texture(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->ntextures; ++l)
   {
      if (strcmp(world->textures[l].name, name) == 0)
         return &world->textures[l];
   }
   return NULL;
}

mesh_t* world_get_mesh(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->nmeshes; ++l)
   {
      if (strcmp(world->meshes[l].name, name) == 0)
         return &world->meshes[l];
   }
   return NULL;
}

lamp_t* world_get_lamp(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->nlamps; ++l)
   {
      if (strcmp(world->lamps[l].name, name) == 0)
         return &world->lamps[l];
   }
   return NULL;
}

scene_t* world_get_scene(const world_t* world, const char* name)
{
   long l = 0;
   for (; l < world->nscenes; ++l)
   {
      if (strcmp(world->scenes[l].name, name) == 0)
         return &world->scenes[l];
   }
   return NULL;
}

node_t* scene_get_node(const scene_t* scene, const char* name)
{
   long l = 0;
   for (; l < scene->nnodes; ++l)
   {
      if (strcmp(scene->nodes[l].name, name) == 0)
         return &scene->nodes[l];
   }
   return NULL;
}

extern struct game_t* game;

void world_render_mesh(const world_t* world, const camera_t* camera, const mesh_t* mesh, const mat4f_t* transform)
{
   long l = 0;

   vec3f_t globalLightPos;
   globalLightPos.x = 4.0f;
   globalLightPos.y = -2.5f;
   globalLightPos.z = 6.0f;

   vec3f_t lightPos = globalLightPos;
   mat4_mult_vec3(&lightPos, &camera->view, &globalLightPos);

   struct submesh_t* submesh = &mesh->submeshes[0];
   for (; l < mesh->nsubmeshes; ++l, ++submesh)
   {
      material_t* material = resman_get_material(game->resman, submesh->material);
      shader_t* shader = resman_get_shader(game->resman, material->shader);

      mat4f_t mv;
      mat4f_t mvp;
      mat4_mult(&mv, &camera->view, transform);
      mat4_mult(&mvp, &camera->proj, &mv);

      mat4f_t mvi = *transform;
      mat4_inverted(&mvi, &mv);
      mat4_transpose(&mvi);
      mvi.m41 = mvi.m42 = mvi.m43 = 0.0f;
      mvi.m44 = 1.0f;

      material_bind(material, 0);

      shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
      shader_set_uniform_matrices(shader, "uMV", 1, mat4_data(&mv));
      shader_set_uniform_matrices(shader, "uMVI", 1, mat4_data(&mvi));
      shader_set_uniform_vectors(shader, "uLightPos", 1, &lightPos.x);
      shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, sizeof(vertex_t), &submesh->vertices[0].point);
      shader_set_attrib_vertices(shader, "aNormal", 3, GL_FLOAT, sizeof(vertex_t), &submesh->vertices[0].normal);
      shader_set_attrib_vertices(shader, "aTexCoord", 2, GL_FLOAT, sizeof(vertex_t), &submesh->vertices[0].uv);

      glDrawElements(GL_TRIANGLES, submesh->nindices, GL_UNSIGNED_INT, submesh->indices);

      material_unbind(material);

      /*bbox_t b;
      long k = 0;
      bbox_reset(&b);
      for (; k < submesh->nvertices; ++k)
      {
         vec3f_t p;
         mat4_mult_vec3(&p, transform, &submesh->vertices[k].point);
         bbox_inflate(&b, &p);
      }
      bbox_draw(&b, camera);*/
   }
}

void world_render_camera(const world_t* world, const camera_t* camera, const camera_t* cam, const mat4f_t* transform)
{
   if (camera == cam)
   {
      return;
   }

   // TODO
}

void world_render_lamp(const world_t* world, const camera_t* camera, const lamp_t* lamp, const mat4f_t* transform)
{
   // TODO
}

