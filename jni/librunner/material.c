#include "material.h"
#include "common.h"
#include "resman.h"
#include "stream.h"
#include "tex2d.h"
#include "shader.h"

extern resman_t* g_resman;

struct material_t
{
   char name[64];
   char texture[64];
   char shader[64];

   int tex_min_filter;
   int tex_mag_filter;
   int tex_wrap_s;
   int tex_wrap_t;

   shader_t* pshader;
   tex2d_t* ptexture;
};

struct flag_t
{
   char name[32];
   int value;
};

static const char tag_texture[] = "texture";
static const char tag_shader[] = "shader";
static const char tag_tex_min_filter[] = "tex_min_filter";
static const char tag_tex_mag_filter[] = "tex_mag_filter";
static const char tag_tex_wrap_s[] = "tex_wrap_s";
static const char tag_tex_wrap_t[] = "tex_wrap_t";

static const struct flag_t flags[] =
{
   { "NEAREST", GL_NEAREST },
   { "LINEAR", GL_LINEAR },
   { "NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST },
   { "LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST },
   { "NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR },
   { "LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR },
   { "REPEAT", GL_REPEAT },
   { "CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE },
   { "MIRRORED_REPEAT", GL_MIRRORED_REPEAT },
};

int get_flag_value(const char* name)
{
   int i = 0;
   const struct flag_t* flag = &flags[0];
   for (i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i, ++flag)
   {
      if (strcmp(name, flag->name) == 0)
      {
         return flag->value;
      }
   }
   LOGE("Unable to find flag '%s'", name);
   return 0;
}

const char* get_flag_name(int value)
{
   int i = 0;
   const struct flag_t* flag = &flags[0];
   for (i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i, ++flag)
   {
      if (value == flag->value)
      {
         return flag->name;
      }
   }
   LOGE("Unable to find flag 0x%04X", value);
   return 0;
}

int material_load(material_t** pm, const char* fname)
{
   LOGI("Loading material from %s", fname);

   if (pm == NULL || fname == NULL)
   {
      LOGE("Invalid arguments");
      return -1;
   }

   long size = 0;
   char* buf = (char*)stream_read_file(fname, &size);
   if (buf == NULL)
   {
      return -1;
   }

   material_t* m = (material_t*)malloc(sizeof(material_t));
   memset(m, 0, sizeof(material_t));

   strncpy(m->name, fname, sizeof(m->name)-1);
   m->tex_min_filter = GL_LINEAR_MIPMAP_LINEAR;
   m->tex_mag_filter = GL_LINEAR;
   m->tex_wrap_s = GL_REPEAT;
   m->tex_wrap_t = GL_REPEAT;

   char* tmp = NULL;
   char* line = strtok_r(buf, "\n", &tmp);
   while (line != NULL)
   {
      char* tmp2 = NULL;
      char* tag = strtok_r(line, "=", &tmp2);
      if (tag != NULL)
      {
         char* value = strtok_r(NULL, "\n", &tmp2);
         if (strcmp(tag, tag_texture) == 0)
         {
            strncpy(m->texture, value, sizeof(m->texture)-1);
         }
         else if (strcmp(tag, tag_shader) == 0)
         {
            strncpy(m->shader, value, sizeof(m->shader)-1);
         }
         else if (strcmp(tag, tag_tex_min_filter) == 0)
         {
            m->tex_min_filter = get_flag_value(value);
         }
         else if (strcmp(tag, tag_tex_mag_filter) == 0)
         {
            m->tex_mag_filter = get_flag_value(value);
         }
         else if (strcmp(tag, tag_tex_wrap_s) == 0)
         {
            m->tex_wrap_s = get_flag_value(value);
         }
         else if (strcmp(tag, tag_tex_wrap_t) == 0)
         {
            m->tex_wrap_t = get_flag_value(value);
         }
         else
         {
            LOGE("Unknown tag: %s value: %s", tag, value);
         }
      }

      line = strtok_r(NULL, "\n", &tmp);
   }

   free(buf);

   m->ptexture = resman_get_texture(g_resman, m->texture);
   if (m->ptexture == NULL)
   {
      free(m);
      return -1;
   }
   tex2d_set_params(m->ptexture, m->tex_min_filter, m->tex_mag_filter, m->tex_wrap_s, m->tex_wrap_t);

   m->pshader = resman_get_shader(g_resman, m->shader);
   if (m->pshader == NULL)
   {
      free(m);
      return -1;
   }

   material_show(m);

   (*pm) = m;
   return 0;
}

void material_free(material_t* m)
{
   free(m);
}

void material_show(const material_t* m)
{
   LOGI("Material: %s", m->name);
   LOGI("\tShader: %s", m->shader);
   LOGI("\tTexture: %s", m->texture);
   LOGI("\t\tMin filter: %s", get_flag_name(m->tex_min_filter));
   LOGI("\t\tMag filter: %s", get_flag_name(m->tex_mag_filter));
   LOGI("\t\tWrap S: %s", get_flag_name(m->tex_wrap_s));
   LOGI("\t\tWrap T: %s", get_flag_name(m->tex_wrap_t));
}

void material_use(const material_t* m, int sampler_id)
{
   shader_use(m->pshader);
   shader_set_uniform_integers(m->pshader, "uTex", 1, &sampler_id);

   tex2d_bind(m->ptexture, sampler_id);
}

shader_t* material_get_shader(const material_t* m)
{
   return m->pshader;
}

