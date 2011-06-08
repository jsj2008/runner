#include "tex2d.h"
#include "image.h"
#include "common.h"

int tex2d_create(tex2d_t** ptexture, const struct image_t* image, int min_filter, int mag_filter, int wrap_s, int wrap_t)
{
   GLuint id = 0;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
   //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_HINT, GL_NICEST);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
   checkGLError("glTexImage2D");

   glGenerateMipmap(GL_TEXTURE_2D);
   checkGLError("glGenerateMipmap");

   LOGI("Loaded texture id = %d", id);

   (*ptexture) = (tex2d_t*)id;

   return 0;
}

void tex2d_free(tex2d_t* texture)
{
   GLuint id = (GLuint)texture;
   if (id > 0)
   {
      glDeleteTextures(1, &id);
   }
}

int tex2d_bind(tex2d_t* texture, int sampler)
{
   GLuint id = (GLuint)texture;

   glActiveTexture(GL_TEXTURE0 + sampler);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   return 0;
}

