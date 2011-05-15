#include "resman.h"
#include "shader.h"
#include "tex2d.h"
#include "model.h"
#include "octree.h"
#include "material.h"

typedef struct entry_t
{
   char key[64];
   void* value;
} entry_t;

#define MAX_SHADERS  64
#define MAX_TEXTURES 64
#define MAX_MODELS   256
#define MAX_OCTREES  32
#define MAX_MATERIALS 64

struct resman_t
{
   long nshaders;
   long ntextures;
   long nmodels;
   long noctrees;
   long nmaterials;

   entry_t shaders[MAX_SHADERS];
   entry_t textures[MAX_TEXTURES];
   entry_t models[MAX_MODELS];
   entry_t octrees[MAX_OCTREES];
   entry_t materials[MAX_MATERIALS];
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

int resman_init(resman_t** prm)
{
   resman_t* rm = (resman_t*)malloc(sizeof(resman_t));
   memset(rm, 0, sizeof(resman_t));
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

   e = &rm->models[0];
   for (l = 0; l < rm->nmodels; ++l)
   {
      model_free((model_t*)e->value);
      ++e;
   }

   e = &rm->octrees[0];
   for (l = 0; l < rm->noctrees; ++l)
   {
      octree_free((octree_t*)e->value);
      ++e;
   }

   e = &rm->materials[0];
   for (l = 0; l < rm->nmaterials; ++l)
   {
      material_free((material_t*)e->value);
      ++e;
   }

   free(rm);
}

void resman_show(const resman_t* rm)
{
   LOGI("Resman [%ld shaders, %ld textures, %ld models, %ld octrees]", rm->nshaders, rm->ntextures, rm->nmodels, rm->noctrees);
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

   LOGI("\tModels:");
   e = &rm->models[0];
   for (l = 0; l < rm->nmodels; ++l)
   {
      LOGI("\t\t#%04ld:\t%s", l, e->key);
      ++e;
   }

   LOGI("\tOctrees:");
   e = &rm->octrees[0];
   for (l = 0; l < rm->noctrees; ++l)
   {
      LOGI("\t\t#%04ld:\t%s", l, e->key);
      ++e;
   }

   LOGI("\tMaterials:");
   e = &rm->materials[0];
   for (l = 0; l < rm->nmaterials; ++l)
   {
      LOGI("\t\t#%04ld:\t%s", l, e->key);
      ++e;
   }
}

shader_t* resman_get_shader(resman_t* rm, const char* name)
{
   shader_t* v = (shader_t*)entry_get(rm->shaders, rm->nshaders, name);
   if (v != NULL)
      return v;

   if (rm->nshaders >= MAX_SHADERS - 1)
   {
      LOGE("Shaders count overflow");
      return NULL;
   }

   if (shader_load(&v, name) != 0)
      return NULL;

   strcpy(rm->shaders[rm->nshaders].key, name);
   rm->shaders[rm->nshaders].value = v;
   ++rm->nshaders;

   return v;
}

struct tex2d_t* resman_get_texture(resman_t* rm, const char* name)
{
   tex2d_t* v = (tex2d_t*)entry_get(rm->textures, rm->ntextures, name);
   if (v != NULL)
      return v;

   if (rm->ntextures >= MAX_TEXTURES - 1)
   {
      LOGE("Textures count overflow");
      return NULL;
   }

   if (tex2d_load_from_png(&v, name) != 0)
      return NULL;

   strcpy(rm->textures[rm->ntextures].key, name);
   rm->textures[rm->ntextures].value = v;
   ++rm->ntextures;

   return v;
}

struct model_t* resman_get_model(resman_t* rm, const char* name)
{
   model_t* v = (model_t*)entry_get(rm->models, rm->nmodels, name);
   if (v != NULL)
      return v;

   if (rm->nmodels >= MAX_MODELS - 1)
   {
      LOGE("Models count overflow");
      return NULL;
   }

   if (model_load(&v, name) != 0)
      return NULL;

   strcpy(rm->models[rm->nmodels].key, name);
   rm->models[rm->nmodels].value = v;
   ++rm->nmodels;

   return v;
}

struct octree_t* resman_get_octree(resman_t* rm, const char* name)
{
   octree_t* v = (octree_t*)entry_get(rm->octrees, rm->noctrees, name);
   if (v != NULL)
      return v;

   if (rm->noctrees >= MAX_OCTREES - 1)
   {
      LOGE("Octrees count overflow");
      return NULL;
   }

   if (octree_load(&v, name) != 0)
      return NULL;

   strcpy(rm->octrees[rm->noctrees].key, name);
   rm->octrees[rm->noctrees].value = v;
   ++rm->noctrees;

   return v;
}

struct material_t* resman_get_material(resman_t* rm, const char* name)
{
   material_t* v = (material_t*)entry_get(rm->materials, rm->nmaterials, name);
   if (v != NULL)
      return v;

   if (rm->nmaterials >= MAX_MATERIALS - 1)
   {
      LOGE("materials count overflow");
      return NULL;
   }

   if (material_load(&v, name) != 0)
      return NULL;

   strcpy(rm->materials[rm->nmaterials].key, name);
   rm->materials[rm->nmaterials].value = v;
   ++rm->nmaterials;

   return v;
}

