#include "gl_defs.h"
#include "common.h"

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
      LOGE("%s caused glerror #%d", op, error);
   }
}

int isGLExtensionSupported(const char* name)
{
   const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
   return (strstr(extensions, name) != NULL);
}

