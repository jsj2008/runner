#include "gl.h"

void outGLString(const char* name, GLenum e)
{
   const char* str = (const char*)glGetString(e);
   LOGI("GL %s = %s", name, str);
}

void checkGLError(const char* op)
{
   GLint error = glGetError();
   for (; error != 0; error = glGetError())
   {
      LOGI("%s caused glerror #%d", op, error);
   }
}

GLuint gl_load_texture(tex2d_t* t)
{
   GLuint id = 0;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->width, t->height, 0, GL_RGB, GL_UNSIGNED_BYTE, t->data);
   checkGLError("glTexImage2D");

   LOGI("Loaded texture id = %d", id);

   return id;
}

GLuint gl_load_checkered_texture()
{
   unsigned char pixels[] =
   {
      127, 127, 127,
      0, 0, 0,
      0, 0, 0,
      127, 127, 127,
   };

   tex2d_t t;
   t.width = 2;
   t.height = 2;
   t.data = pixels;

   return gl_load_texture(&t);
}

