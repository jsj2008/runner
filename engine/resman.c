#include "resman.h"
#include "shader.h"
#include "tex2d.h"
#include "world.h"
#include "image.h"
#include "common.h"

typedef struct entry_t
{
   char key[64];
   void* value;
} entry_t;

#define MAX_SHADERS  64
#define MAX_TEXTURES 64

struct resman_t
{
   const world_t* world;

   long nshaders;
   long ntextures;

   entry_t shaders[MAX_SHADERS];
   entry_t textures[MAX_TEXTURES];

   char texture_root[32];
};

static void* entry_get(const entry_t* entries, long nentries, const char* key)
{
   long l = 0;
   const entry_t* e = &entries[0];
   for (l = 0; l < nentries; ++l)
   {
      if (strcmp(e->key, key) == 0)
      {
         return e->value;
      }
      ++e;
   }
   return NULL;
}

char* modify_texture_path(char* modified, const char* root, const char* original)
{
   char* fname = strrchr(original, '/');
   if (fname == NULL)
   {
      return NULL;
   }

   ++fname;
   strncpy(modified, original, fname - original);
   strcat(modified, root);
   strcat(modified, fname);

   char* end = strrchr(modified, '.');
   strcpy(end, ".texture");

   return modified;
}

int add_texture(resman_t* rm, const struct texture_t* texture)
{
   if (resman_get_texture(rm, texture->name) == NULL)
   {
      image_t* image = NULL;

      char fname[256] = {0};
      modify_texture_path(fname, rm->texture_root, texture->path);

      if (image_load(&image, fname) != 0)
      {
         if (image_load_from_png(&image, texture->path) != 0)
         {
            return -1;
         }
      }

      tex2d_t* tex2d = NULL;
      if (tex2d_create(&tex2d, image, texture->min_filter, texture->mag_filter, texture->wrap_s, texture->wrap_t) != 0)
      {
         image_free(image);
         return -1;
      }

      image_free(image);

      strcpy(rm->textures[rm->ntextures].key, texture->name);
      rm->textures[rm->ntextures].value = tex2d;
      ++rm->ntextures;
   }
   return 0;
}

int add_shader(resman_t* rm, const char* name)
{
   shader_t* shader = resman_get_shader(rm, name);
   if (shader == NULL)
   {
      if (shader_load(&shader, name) != 0)
      {
         return -1;
      }

      strcpy(rm->shaders[rm->nshaders].key, name);
      rm->shaders[rm->nshaders].value = shader;
      ++rm->nshaders;
   }
   return 0;
}

int resman_init(resman_t** prm, const world_t* world)
{
   resman_t* rm = (resman_t*)malloc(sizeof(resman_t));
   memset(rm, 0, sizeof(resman_t));
   rm->world = world;

   if (isGLExtensionSupported("GL_EXT_texture_compression_s3tc"))
   {
      strcpy(rm->texture_root, "s3tc/");
   }
   else if (isGLExtensionSupported("GL_AMD_compressed_ATC_texture"))
   {
      strcpy(rm->texture_root, "atc/");
   }
   else if (isGLExtensionSupported("GL_IMG_texture_compression_pvrtc"))
   {
      strcpy(rm->texture_root, "pvrtc/");
   }
   else if (isGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture"))
   {
      strcpy(rm->texture_root, "etc/");
   }
   else
   {
      strcpy(rm->texture_root, "");
   }

   long l = 0;

   struct texture_t* texture = &world->textures[0];
   for (l = 0; l < world->ntextures; ++l, ++texture)
   {
      if (add_texture(rm, texture) != 0)
      {
         resman_free(rm);
         return -1;
      }
   }

   const struct material_t* material = &world->materials[0];
   for (l = 0; l < world->nmaterials; ++l, ++material)
   {
      if (add_shader(rm, material->shader) != 0)
      {
         resman_free(rm);
         return -1;
      }
   }

   (*prm) = rm;
   return 0;
}

void resman_free(resman_t* rm)
{
   long l = 0;
   entry_t* e = NULL;

   e = &rm->shaders[0];
   for (l = 0; l < rm->nshaders; ++l)
   {
      shader_free((shader_t*)e->value);
      ++e;
   }

   e = &rm->textures[0];
   for (l = 0; l < rm->ntextures; ++l)
   {
      tex2d_free((tex2d_t*)e->value);
      ++e;
   }

   free(rm);
}

void resman_show(const resman_t* rm)
{
   LOGI("Resman [%ld shaders, %ld textures]", rm->nshaders, rm->ntextures);
   long l = 0;
   const entry_t* e = NULL;

   LOGI("\tShaders:");
   e = &rm->shaders[0];
   for (l = 0; l < rm->nshaders; ++l)
   {
      LOGI("\t\t#%04ld:\t%s", l, e->key);
      ++e;
   }

   LOGI("\tTextures:");
   e = &rm->textures[0];
   for (l = 0; l < rm->ntextures; ++l)
   {
      LOGI("\t\t#%04ld:\t%s", l, e->key);
      ++e;
   }
}

shader_t* resman_get_shader(resman_t* rm, const char* name)
{
   return (shader_t*)entry_get(rm->shaders, rm->nshaders, name);
}

struct tex2d_t* resman_get_texture(resman_t* rm, const char* name)
{
   return (tex2d_t*)entry_get(rm->textures, rm->ntextures, name);
}

struct material_t* resman_get_material(resman_t* rm, const char* name)
{
   long l = 0;
   struct material_t* material = &rm->world->materials[0];
   for (l = 0; l < rm->world->nmaterials; ++l, ++material)
   {
      if (strcmp(material->name, name) == 0)
      {
         return material;
      }
   }
   return NULL;
}

struct mesh_t* resman_get_mesh(resman_t* rm, const char* name)
{
   long l = 0;
   struct mesh_t* mesh = &rm->world->meshes[0];
   for (l = 0; l < rm->world->nmeshes; ++l, ++mesh)
   {
      if (strcmp(mesh->name, name) == 0)
      {
         return mesh;
      }
   }
   return NULL;
}

